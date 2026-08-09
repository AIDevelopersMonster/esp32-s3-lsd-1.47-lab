<#
.SYNOPSIS
    Creates a complete binary backup of the ESP32-S3 flash memory.

.DESCRIPTION
    This script reads the entire SPI Flash of the Waveshare
    ESP32-S3-LCD-1.47 board and saves it to the repository's local backup
    directory.

    The main purpose is to preserve the factory firmware BEFORE the board is
    erased or reflashed with Arduino, PlatformIO, ESP-IDF, or another tool.

    The script performs READ operations only. It does not erase or write the
    ESP32-S3 flash memory.

    The default output is resolved relative to the repository root, not the
    current PowerShell working directory. Therefore the same backup location is
    used whether the script is started from the repo root or from tools/.

    After the read completes, the script:
      - checks the resulting file size;
      - calculates SHA-256;
      - saves the SHA-256 and basic metadata next to the .bin file.

.PARAMETER Port
    Serial/USB COM port assigned to the ESP32-S3 board, for example COM16.

.PARAMETER Baud
    Serial transfer speed used by esptool while reading flash.

    Default:
        460800

    If the connection is unstable, try 115200.

.PARAMETER Output
    Optional output path for the complete raw flash image.

    If omitted, the default is:

        <repo>\backup\esp32-s3-lcd-1.47-factory.bin

    A relative custom path is interpreted relative to the repository root.

.PARAMETER Force
    Allows replacing an existing output file.

    Without -Force the script refuses to overwrite an existing factory backup.

.EXAMPLE
    .\tools\backup-factory.ps1 -Port COM16

.EXAMPLE
    powershell.exe -ExecutionPolicy Bypass -File .\tools\backup-factory.ps1 -Port COM16

.EXAMPLE
    .\tools\backup-factory.ps1 -Port COM16 -Baud 115200

.REQUIREMENTS
    - Windows PowerShell or PowerShell 7
    - Python available through the Windows `py` launcher
    - Espressif esptool installed (`py -m pip install esptool`)
    - ESP32-S3 board connected to the PC

.SAFETY
    This script is intended to be run BEFORE the first experimental firmware
    upload. The generated backup may be the only convenient copy of the
    original Waveshare factory demonstration firmware.

    Keep at least one additional copy outside the working repository.

.NOTES
    Project : Waveshare ESP32-S3-LCD-1.47 Lab
    File    : tools/backup-factory.ps1
    Mode    : READ ONLY
#>

param(
    [Parameter(Mandatory = $true)]
    [string]$Port,

    [int]$Baud = 460800,

    [string]$Output = "",

    [switch]$Force
)

$ErrorActionPreference = "Stop"

# Resolve repository root from this script's physical location:
#   <repo>\tools\backup-factory.ps1 -> <repo>
$repositoryRoot = Split-Path -Parent $PSScriptRoot

if ([string]::IsNullOrWhiteSpace($Output)) {
    $Output = Join-Path $repositoryRoot "backup\esp32-s3-lcd-1.47-factory.bin"
}
elif (-not [System.IO.Path]::IsPathRooted($Output)) {
    $Output = Join-Path $repositoryRoot $Output
}

$directory = Split-Path -Parent $Output
if ($directory -and -not (Test-Path $directory)) {
    New-Item -ItemType Directory -Path $directory -Force | Out-Null
}

if ((Test-Path $Output) -and -not $Force) {
    throw "Backup file already exists: $Output`nRefusing to overwrite it. Use -Force only if replacement is intentional."
}

Write-Host "Factory flash backup"
Write-Host "Board : Waveshare ESP32-S3-LCD-1.47"
Write-Host "Port  : $Port"
Write-Host "Baud  : $Baud"
Write-Host "Output: $Output"
Write-Host "Mode  : READ ONLY"
Write-Host ""

# First query the connected chip and Flash. This is informational and also
# verifies communication before starting the longer full-flash read.
Write-Host "Reading chip / Flash identification..."
py -m esptool --chip esp32s3 --port $Port flash-id
if ($LASTEXITCODE -ne 0) {
    throw "esptool flash-id failed with exit code $LASTEXITCODE"
}

Write-Host ""
Write-Host "Reading the complete flash..."

# Address 0 + size ALL means: start at 0x00000000 and let esptool detect and
# read the entire physical SPI Flash device.
py -m esptool --chip esp32s3 --port $Port --baud $Baud read-flash 0 ALL $Output
if ($LASTEXITCODE -ne 0) {
    throw "esptool read-flash failed with exit code $LASTEXITCODE"
}

if (-not (Test-Path $Output)) {
    throw "esptool reported success but the backup file was not found: $Output"
}

$file = Get-Item $Output
$hash = Get-FileHash -Algorithm SHA256 $Output

# This exact board has already been measured as having 16 MiB Flash. We do not
# use that value to control the read (ALL performs auto-detection), but we warn
# if the resulting raw image has an unexpected size.
$expectedBytes = 16MB
if ($file.Length -ne $expectedBytes) {
    Write-Warning "Unexpected backup size: $($file.Length) bytes. Expected 16777216 bytes for the verified 16 MiB board."
}

$hashFile = "$Output.sha256.txt"
$metadataFile = "$Output.info.txt"

$hash.Hash | Set-Content -Path $hashFile -Encoding ASCII

@(
    "Board: Waveshare ESP32-S3-LCD-1.47"
    "Chip: ESP32-S3"
    "Port used: $Port"
    "Baud used: $Baud"
    "Backup file: $([System.IO.Path]::GetFileName($Output))"
    "Backup size bytes: $($file.Length)"
    "Backup size MiB: $([Math]::Round($file.Length / 1MB, 2))"
    "SHA256: $($hash.Hash)"
    "Created: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss zzz')"
    "Read command: py -m esptool --chip esp32s3 --port $Port --baud $Baud read-flash 0 ALL <output>"
    "Operation: READ ONLY"
) | Set-Content -Path $metadataFile -Encoding UTF8

Write-Host ""
Write-Host "Backup completed successfully."
Write-Host "BIN    : $Output"
Write-Host "Size   : $($file.Length) bytes ($([Math]::Round($file.Length / 1MB, 2)) MiB)"
Write-Host "SHA256 : $($hash.Hash)"
Write-Host "Hash   : $hashFile"
Write-Host "Info   : $metadataFile"
Write-Host ""
Write-Host "Keep a second copy of the BIN and SHA-256 outside this repository."
