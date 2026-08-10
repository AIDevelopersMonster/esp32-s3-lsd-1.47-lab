@echo off
setlocal EnableExtensions

if "%~1"=="" (
  echo Usage: %~nx0 VERSION
  echo Example: %~nx0 0.1.0
  exit /b 2
)

set "VERSION=%~1"
set "REPO=%~dp0.."
set "SRC=%REPO%\firmware\KONTAKTS_ESP32_S3_LCD_1_47\build\esp32.esp32.esp32s3"
set "DST=%REPO%\firmware\KONTAKTS_ESP32_S3_LCD_1_47\release"
set "BASE=KONTAKTS_ESP32_S3_LCD_1_47"

set "APP_SRC=%SRC%\%BASE%.ino.bin"
set "MERGED_SRC=%SRC%\%BASE%.ino.merged.bin"
set "APP_DST=%DST%\%BASE%_v%VERSION%.bin"
set "MERGED_DST=%DST%\%BASE%_v%VERSION%.merged.bin"

if not exist "%APP_SRC%" (
  echo ERROR: application BIN not found:
  echo   %APP_SRC%
  exit /b 3
)

if not exist "%MERGED_SRC%" (
  echo ERROR: merged BIN not found:
  echo   %MERGED_SRC%
  exit /b 4
)

if not exist "%DST%" mkdir "%DST%"

copy /Y "%APP_SRC%" "%APP_DST%" >nul
if errorlevel 1 exit /b 5

copy /Y "%MERGED_SRC%" "%MERGED_DST%" >nul
if errorlevel 1 exit /b 6

echo Exported:
echo   %APP_DST%
echo   %MERGED_DST%
echo.
echo Next step:
echo   cd /d "%DST%"
echo   "%REPO%\tools\make-firmware-hashes.bat"

endlocal
