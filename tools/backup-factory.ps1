<#
.SYNOPSIS
    Creates a complete 16 MiB factory Flash backup of the ESP32-S3-LCD-1.47.

.DESCRIPTION
    Reads the entire SPI Flash in ROM bootloader mode using esptool --no-stub.

    This mode is used because on the tested ESP32-S3 (revision v0.2) with
    esptool 5.3.1, normal stub-based full reads repeatedly stopped near
    0x00158000 with "Packet content transfer stopped". The same region and the
    complete 16 MiB Flash were successfully read with --no-stub.

    This script is READ ONLY. It does not erase or write Flash.

.VIDEO
    Factory firmware backup demonstration:
    https://youtube.com/shorts/AqG7NoXsGEw

.PARAMETER Port
    COM port of the board, for example COM16.

.PARAMETER Output
    Optional output path. Relative paths are resolved from the repository root.
    Default: backup\esp32-s3-lcd-1.47-factory.bin

.PARAMETER Force
    Allows replacement of an existing output file.

.EXAMPLE
    powershell.exe -ExecutionPolicy Bypass -File .\tools\backup-factory.ps1 -Port COM16 -Force

.NOTES
    The verified Flash size for this board is 16 MiB (16777216 bytes).
    The command intentionally uses --after no-reset, so the board remains in
    the ROM bootloader after reading. Press RESET or reconnect it afterward.
#>

param(
    [Parameter(Mandatory = $true)]
    [string]$Port,

    [string]$Output = "",

    [switch]$Force
)

$ErrorActionPreference = "Stop"

$repositoryRoot = Split-Path -Parent $PSScriptRoot

if ([string]::IsNullOrWhiteSpace($Output)) {
    $Output = Join-Path $repositoryRoot "backup\esp32-s3-lcd-1.47-factory.bin"
}
elseif (-not [System.IO.Path]::IsPathRooted($Output)) {
    $Output = Join-Path $repositoryRoot $Output
}

$outputDirectory = Split-Path -Parent $Output
if ($outputDirectory -and -not (Test-Path $outputDirectory)) {
    New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
}

if ((Test-Path $Output) -and -not $Force) {
    throw "Backup file already exists: $Output`nUse -Force only if replacement is intentional."
}

Write-Host "Factory Flash backup"
Write-Host "Board : Waveshare ESP32-S3-LCD-1.47"
Write-Host "Port  : $Port"
Write-Host "Output: $Output"
Write-Host "Mode  : READ ONLY / ROM bootloader / --no-stub"
Write-Host "Size  : 16 MiB (0x01000000 bytes)"
Write-Host ""
Write-Host "Reading the complete Flash. This can take about 30-40 minutes..."
Write-Host ""

# Exact method verified successfully on the physical board:
#   --no-stub            use the ESP32-S3 ROM bootloader directly
#   --flash-size 16MB    explicitly declare the physical Flash size
#   0x0 0x1000000       read the complete 16 MiB address range
#   --after no-reset     leave the board in the bootloader after reading
py -m esptool `
    --chip esp32s3 `
    --port $Port `
    --before usb-reset `
    --after no-reset `
    --no-stub `
    read-flash `
    --flash-size 16MB `
    0x0 0x1000000 `
    $Output

if ($LASTEXITCODE -ne 0) {
    throw "esptool read-flash failed with exit code $LASTEXITCODE"
}

if (-not (Test-Path $Output)) {
    throw "esptool reported success but the output file was not found: $Output"
}

$file = Get-Item $Output
$expectedBytes = 16777216

if ($file.Length -ne $expectedBytes) {
    throw "Backup size check FAILED: $($file.Length) bytes; expected $expectedBytes bytes."
}

$hash = Get-FileHash -Algorithm SHA256 $Output
$hashFile = "$Output.sha256.txt"
$metadataFile = "$Output.info.txt"

$hash.Hash | Set-Content -Path $hashFile -Encoding ASCII

@(
    "Board: Waveshare ESP32-S3-LCD-1.47"
    "Chip: ESP32-S3"
    "Port used: $Port"
    "Method: ROM bootloader / esptool --no-stub"
    "Flash size: 16 MiB"
    "Backup file: $([System.IO.Path]::GetFileName($Output))"
    "Backup size bytes: $($file.Length)"
    "SHA256: $($hash.Hash)"
    "Created: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss zzz')"
    "Operation: READ ONLY"
) | Set-Content -Path $metadataFile -Encoding UTF8

Write-Host ""
Write-Host "Backup completed successfully."
Write-Host "BIN    : $Output"
Write-Host "Size   : $($file.Length) bytes (16 MiB)"
Write-Host "SHA256 : $($hash.Hash)"
Write-Host "Hash   : $hashFile"
Write-Host "Info   : $metadataFile"
Write-Host ""
Write-Host "The board is left in the ROM bootloader. Press RESET or reconnect it."
