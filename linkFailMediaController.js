const fs = require('fs');
const path = require('path');
const { execFile } = require('child_process');

// Configuration
const MEDIA_DIR = 'D:\\media';
const NATIVE_DIR = path.join(__dirname, 'native');
const EXE_PATH = path.join(NATIVE_DIR, 'ledsdk.exe');
const TARGET_IP = '10.0.30.81';
const TARGET_PORT = 5005;

// Display duration for static images (and fallback duration for videos)
const IMAGE_HOLD_TIME_MS = 10000; // 10 seconds
const VIDEO_PLAY_TIME_MS = 25000; // 25 seconds (can be adjusted according to video length)

const SUPPORTED_IMAGE_EXTS = ['.png', '.jpg', '.jpeg', '.bmp'];
const SUPPORTED_VIDEO_EXTS = ['.mp4', '.avi', '.mkv'];

// Sleep helper promise
const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

/**
 * Sends a specific media file to the Onbon LED controller
 */
function sendMediaToBoard(filePath, ip = TARGET_IP, port = TARGET_PORT) {
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

    const files = fs.readdirSync(MEDIA_DIR);
    return files
        .filter((file) => {
            const ext = path.extname(file).toLowerCase();
            return SUPPORTED_IMAGE_EXTS.includes(ext) || SUPPORTED_VIDEO_EXTS.includes(ext);
        })
        .map((file) => path.join(MEDIA_DIR, file));
}

/**
 * Infinite loop runner to cycle through offline media playlist
 */
async function startLinkFailLoop() {
    console.log('====================================================');
    console.log('      IPIS Edge VDC - Link Fail Media Player        ');
    console.log(`      Scanning Directory: ${MEDIA_DIR}              `);
    console.log(`      Target Controller : ${TARGET_IP}:${TARGET_PORT}`);
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
            const file = mediaFiles[i];
            const fileName = path.basename(file);
            const ext = path.extname(file).toLowerCase();
            const isVideo = SUPPORTED_VIDEO_EXTS.includes(ext);

            console.log(`[LinkFail PLAYING ${i + 1}/${mediaFiles.length}] Type: ${isVideo ? 'VIDEO' : 'IMAGE'} -> ${fileName}`);

            try {
                await sendMediaToBoard(file);

                // Hold display for the designated image/video playback duration
                const waitTime = isVideo ? VIDEO_PLAY_TIME_MS : IMAGE_HOLD_TIME_MS;
                console.log(`[LinkFail] Holding on display for ${waitTime / 1000}s...`);
                await sleep(waitTime);

            } catch (err) {
                console.error(`[LinkFail ERROR] Skipping ${fileName} due to send failure.`);
                await sleep(2000);
            }
        }

        console.log('\n[LinkFail CYCLE COMPLETE] Restarting playlist from first item...\n');
    }
}

// Exports and direct execution handler
module.exports = { startLinkFailLoop, sendMediaToBoard };

if (require.main === module) {
    startLinkFailLoop();
}