const fs = require('fs');
const path = require('path');
const { execFile } = require('child_process');

// কনফিগারেশন
const MEDIA_DIR = 'D:\\media';
const NATIVE_DIR = path.join(__dirname, 'native');
const EXE_PATH = path.join(NATIVE_DIR, 'ledsdk.exe');
const TARGET_IP = '10.0.30.81';
const TARGET_PORT = 5005;

// প্রতিটি ইমেজ ডিসপ্লেতে কত সেকেন্ড থাকবে (ভিডিওর ক্ষেত্রে ডিফল্ট ডিউরেশন)
const IMAGE_HOLD_TIME_MS = 10000; // 10 seconds
const VIDEO_PLAY_TIME_MS = 25000; // 25 seconds (প্রয়োজনে ভিডিওর দৈর্ঘ্য অনুযায়ী বাড়াতে পারো)

const SUPPORTED_IMAGE_EXTS = ['.png', '.jpg', '.jpeg', '.bmp'];
const SUPPORTED_VIDEO_EXTS = ['.mp4', '.avi', '.mkv'];

// স্লিপ হেল্পার
const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

/**
 * Onbon LED বোর্ডে নির্দিষ্ট মিডিয়া ফাইল পাঠানোর ফাংশন
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
 * D:\media ফোল্ডার স্ক্যান করে ভ্যালিড ফাইল লিস্ট বের করা
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
 * ইনফাইনাইট মিডিয়া লুপ রানার
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

                // ইমেজ বা ভিডিওর জন্য নির্ধারিত সময় অপেক্ষা
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

// এক্সপোর্ট এবং সরাসরি এক্সিকিউশন
module.exports = { startLinkFailLoop, sendMediaToBoard };

if (require.main === module) {
    startLinkFailLoop();
}