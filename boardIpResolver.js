const os = require('os');
const fs = require('fs');
const path = require('path');
const { execFileSync } = require('child_process');

const DATA_DIR = path.join(__dirname, 'data');
const CONFIG_FILE = path.join(DATA_DIR, 'board_config.json');
const DEFAULT_TARGET_IP = '10.0.30.81';

/**
 * Saves resolved board IP to persistent disk cache.
 * @param {string} boardIp
 * @param {string} source
 */
function saveCachedBoardIp(boardIp, source) {
    try {
        const currentCached = getCachedBoardIp();
        if (currentCached === boardIp) {
            return; // Already cached with same IP, avoid disk write & nodemon restarts
        }
        if (!fs.existsSync(DATA_DIR)) {
            fs.mkdirSync(DATA_DIR, { recursive: true });
        }
        const config = {
            boardIp,
            source,
            lastUpdated: new Date().toISOString()
        };
        fs.writeFileSync(CONFIG_FILE, JSON.stringify(config, null, 2), 'utf8');
    } catch (e) {
        // Silently ignore disk write issues (e.g. read-only permissions)
    }
}

/**
 * Reads last-known board IP from persistent disk cache.
 * @returns {string|null}
 */
function getCachedBoardIp() {
    try {
        if (fs.existsSync(CONFIG_FILE)) {
            const content = fs.readFileSync(CONFIG_FILE, 'utf8');
            const data = JSON.parse(content);
            if (data && data.boardIp && /^10\.0\.30\.\d{1,3}$/.test(data.boardIp)) {
                return data.boardIp;
            }
        }
    } catch (e) {}
    return null;
}

/**
 * Multi-Tier Industrial-Grade Board IP Resolver for Railway IPIS:
 * - Tier 1: Live OS network adapters (matches 10.x.0.X where 3rd octet is 0)
 * - Tier 2: Windows Registry static TCP/IP configurations (persists even during cable disconnect)
 * - Tier 3: Persistent disk cache (data/board_config.json)
 * - Tier 4: Standard default fallback (10.0.30.81)
 *
 * @param {string} [fallbackIp=DEFAULT_TARGET_IP]
 * @returns {string} Target Board IP (e.g. "10.0.30.31")
 */
function getDynamicBoardIp(fallbackIp = DEFAULT_TARGET_IP) {
    // -------------------------------------------------------------
    // Tier 1: Live OS Network Interfaces (when carrier is UP)
    // -------------------------------------------------------------
    try {
        const interfaces = os.networkInterfaces();
        for (const name of Object.keys(interfaces)) {
            for (const iface of interfaces[name] || []) {
                const isIPv4 = iface.family === 'IPv4' || iface.family === 4;
                if (isIPv4 && !iface.internal && iface.address) {
                    const octets = iface.address.split('.');
                    if (octets.length === 4 && octets[0] === '10' && octets[2] === '0') {
                        const boardIp = `10.0.30.${octets[3]}`;
                        saveCachedBoardIp(boardIp, `Live NIC: ${name} (${iface.address})`);
                        return boardIp;
                    }
                }
            }
        }
    } catch (err) {
        // Fall through to Tier 2
    }

    // -------------------------------------------------------------
    // Tier 2: Windows Registry Static TCP/IP Configuration
    // (Works 100% even if cable is disconnected / Media Disconnected)
    // -------------------------------------------------------------
    try {
        if (process.platform === 'win32') {
            const regOutput = execFileSync('reg', [
                'query',
                'HKLM\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces',
                '/s',
                '/v',
                'IPAddress'
            ], { encoding: 'utf8', timeout: 2000, windowsHide: true });

            const ipRegex = /\b10\.\d{1,3}\.0\.(\d{1,3})\b/g;
            let match;
            while ((match = ipRegex.exec(regOutput)) !== null) {
                const fourthOctet = match[1];
                if (fourthOctet !== '0' && fourthOctet !== '255') {
                    const boardIp = `10.0.30.${fourthOctet}`;
                    saveCachedBoardIp(boardIp, `Windows Registry Static Config (10.x.0.${fourthOctet})`);
                    return boardIp;
                }
            }
        }
    } catch (err) {
        // Fall through to Tier 3
    }

    // -------------------------------------------------------------
    // Tier 3: Persistent Local State Cache
    // -------------------------------------------------------------
    const cachedIp = getCachedBoardIp();
    if (cachedIp) {
        return cachedIp;
    }

    // -------------------------------------------------------------
    // Tier 4: Default Fallback
    // -------------------------------------------------------------
    return fallbackIp;
}

module.exports = {
    getDynamicBoardIp,
    getCachedBoardIp,
    saveCachedBoardIp,
    DEFAULT_TARGET_IP
};
