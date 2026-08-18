const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');

const nativeDir = path.join(__dirname, 'native');
const exePath = path.join(nativeDir, 'ledsdk.exe');
const defaultBgImage = path.join(__dirname, 'media', 'bgcolor.png');

/**
 * Onbon VDC LED বোর্ডে টেক্সট বা ইমেজ পাঠানোর ফাংশন
 * @param {string} payload - টেক্সট বা ইমেজ ফাইলের পাথ (e.g. "12301" বা "D:\\IPIS_Edge_VDC\\media\\bgcolor.png")
 * @param {string} ip - VDC বোর্ডের IP (Default: 10.0.30.81)
 * @param {number} port - পোর্ট (Default: 5005)
 * @returns {Promise<string>}
 */
function sendVdcMessage(payload, ip = '10.0.30.81', port = 5005) {
    return new Promise((resolve, reject) => {
        if (!payload) {
            return reject(new Error('Payload cannot be empty'));
        }

        const args = [ip, port.toString(), payload];

        // DLL সঠিকভাবে লোড হওয়ার জন্য cwd 'native' ফোল্ডার রাখা হয়েছে
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
 * Onbon VDC LED বোর্ডে ইমেজ পাঠানোর ডেডিকেটেড ফাংশন
 * @param {string} [imagePath] - ইমেজ ফাইলের পাথ (Default: media/bgcolor.png)
 * @param {string} [ip] - VDC বোর্ডের IP (Default: 10.0.30.81)
 * @param {number} [port] - পোর্ট (Default: 5005)
 * @returns {Promise<string>}
 */
function sendVdcImage(imagePath = defaultBgImage, ip = '10.0.30.81', port = 5005) {
    const resolvedPath = path.resolve(imagePath);
    if (!fs.existsSync(resolvedPath)) {
        return Promise.reject(new Error(`Image file not found: ${resolvedPath}`));
    }
    return sendVdcMessage(resolvedPath, ip, port);
}

module.exports = { sendVdcMessage, sendVdcImage };

// টার্মিনাল থেকে সরাসরি রান করার হ্যান্ডলার
if (require.main === module) {
    const inputArg = process.argv[2] || '12301';
    const ipArg = process.argv[3] || '10.0.30.81';
    const portArg = process.argv[4] ? parseInt(process.argv[4], 10) : 5005;

    console.log(`[VDC NODE] Dispatching to ${ipArg}:${portArg} -> "${inputArg}"...`);

    sendVdcMessage(inputArg, ipArg, portArg)
        .then(() => process.exit(0))
        .catch(() => process.exit(1));
}