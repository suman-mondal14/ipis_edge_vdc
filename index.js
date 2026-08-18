const express = require('express');
const multer = require('multer');
const cors = require('cors');
const fs = require('fs');
const path = require('path');
const { exec, spawn } = require('child_process');

// ================= কনফিগারেশন =================
const HTTP_PORT = 3500;
const MEDIA_DIR = 'D:\\media';
const CDC_IP = '10.0.0.253';
const PING_INTERVAL_MS = 5000;
const MAX_FAIL_COUNT = 2;
const MEDIA_CONTROLLER_SCRIPT = path.join(__dirname, 'linkFailMediaController.js');

// ডিরেক্টরি নিশ্চিত করা
if (!fs.existsSync(MEDIA_DIR)) {
    fs.mkdirSync(MEDIA_DIR, { recursive: true });
}

// ================= ১. HTTP MEDIA RECEIVER ENGINE =================
const app = express();
app.use(cors());
app.use(express.json());

const storage = multer.diskStorage({
    destination: (req, file, cb) => cb(null, MEDIA_DIR),
    filename: (req, file, cb) => cb(null, file.originalname)
});
const upload = multer({ storage });

app.get('/api/health', (req, res) => {
    res.json({
        status: "ONLINE",
        mode: "OFFLINE_LAN",
        targetDir: MEDIA_DIR,
        isFailoverActive,
        timestamp: new Date().toISOString()
    });
});

app.get('/api/media/list', (req, res) => {
    try {
        const files = fs.readdirSync(MEDIA_DIR);
        res.json({ success: true, files });
    } catch (e) {
        res.status(500).json({ success: false, error: e.message });
    }
});

app.post('/api/media/upload', upload.single('file'), (req, res) => {
    if (!req.file) return res.status(400).json({ success: false, message: 'No file uploaded' });
    console.log(`[VDC SYNC] Received & Saved: ${req.file.originalname}`);
    res.json({ success: true, filename: req.file.originalname });
});

app.post('/api/media/delete', (req, res) => {
    const { filename } = req.body;
    if (!filename) return res.status(400).json({ success: false, message: 'Filename required' });

    const filePath = path.join(MEDIA_DIR, filename);
    if (fs.existsSync(filePath)) {
        fs.unlinkSync(filePath);
        console.log(`[VDC CLEANUP] Deleted: ${filename}`);
        return res.json({ success: true, deleted: filename });
    }
    res.json({ success: false, message: 'File not found' });
});

app.listen(HTTP_PORT, '0.0.0.0', () => {
    console.log(`[HTTP API] Offline Media Receiver running on port ${HTTP_PORT}`);
});

// ================= ২. LINK FAILOVER WATCHDOG ENGINE =================
let mediaProcess = null;
let consecutiveFailures = 0;
let consecutiveSuccesses = 0;
let isFailoverActive = false;

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

function startMediaController() {
    if (mediaProcess) return;

    console.log('\n[FAILOVER TRIGGERED] CDC Link Down! Launching Media Player on LED Board...');

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

function stopMediaController() {
    if (!mediaProcess) return;

    console.log('\n[LINK RESTORED] CDC Link is UP! Terminating Media Player...');

    if (process.platform === 'win32') {
        exec(`taskkill /pid ${mediaProcess.pid} /T /F`, (err) => {
            if (err) console.warn('[WATCHDOG WARN] Taskkill fallback:', err.message);
        });
    } else {
        mediaProcess.kill('SIGTERM');
    }

    mediaProcess = null;
    isFailoverActive = false;
}

function startWatchdogLoop() {
    console.log(`[WATCHDOG] Monitoring CDC Host: ${CDC_IP} (Interval: ${PING_INTERVAL_MS / 1000}s)`);

    setInterval(async () => {
        const isAlive = await pingCDC(CDC_IP);

        if (isAlive) {
            consecutiveSuccesses++;
            consecutiveFailures = 0;

            if (isFailoverActive && consecutiveSuccesses >= 1) {
                stopMediaController();
            }
        } else {
            consecutiveFailures++;
            consecutiveSuccesses = 0;

            if (!isFailoverActive && consecutiveFailures >= MAX_FAIL_COUNT) {
                startMediaController();
            }
        }
    }, PING_INTERVAL_MS);
}

// ইঞ্জিন চালু করা
startWatchdogLoop();

// ক্লিন শাটডাউন
process.on('SIGINT', () => {
    stopMediaController();
    process.exit(0);
});