param(
    [Parameter(Mandatory = $true)]
    [string]$Port,

    [int]$Baud = 460800,

    [string]$Output = "backup\esp32-s3-lcd-1.47-factory.bin"
)

$ErrorActionPreference = "Stop"

$directory = Split-Path -Parent $Output
if ($directory -and -not (Test-Path $directory)) {
    New-Item -ItemType Directory -Path $directory | Out-Null
}

Write-Host "Reading the complete flash from $Port..."
py -m esptool --chip esp32s3 --port $Port --baud $Baud read-flash 0 ALL $Output

if ($LASTEXITCODE -ne 0) {
    throw "esptool failed with exit code $LASTEXITCODE"
}

$hash = Get-FileHash -Algorithm SHA256 $Output
Write-Host "Backup completed: $Output"
Write-Host "SHA256: $($hash.Hash)"
