const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');
const { getDynamicBoardIp, DEFAULT_TARGET_IP } = require('./boardIpResolver');

const nativeDir = path.join(__dirname, 'native');
const exePath = path.join(nativeDir, 'ledsdk.exe');
const defaultBgImage = path.join(__dirname, 'media', 'bgcolor.png');
const defaultVideo = path.join(__dirname, 'media', 'video.mp4');

/**
 * Sends text, image, or video payload to the Onbon VDC LED controller
 * @param {string} payload - Text message, image file path, or video file path (e.g. "12301", "D:\\IPIS_Edge_VDC\\media\\bgcolor.png", "D:\\IPIS_Edge_VDC\\media\\video.mp4")
 * @param {string} [ip] - Target VDC board IP address (Default: dynamically resolved via boardIpResolver, fallback 10.0.30.81)
 * @param {number} [port] - Target communication port (Default: 5005)
 * @returns {Promise<string>}
 */
function sendVdcMessage(payload, ip = getDynamicBoardIp(), port = 5005) {
    return new Promise((resolve, reject) => {
        if (!payload) {
            return reject(new Error('Payload cannot be empty'));
        }

        const args = [ip, port.toString(), payload];

        // Set cwd to 'native' folder so that dependent C++ DLLs are loaded properly
        execFile(exePath, args, { cwd: nativeDir }, (error, stdout, stderr) => {
            if (error) {
                console.error(`[VDC ERROR] Execution failed with code: ${error.code}`);
                if (stderr) console.error(`[VDC STDERR]: ${stderr.trim()}`);
                return reject(error);
            }

            console.log(`[VDC SUCCESS] Display updated [${ip}:${port}] -> "${payload}"`);
            resolve(stdout ? stdout.trim() : 'OK');
        });
    });
}

/**
 * Dedicated helper function to send an image to the Onbon VDC LED controller
 * @param {string} [imagePath] - Image file path (Default: media/bgcolor.png)
 * @param {string} [ip] - Target VDC board IP address (Default: dynamically resolved)
 * @param {number} [port] - Target communication port (Default: 5005)
 * @returns {Promise<string>}
 */
function sendVdcImage(imagePath = defaultBgImage, ip = getDynamicBoardIp(), port = 5005) {
    const resolvedPath = path.resolve(imagePath);
    if (!fs.existsSync(resolvedPath)) {
        return Promise.reject(new Error(`Image file not found: ${resolvedPath}`));
    }
    return sendVdcMessage(resolvedPath, ip, port);
}

/**
 * Dedicated helper function to send a video file (.mp4, .avi, .mkv, etc.) to the Onbon VDC LED controller
 * @param {string} [videoPath] - Video file path (Default: media/video.mp4)
 * @param {string} [ip] - Target VDC board IP address (Default: dynamically resolved)
 * @param {number} [port] - Target communication port (Default: 5005)
 * @returns {Promise<string>}
 */
function sendVdcVideo(videoPath = defaultVideo, ip = getDynamicBoardIp(), port = 5005) {
    const resolvedPath = path.resolve(videoPath);
    if (!fs.existsSync(resolvedPath)) {
        return Promise.reject(new Error(`Video file not found: ${resolvedPath}`));
    }
    return sendVdcMessage(resolvedPath, ip, port);
}

module.exports = { sendVdcMessage, sendVdcImage, sendVdcVideo, getDynamicBoardIp };

// Direct CLI execution handler
if (require.main === module) {
    const inputArg = process.argv[2] || '12301';
    const ipArg = process.argv[3] || getDynamicBoardIp();
    const portArg = process.argv[4] ? parseInt(process.argv[4], 10) : 5005;

    console.log(`[VDC NODE] Dispatching to ${ipArg}:${portArg} -> "${inputArg}"...`);

    sendVdcMessage(inputArg, ipArg, portArg)
        .then(() => process.exit(0))
        .catch(() => process.exit(1));
}