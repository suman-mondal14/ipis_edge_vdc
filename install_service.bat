@echo off
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] Please Run this script as Administrator!
    pause
    exit /b 1
)

set SERVICE_NAME=IPIS_Edge_VDC
set APP_DIR=D:\IPIS_Edge_VDC
set LOG_DIR=%APP_DIR%\logs

if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

"%APP_DIR%\nssm.exe" stop %SERVICE_NAME% >nul 2>&1
"%APP_DIR%\nssm.exe" remove %SERVICE_NAME% confirm >nul 2>&1

echo [INFO] Installing %SERVICE_NAME% as a Windows Background Service...

"%APP_DIR%\nssm.exe" install %SERVICE_NAME% "node.exe" "index.js"
"%APP_DIR%\nssm.exe" set %SERVICE_NAME% AppDirectory "%APP_DIR%"
"%APP_DIR%\nssm.exe" set %SERVICE_NAME% Start SERVICE_AUTO_START
"%APP_DIR%\nssm.exe" set %SERVICE_NAME% AppRestartDelay 2000
"%APP_DIR%\nssm.exe" set %SERVICE_NAME% AppExit Default Restart

"%APP_DIR%\nssm.exe" set %SERVICE_NAME% AppStdout "%LOG_DIR%\service_out.log"
"%APP_DIR%\nssm.exe" set %SERVICE_NAME% AppStderr "%LOG_DIR%\service_err.log"
"%APP_DIR%\nssm.exe" set %SERVICE_NAME% AppRotateFiles 1
"%APP_DIR%\nssm.exe" set %SERVICE_NAME% AppRotateOnline 1
"%APP_DIR%\nssm.exe" set %SERVICE_NAME% AppRotateBytes 5242880

"%APP_DIR%\nssm.exe" start %SERVICE_NAME%

echo ==========================================================
echo [SUCCESS] %SERVICE_NAME% installed and started successfully!
echo ==========================================================
pause