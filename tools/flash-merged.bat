@echo off
setlocal

if "%~2"=="" (
  echo Usage: %~nx0 COM7 path\to\firmware.merged.bin
  exit /b 2
)

set "PORT=%~1"
set "IMAGE=%~2"

if not exist "%IMAGE%" (
  echo ERROR: firmware image not found: %IMAGE%
  exit /b 3
)

echo Flashing ESP32-S3 on %PORT%
echo Image: %IMAGE%
py -m esptool --chip esp32s3 --port "%PORT%" --baud 921600 write-flash 0x0 "%IMAGE%"
if errorlevel 1 (
  echo ERROR: flashing failed.
  exit /b 1
)

echo.
echo Flash complete. Press RESET if the board does not restart automatically.
endlocal
