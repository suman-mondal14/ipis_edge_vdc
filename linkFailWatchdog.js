const { exec, spawn } = require('child_process');
const path = require('path');

// কনফিগারেশন
const CDC_IP = '10.0.0.253';
const CHECK_INTERVAL_MS = 5000;      // প্রতি ৫ সেকেন্ড পর পর পিং চেক
const MAX_FAIL_COUNT = 2;              // পর পর ২ বার ফেইল হলে ফেইলওভার ট্রিগার হবে
const MEDIA_CONTROLLER_SCRIPT = path.join(__dirname, 'linkFailMediaController.js');

let mediaProcess = null;
let consecutiveFailures = 0;
let consecutiveSuccesses = 0;
let isFailoverActive = false;

/**
 * উইন্ডোজ পিং ফাংশন (১টি প্যাকেট পাঠিয়ে ১ সেকেন্ড অপেক্ষা)
 */
function pingCDC(ip) {
    return new Promise((resolve) => {
        exec(`ping -n 1 -w 1000 ${ip}`, (error, stdout) => {
            if (!error && stdout.includes('TTL=')) {
                resolve(true);
            } else {
                resolve(false);
            }
        });
    });
}

/**
 * Media Controller শুরু করার ফাংশন
 */
function startMediaController() {
    if (mediaProcess) return;

    console.log('\n[FAILOVER TRIGGERED] CDC Link Down! Launching Media Player...');

    mediaProcess = spawn('node', [MEDIA_CONTROLLER_SCRIPT], {
        cwd: __dirname,
        stdio: 'inherit',
        shell: true
    });

    mediaProcess.on('exit', (code) => {
        console.log(`[FAILOVER INFO] Media Player exited with code: ${code}`);
        mediaProcess = null;
    });

    isFailoverActive = true;
}

/**
 * Media Controller কিল / স্টপ করার ফাংশন
 */
function stopMediaController() {
    if (!mediaProcess) return;

    console.log('\n[LINK RESTORED] CDC Link is UP! Terminating Media Player...');

    if (process.platform === 'win32') {
        // উইন্ডোজে চাইল্ড প্রসেস ট্রি সহ কিল করা
        exec(`taskkill /pid ${mediaProcess.pid} /T /F`, (err) => {
            if (err) {
                console.warn('[WATCHDOG WARN] Taskkill fallback:', err.message);
            }
        });
    } else {
        mediaProcess.kill('SIGTERM');
    }

    mediaProcess = null;
    isFailoverActive = false;
}

/**
 * প্রধান ওয়াচডগ লুপ
 */
async function runWatchdog() {
    console.log('====================================================');
    console.log('       IPIS VDC Failover Watchdog Service           ');
    console.log(`       Target CDC Host : ${CDC_IP}                 `);
    console.log(`       Interval        : ${CHECK_INTERVAL_MS / 1000}s               `);
    console.log('====================================================\n');

    setInterval(async () => {
        const isAlive = await pingCDC(CDC_IP);

        if (isAlive) {
            consecutiveSuccesses++;
            consecutiveFailures = 0;

            // পিং ঠিক থাকলে এবং মিডিয়া প্লেয়ার চালু থাকলে বন্ধ করা
            if (isFailoverActive && consecutiveSuccesses >= 1) {
                stopMediaController();
            }
        } else {
            consecutiveFailures++;
            consecutiveSuccesses = 0;

            console.warn(`[WATCHDOG] Ping missed from ${CDC_IP} (${consecutiveFailures}/${MAX_FAIL_COUNT})`);

            // নির্ধারিত সংখ্যা ফেইল করলে মিডিয়া প্লেয়ার অন করা
            if (!isFailoverActive && consecutiveFailures >= MAX_FAIL_COUNT) {
                startMediaController();
            }
        }
    }, CHECK_INTERVAL_MS);
}

// Graceful Shutdown
process.on('SIGINT', () => {
    console.log('\n[WATCHDOG] Exiting watchdog...');
    stopMediaController();
    process.exit(0);
});

// ওয়াচডগ চালু
runWatchdog();