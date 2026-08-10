@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "OUT=SHA256SUMS.txt"
> "%OUT%" echo # SHA-256 firmware hashes

set "FOUND=0"
for %%F in (*.bin) do (
  set "FOUND=1"
  for /f "tokens=*" %%H in ('certutil -hashfile "%%F" SHA256 ^| findstr /R /V /C:"hash of file" /C:"CertUtil:"') do (
    set "HASH=%%H"
    set "HASH=!HASH: =!"
    if not "!HASH!"=="" >> "%OUT%" echo !HASH!  %%F
  )
)

if "%FOUND%"=="0" (
  echo ERROR: no .bin files found in %CD%
  del "%OUT%" >nul 2>nul
  exit /b 1
)

echo Created %OUT%
type "%OUT%"
endlocal
