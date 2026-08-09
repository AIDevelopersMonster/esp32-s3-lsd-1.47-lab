<#
.SYNOPSIS
    Creates a complete binary backup of the ESP32-S3 flash memory.

.DESCRIPTION
    This script reads the entire SPI Flash of the Waveshare
    ESP32-S3-LCD-1.47 board and saves it to a binary file on the PC.

    The main purpose is to preserve the factory firmware BEFORE the board is
    erased or reflashed with Arduino, PlatformIO, ESP-IDF, or another tool.

    The script performs a READ operation only. It does not erase or write the
    ESP32-S3 flash memory.

    After the backup is complete, the script calculates the SHA-256 checksum
    of the resulting .bin file. Keep this checksum together with the backup;
    it can later be used to verify that the file has not been changed or
    corrupted.

    Internally the script calls Espressif esptool through Python:

        py -m esptool --chip esp32s3 --port <PORT> --baud <BAUD> \
            read-flash 0 ALL <OUTPUT_FILE>

.PARAMETER Port
    Serial/USB COM port assigned to the ESP32-S3 board.

    Examples:
        COM3
        COM7
        COM12

    This parameter is mandatory.

.PARAMETER Baud
    Serial transfer speed used by esptool while reading flash.

    Default:
        460800 baud

    If the connection is unstable, try a lower value such as 115200.

.PARAMETER Output
    Path and filename of the flash backup.

    Default:
        backup\esp32-s3-lcd-1.47-factory.bin

    If the destination directory does not exist, the script creates it
    automatically.

.EXAMPLE
    .\tools\backup-factory.ps1 -Port COM7

    Reads the complete flash using COM7 at the default 460800 baud and saves
    it to:

        backup\esp32-s3-lcd-1.47-factory.bin

.EXAMPLE
    .\tools\backup-factory.ps1 -Port COM7 -Baud 115200

    Uses a lower transfer speed. This can help with unreliable USB links,
    long extension cables, or some USB hubs.

.EXAMPLE
    .\tools\backup-factory.ps1 `
        -Port COM7 `
        -Output "backup\factory-before-first-flash.bin"

    Saves the flash image under a custom filename.

.REQUIREMENTS
    - Windows PowerShell or PowerShell 7
    - Python available through the Windows `py` launcher
    - Espressif esptool installed for that Python environment
    - ESP32-S3 board connected to the PC

    A typical esptool installation command is:

        py -m pip install esptool

.SAFETY
    This script is intended to be run BEFORE the first experimental firmware
    upload. The generated backup may be the only convenient copy of the
    original Waveshare factory demonstration firmware.

    Do not delete the backup after successfully creating it. Preferably keep
    a second copy outside the project directory.

.NOTES
    Project : Waveshare ESP32-S3-LCD-1.47 Lab
    File    : tools/backup-factory.ps1
    Purpose : Preserve the complete original flash image

    GitHub:
    https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
#>

# -----------------------------------------------------------------------------
# Command-line parameters
# -----------------------------------------------------------------------------
param(
    # COM port used by the ESP32-S3 USB/serial connection.
    # Mandatory because Windows may assign a different port on each PC.
    [Parameter(Mandatory = $true)]
    [string]$Port,

    # esptool transfer speed. 460800 is substantially faster than 115200,
    # while still being a reasonable default for most direct USB connections.
    [int]$Baud = 460800,

    # Destination file for the complete raw flash image.
    [string]$Output = "backup\esp32-s3-lcd-1.47-factory.bin"
)

# Treat PowerShell errors as terminating errors so the script does not silently
# continue after a failed operation such as directory creation.
$ErrorActionPreference = "Stop"

# -----------------------------------------------------------------------------
# Prepare the destination directory
# -----------------------------------------------------------------------------

# Extract the directory part from the requested output path.
# Example:
#   backup\factory.bin -> backup
$directory = Split-Path -Parent $Output

# Create the directory only when:
#   1. the output path actually contains a directory, and
#   2. that directory does not already exist.
if ($directory -and -not (Test-Path $directory)) {
    New-Item -ItemType Directory -Path $directory | Out-Null
}

# -----------------------------------------------------------------------------
# Read the ESP32-S3 flash
# -----------------------------------------------------------------------------

Write-Host "Reading the complete flash from $Port..."

# Run Espressif esptool through the Python launcher.
#
# --chip esp32s3
#     Explicitly tells esptool that the target is an ESP32-S3.
#
# --port $Port
#     Selects the Windows COM port supplied with -Port.
#
# --baud $Baud
#     Sets the data-transfer speed.
#
# read-flash 0 ALL $Output
#     Starts reading at flash address 0x00000000 and reads ALL available flash
#     into one raw binary image. On this board the expected flash size is 16 MB.
#
# IMPORTANT:
#     This is a READ operation. It does not erase or overwrite the board.
py -m esptool --chip esp32s3 --port $Port --baud $Baud read-flash 0 ALL $Output

# Native programs return an exit code. PowerShell stores the exit code of the
# last native command in $LASTEXITCODE. Zero means success; any non-zero value
# indicates that esptool reported an error.
if ($LASTEXITCODE -ne 0) {
    throw "esptool failed with exit code $LASTEXITCODE"
}

# -----------------------------------------------------------------------------
# Verify and identify the backup file
# -----------------------------------------------------------------------------

# Calculate a SHA-256 checksum. If another copy of the .bin file produces the
# same hash, the copies are byte-for-byte identical with overwhelming
# probability.
$hash = Get-FileHash -Algorithm SHA256 $Output

# Print the two most important results for the user:
#   - where the backup was saved;
#   - its SHA-256 checksum.
Write-Host "Backup completed: $Output"
Write-Host "SHA256: $($hash.Hash)"
