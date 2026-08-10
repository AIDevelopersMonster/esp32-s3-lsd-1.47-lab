@echo off
setlocal EnableExtensions

if "%~2"=="" (
  echo Usage:
  echo   %~nx0 COM7 path\to\firmware.merged.bin
  echo   %~nx0 COM7 0.1.0
  exit /b 2
)

set "PORT=%~1"
set "ARG=%~2"

rem Normalize repository root from this script location, independent of current directory.
for %%I in ("%~dp0..") do set "REPO=%%~fI"

rem If argument is an existing file, use it directly.
if exist "%ARG%" (
  for %%I in ("%ARG%") do set "IMAGE=%%~fI"
) else (
  rem Otherwise treat argument as a KONTAKTS version number.
  set "IMAGE=%REPO%\firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v%ARG%.merged.bin"
)

if not exist "%IMAGE%" (
  echo ERROR: firmware image not found:
  echo   %IMAGE%
  echo.
  echo Pass either a valid merged BIN path or a release version such as 0.1.0.
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
