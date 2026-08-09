# Restore the saved factory firmware

This document describes how to restore a **complete raw 16 MiB Flash backup**
previously created from the Waveshare **ESP32-S3-LCD-1.47**.

> [!WARNING]
> Restoring firmware is a **write operation**. It overwrites the ESP32-S3 SPI
> Flash. Verify the backup file, its size and its SHA-256 before running the
> write command.

The backup procedure is documented here:

- [firmware-backup.md](firmware-backup