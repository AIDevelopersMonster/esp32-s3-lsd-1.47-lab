@echo off
setlocal EnableExtensions

set "OUT=SHA256SUMS.txt"
> "%OUT%" echo # SHA-256 firmware hashes

set "FOUND=0"
for %%F in (*.bin) do (
  set "FOUND=1"
  for /f "usebackq delims=" %%H in (`powershell -NoProfile -Command "(Get-FileHash -LiteralPath '%%~fF' -Algorithm SHA256).Hash.ToLowerInvariant()"`) do (
    >> "%OUT%" echo %%H  %%F
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
