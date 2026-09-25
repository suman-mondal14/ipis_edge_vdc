const fs = require('fs');
const path = require('path');
const { execFile } = require('child_process');

const { getDynamicBoardIp, DEFAULT_TARGET_IP } = require('./boardIpResolver');

// ================= CONFIGURATION =================
const MEDIA_DIR = 'D:\\media';
const NATIVE_DIR = path.join(__dirname, 'native');
const EXE_PATH = path.join(NATIVE_DIR, 'ledsdk.exe');
const TARGET_PORT = 5005;

// Display duration settings
const IMAGE_HOLD_TIME_MS = 10000;        // 10 seconds fixed for static images
const DEFAULT_VIDEO_FALLBACK_MS = 15000; // 15 seconds fallback if video duration cannot be detected
const VIDEO_TRANSITION_LEAD_MS = 700;    // Lead time compensation (~700ms) to cut off loop cycle before trailing replay
const MIN_VIDEO_PLAY_TIME_MS = 1000;     // Minimum floor for video playback wait time

const SUPPORTED_IMAGE_EXTS = ['.png', '.jpg', '.jpeg', '.bmp'];
const SUPPORTED_VIDEO_EXTS = ['.mp4', '.avi', '.mkv'];

// Cache for video durations: filePath -> { mtimeMs, durationMs }
const videoDurationCache = new Map();

// Sleep helper promise
const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

/**
 * Extracts accurate media duration in milliseconds using Windows Shell COM object.
 * Zero external dependencies.
 *
 * @param {string} filePath - Absolute or relative path to media file
 * @returns {Promise<number|null>} Duration in milliseconds or null if detection failed
 */
function getVideoDurationMs(filePath) {
    return new Promise((resolve) => {
        const fullPath = path.resolve(filePath);
        const dir = path.dirname(fullPath);
        const base = path.basename(fullPath);

        // Escape single quotes for PowerShell single-quoted string literals
        const escapedDir = dir.replace(/'/g, "''");
        const escapedBase = base.replace(/'/g, "''");

        const psScript = `$sh = New-Object -ComObject Shell.Application; ` +
            `$folder = $sh.NameSpace('${escapedDir}'); ` +
            `if ($folder) { ` +
                `$file = $folder.ParseName('${escapedBase}'); ` +
                `if ($file) { ` +
                    `$dur = $file.ExtendedProperty('System.Media.Duration'); ` +
                    `if ($dur) { [math]::Round($dur / 10000) } ` +
                `} ` +
            `}`;

        const args = [
            '-NoProfile',
            '-NonInteractive',
            '-ExecutionPolicy', 'Bypass',
            '-Command', psScript
        ];

        execFile('powershell.exe', args, { windowsHide: true, timeout: 5000 }, (error, stdout, stderr) => {
            if (error) {
                console.warn(`[LinkFail WARN] Could not extract video duration for "${base}": ${error.message}`);
                return resolve(null);
            }

            const output = stdout ? stdout.trim() : '';
            const durationMs = parseInt(output, 10);

            if (!isNaN(durationMs) && durationMs > 0) {
                resolve(durationMs);
            } else {
                resolve(null);
            }
        });
    });
}

/**
 * Retrieves cached video duration or reads it from disk if modified/uncached.
 *
 * @param {string} filePath - Path to video file
 * @returns {Promise<number|null>}
 */
async function getCachedVideoDurationMs(filePath) {
    try {
        const stats = fs.statSync(filePath);
        const cached = videoDurationCache.get(filePath);

        if (cached && cached.mtimeMs === stats.mtimeMs) {
            return cached.durationMs;
        }

        const durationMs = await getVideoDurationMs(filePath);
        if (durationMs && durationMs > 0) {
            videoDurationCache.set(filePath, { mtimeMs: stats.mtimeMs, durationMs });
            return durationMs;
        }
    } catch (err) {
        console.warn(`[LinkFail WARN] Could not read file stats for "${filePath}": ${err.message}`);
    }
    return null;
}

/**
 * Sends a specific media file to the Onbon LED controller
 */
function sendMediaToBoard(filePath, ip = getDynamicBoardIp(), port = TARGET_PORT) {
    return new Promise((resolve, reject) => {
        const args = [ip, port.toString(), filePath];

        execFile(EXE_PATH, args, { cwd: NATIVE_DIR }, (error, stdout, stderr) => {
            if (error) {
                console.error(`[LinkFail ERROR] Execution failed for: ${path.basename(filePath)} | Code: ${error.code}`);
                if (stderr) console.error(`[STDERR]: ${stderr.trim()}`);
                return reject(error);
            }
            if (stdout) console.log(stdout.trim());
            resolve();
        });
    });
}

/**
 * Scans D:\media directory and returns a list of valid supported media files
 */
function getMediaFiles() {
    if (!fs.existsSync(MEDIA_DIR)) {
        console.error(`[LinkFail ERROR] Directory not found: ${MEDIA_DIR}`);
        return [];
    }

    try {
        const files = fs.readdirSync(MEDIA_DIR);
        return files
            .filter((file) => {
                const ext = path.extname(file).toLowerCase();
                return SUPPORTED_IMAGE_EXTS.includes(ext) || SUPPORTED_VIDEO_EXTS.includes(ext);
            })
            .map((file) => path.join(MEDIA_DIR, file));
    } catch (err) {
        console.error(`[LinkFail ERROR] Failed to read directory ${MEDIA_DIR}: ${err.message}`);
        return [];
    }
}

/**
 * Infinite loop runner to cycle through offline media playlist
 */
async function startLinkFailLoop() {
    const targetIp = getDynamicBoardIp();
    console.log('====================================================');
    console.log('      IPIS Edge VDC - Link Fail Media Player        ');
    console.log(`      Scanning Directory: ${MEDIA_DIR}              `);
    console.log(`      Target Controller : ${targetIp}:${TARGET_PORT}`);
    console.log('====================================================');

    let isRunning = true;

    // Graceful Shutdown
    process.on('SIGINT', () => {
        console.log('\n[LinkFail] Stopping media player loop...');
        isRunning = false;
        process.exit(0);
    });

    while (isRunning) {
        const mediaFiles = getMediaFiles();

        if (mediaFiles.length === 0) {
            console.warn(`[LinkFail WARN] No images/videos found in ${MEDIA_DIR}. Retrying in 5 seconds...`);
            await sleep(5000);
            continue;
        }

        console.log(`[LinkFail] Found ${mediaFiles.length} media item(s). Starting playlist cycle...\n`);

        for (let i = 0; i < mediaFiles.length; i++) {
            if (!isRunning) break;

            const file = mediaFiles[i];
            const fileName = path.basename(file);
            const ext = path.extname(file).toLowerCase();
            const isVideo = SUPPORTED_VIDEO_EXTS.includes(ext);

            console.log(`[LinkFail PLAYING ${i + 1}/${mediaFiles.length}] Type: ${isVideo ? 'VIDEO' : 'IMAGE'} -> ${fileName}`);

            try {
                // Determine precise display / playback hold duration
                let waitTime = IMAGE_HOLD_TIME_MS;

                if (isVideo) {
                    const rawDurationMs = await getCachedVideoDurationMs(file);
                    if (rawDurationMs && rawDurationMs > 0) {
                        // Subtract transition lead time (~700ms) to switch exactly on the final frame
                        waitTime = Math.max(rawDurationMs - VIDEO_TRANSITION_LEAD_MS, MIN_VIDEO_PLAY_TIME_MS);
                        console.log(`[LinkFail DURATION] ${fileName} -> Detected: ${rawDurationMs}ms (${(rawDurationMs / 1000).toFixed(2)}s) | Compensated Hold: ${waitTime}ms (${(waitTime / 1000).toFixed(2)}s)`);
                    } else {
                        waitTime = DEFAULT_VIDEO_FALLBACK_MS;
                        console.warn(`[LinkFail WARN] ${fileName} -> Duration detection unavailable. Fallback Hold: ${waitTime}ms (${(waitTime / 1000).toFixed(2)}s)`);
                    }
                } else {
                    console.log(`[LinkFail DURATION] ${fileName} -> Fixed Image Hold: ${waitTime}ms (${(waitTime / 1000).toFixed(2)}s)`);
                }

                // Send media command to LED board using dynamically resolved IP
                const currentBoardIp = getDynamicBoardIp();
                await sendMediaToBoard(file, currentBoardIp);

                // Hold display for the designated duration
                console.log(`[LinkFail] Holding on display for ${(waitTime / 1000).toFixed(2)}s...`);
                await sleep(waitTime);

            } catch (err) {
                console.error(`[LinkFail ERROR] Skipping ${fileName} due to send failure:`, err.message || err);
                await sleep(2000);
            }
        }

        console.log('\n[LinkFail CYCLE COMPLETE] Restarting playlist from first item...\n');
    }
}

// Exports and direct execution handler
module.exports = {
    startLinkFailLoop,
    sendMediaToBoard,
    getVideoDurationMs,
    getMediaFiles,
    getDynamicBoardIp,
    IMAGE_HOLD_TIME_MS,
    DEFAULT_VIDEO_FALLBACK_MS,
    VIDEO_TRANSITION_LEAD_MS
};

if (require.main === module) {
    startLinkFailLoop();
}