#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")

extern int SendToLedBoard(const char* ip, unsigned short port, const char* message, int effect, const char* border, int intensity, int speed, int chr, const char* lang);

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  IPIS Edge VDC - Onbon LED Controller  " << std::endl;
    std::cout << "  Target Resolution: 192x112 (BX-Y08)   " << std::endl;
    std::cout << "========================================" << std::endl;

    // Use CommandLineToArgvW to capture Unicode command-line arguments properly
    int wargc = 0;
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);

    if (!wargv || wargc < 4) {
        std::cerr << "Usage: ledsdk.exe <ip> <port> <message> [effect] [border] [intensity] [speed] [chr] [lang]\n"
                  << "       ledsdk.exe <ip> <port> <message> [color] [effect] [border] [intensity] [speed] [chr] [lang]\n"
                  << "Example: ledsdk.exe 192.168.1.100 5005 \"WELCOME TO PLATFORM 1\" 1 FFFFFF 100 2 0 en"
                  << std::endl;
        if (wargv) LocalFree(wargv);
        return 1;
    }

    char ip[128] = {0};
    WideCharToMultiByte(CP_UTF8, 0, wargv[1], -1, ip, sizeof(ip), NULL, NULL);
    unsigned short port = static_cast<unsigned short>(_wtoi(wargv[2]));

    // Convert Unicode message argument to UTF-8
    int msgLen = WideCharToMultiByte(CP_UTF8, 0, wargv[3], -1, NULL, 0, NULL, NULL);
    std::string message;
    if (msgLen > 0) {
        message.resize(msgLen - 1);
        WideCharToMultiByte(CP_UTF8, 0, wargv[3], -1, &message[0], msgLen, NULL, NULL);
    }

    int effect = 1;
    char border[64] = "FFFFFF";
    int intensity = 100;
    int speed = 2;
    int chr = 0;
    char lang[32] = "en";

    // Handle optional arguments, accommodating both:
    // Format A: <ip> <port> <message> [effect] [border] [intensity] [speed] [chr] [lang]
    // Format B: <ip> <port> <message> [color] [effect] [border] [intensity] [speed] [chr] [lang]
    int argIdx = 4;
    if (wargc > argIdx) {
        std::wstring arg4 = wargv[argIdx];
        // If arg4 is NOT a pure number / hex number (e.g. "yellow", "red", "#FF0000"), skip color argument
        bool isNumber = true;
        for (size_t i = 0; i < arg4.length(); ++i) {
            if (i == 0 && (arg4[i] == L'+' || arg4[i] == L'-')) continue;
            if (!iswdigit(arg4[i])) {
                isNumber = false;
                break;
            }
        }
        if (!isNumber && (arg4.rfind(L"0x", 0) != 0 && arg4.rfind(L"0X", 0) != 0)) {
            // It's a color string (e.g. "yellow", "red"), so advance to next argument for effect
            argIdx++;
        }
    }

    if (wargc > argIdx) {
        std::wstring effArg = wargv[argIdx];
        if (effArg.rfind(L"0x", 0) == 0 || effArg.rfind(L"0X", 0) == 0) {
            effect = (int)wcstol(effArg.c_str(), NULL, 16);
        } else {
            effect = _wtoi(effArg.c_str());
            if (effect == 0 && effArg != L"0") effect = 1;
        }
        argIdx++;
    }

    if (wargc > argIdx) {
        WideCharToMultiByte(CP_UTF8, 0, wargv[argIdx], -1, border, sizeof(border), NULL, NULL);
        argIdx++;
    }

    if (wargc > argIdx) {
        intensity = _wtoi(wargv[argIdx]);
        argIdx++;
    }

    if (wargc > argIdx) {
        std::wstring spdArg = wargv[argIdx];
        if (spdArg.rfind(L"0x", 0) == 0 || spdArg.rfind(L"0X", 0) == 0) {
            speed = (int)wcstol(spdArg.c_str(), NULL, 16);
        } else {
            speed = _wtoi(spdArg.c_str());
        }
        argIdx++;
    }

    if (wargc > argIdx) {
        chr = _wtoi(wargv[argIdx]);
        argIdx++;
    }

    if (wargc > argIdx) {
        WideCharToMultiByte(CP_UTF8, 0, wargv[argIdx], -1, lang, sizeof(lang), NULL, NULL);
        argIdx++;
    }

    std::cout << "[MAIN] Target: " << ip << ":" << port << std::endl;
    std::cout << "[MAIN] Message: " << message << std::endl;
    std::cout << "[MAIN] Effect: " << effect << " | Speed: " << speed << " | Intensity: " << intensity << "% | Lang: " << lang << std::endl;

    int result = SendToLedBoard(ip, port, message.c_str(), effect, border, intensity, speed, chr, lang);

    if (result == 0) {
        std::cout << "[SUCCESS] Data sent successfully to LED board." << std::endl;
    } else {
        std::cerr << "[ERROR] SendToLedBoard failed with error code: " << result << std::endl;
    }

    LocalFree(wargv);
    return result;
}