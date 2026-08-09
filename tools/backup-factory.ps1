<#
.SYNOPSIS
    Creates a complete binary backup of the ESP32-S3 flash memory.

.DESCRIPTION
    This script reads the entire 16 MiB SPI Flash of the Waveshare
    ESP32-S3-LCD-1.47 board and saves it to the repository's local backup
    directory.

    The main purpose is to preserve the factory firmware BEFORE the board is
    erased or reflashed with Arduino, PlatformIO, ESP-IDF, or another tool.

    The script performs READ operations only