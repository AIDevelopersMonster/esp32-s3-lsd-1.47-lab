<#
.SYNOPSIS
    Reads the ESP32-S3 eFuse state and saves local snapshots.

.DESCRIPTION
    This script uses Espressif `espefuse` in READ-ONLY mode to collect the
    current eFuse state of the connected ESP32-S3.

    It saves three files:
      - a full human-readable summary;
      - an active-fields-only summary;
      - a machine-readable JSON summary.

    No eFuse burn/write command is used by this script.

    IMPORTANT: eFuses are one-time-programmable. Reading them is safe, but
    commands such as burn-efuse, burn-key, write-protect-efuse and related
    operations can make permanent changes. This script intentionally does not
    expose any write operation.

.PARAMETER Port
    Windows COM port assigned to the board, for example COM16.

.PARAMETER OutputDirectory
    Directory in which the snapshot files are stored.

    Default:
        backup\efuses

    The project .gitignore excludes backup/, so raw per-device data is not
    committed accidentally.

.EXAMPLE
    .\tools\read-efuses.ps1 -Port COM16

.REQUIREMENTS
    - Windows PowerShell or PowerShell 7
    - Python available through the `py` launcher
    - Espressif esptool/espefuse installed (`py -m pip install esptool`)

.NOTES
    Project : Waveshare ESP32-S3-LCD-1.47 Lab
    File    : tools/read-efuses.ps1
    Mode    : READ ONLY
#>

param(
    [Parameter(Mandatory = $true)]
    [string]$Port,

    [string]$OutputDirectory = "backup\efuses"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $OutputDirectory)) {
    New-Item -ItemType Directory -Path $OutputDirectory | Out-Null
}

$timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
$summaryFile = Join-Path $OutputDirectory "efuses-$timestamp-summary.txt"
$activeFile  = Join-Path $OutputDirectory "efuses-$timestamp-active.txt"
$jsonFile    = Join-Path $OutputDirectory "efuses-$timestamp-summary.json"

Write-Host "Reading ESP32-S3 eFuses from $Port..."
Write-Host "This script performs READ operations only."
Write-Host ""

# Full human-readable summary.
py -m espefuse --chip esp32s3 --port $Port summary --format summary --file $summaryFile
if ($LASTEXITCODE -ne 0) {
    throw "espefuse summary failed with exit code $LASTEXITCODE"
}

# Shorter view containing only active/non-zero/protected/error fields.
py -m espefuse --chip esp32s3 --port $Port summary --active --format summary --file $activeFile
if ($LASTEXITCODE -ne 0) {
    throw "espefuse active summary failed with exit code $LASTEXITCODE"
}

# Machine-readable snapshot for later comparison or automated analysis.
py -m espefuse --chip esp32s3 --port $Port summary --format json --file $jsonFile
if ($LASTEXITCODE -ne 0) {
    throw "espefuse JSON summary failed with exit code $LASTEXITCODE"
}

Write-Host ""
Write-Host "eFuse snapshot completed."
Write-Host "Full summary : $summaryFile"
Write-Host "Active fields: $activeFile"
Write-Host "JSON summary : $jsonFile"
Write-Host ""
Write-Host "NOTE: These files may contain a unique MAC address and security-related"
Write-Host "eFuse fields. The backup directory is ignored by Git. Review and redact"
Write-Host "per-device data before publishing any snapshot as a public example."
