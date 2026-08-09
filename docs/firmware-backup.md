# Back up the factory firmware first

The Waveshare **ESP32-S3-LCD-1.47** is supplied with a useful factory diagnostic
firmware. Save a complete raw Flash image before uploading your own Arduino,
PlatformIO, or ESP-IDF firmware.

For the physical board tested in this project, `esptool v5.3.1` reports:

```text
Chip type: ESP32-S3 (QFN56) (revision v0.2)
Embedded PSRAM: 8 MB
Detected flash size: 16 MB
USB mode: USB-Serial/JTAG
```

The complete Flash image is therefore exactly:

```text
16 MiB = 16777216 bytes = 0x01000000 bytes
```

## Recommended method: project PowerShell script

Install or update Espressif esptool:

```powershell
py -m pip install --upgrade esptool
```

If PowerShell script execution is disabled on the PC, the script can be run
without changing the permanent system policy:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\tools\backup-factory.ps1 -Port COM16
```

Replace `COM16` with the actual port of the board.

If an incomplete or older backup file already exists and you intentionally want
to replace it, add `-Force`:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\tools\backup-factory.ps1 -Port COM16 -Force
```

`-Force` only permits replacement of the **file on the PC**. The backup script
itself remains read-only with respect to the ESP32-S3 Flash.

The script saves the complete image to:

```text
backup\esp32-s3-lcd-1.47-factory.bin
```

After a successful read it verifies that the file is exactly `16777216` bytes,
calculates SHA-256, and creates:

```text
backup\esp32-s3-lcd-1.47-factory.bin.sha256.txt
backup\esp32-s3-lcd-1.47-factory.bin.info.txt
```

The whole `backup/` directory is excluded from Git so device-specific binaries,
eFuse snapshots, MAC addresses, and other per-device data are not published
accidentally.

## Why the script uses `--no-stub`

On the tested ESP32-S3 board, full Flash reads through the normal esptool
flasher stub repeatedly stopped at approximately the same point:

```text
Reading from 0x00158000 ... 8.4% 1409024/16777216 bytes
A fatal error occurred: Packet content transfer stopped
```

The failure was reproduced at several requested baud rates, including `460800`,
`230400`, and a very low diagnostic rate. This made a simple serial-speed
problem unlikely.

A diagnostic read crossing the same Flash address succeeded when the flasher
stub was disabled:

```powershell
python -m esptool --chip esp32s3 --port COM16 `
    --before usb-reset --after no-reset --no-stub `
    read-flash --flash-size 16MB `
    0x00150000 0x00020000 `
    .\backup\images\read-cross-158000-nostub.bin
```

The resulting test file was exactly:

```text
131072 bytes
```

A complete 16 MiB read using the same ROM-bootloader method then completed
successfully:

```text
Read 16777216 bytes from 0x00000000 ...
```

For this reason the project backup script uses the ESP32-S3 ROM bootloader
directly with `--no-stub` instead of relying on the RAM flasher stub.

This is a workaround verified on the tested board/PC/esptool combination; it
should not be interpreted as evidence that every ESP32-S3 or every esptool
version has the same behavior.

## Manual full-backup command

The verified manual equivalent is:

```powershell
python -m esptool `
    --chip esp32s3 `
    --port COM16 `
    --before usb-reset `
    --after no-reset `
    --no-stub `
    read-flash `
    --flash-size 16MB `
    0x0 0x1000000 `
    .\backup\esp32-s3-lcd-1.47-factory.bin
```

This reads the complete physical Flash from `0x00000000` through the end of the
16 MiB device.

The ROM-bootloader method is slower than a normal stub-based read. On the tested
setup, the successful complete read took about 39 minutes. For a one-time
factory backup, reliability is more important than transfer speed.

## Verify the result manually

Check the file size:

```powershell
(Get-Item .\backup\esp32-s3-lcd-1.47-factory.bin).Length
```

Expected result:

```text
16777216
```

Calculate SHA-256:

```powershell
Get-FileHash .\backup\esp32-s3-lcd-1.47-factory.bin -Algorithm SHA256
```

Keep the `.bin` file and its SHA-256 checksum in at least two separate physical
or cloud locations.

## Verified factory image fingerprint for the tested board

Three independent complete 16 MiB reads of the same physical board were made:

```text
esp32-s3-lcd-1.47-factory-nostub1.bin
esp32-s3-lcd-1.47-factory-nostub2.bin
esp32-s3-lcd-1.47-factory.bin
```

All three files were exactly `16777216` bytes and produced the same SHA-256:

```text
E4CD5C150C4C90C3D0DE1DB375BFAF882CFB4A97776B5C7E17A45758D49BCA00
```

The pairwise comparisons were all identical:

```text
nostub1 == nostub2 : True
nostub1 == ps1     : True
nostub2 == ps1     : True
```

This fingerprint is the verified factory image hash for **this specific physical
board** and this captured factory state. It is not a universal hash for every
Waveshare ESP32-S3-LCD-1.47 board; another unit, firmware revision, calibration
state, or device-specific Flash content may legitimately produce a different
SHA-256.

For this repository, `backup\esp32-s3-lcd-1.47-factory.bin` is the canonical
local factory-backup filename, and the two `nostub` images served as independent
verification copies.

## After the backup

The verified command intentionally uses:

```text
--after no-reset
```

so esptool prints:

```text
Staying in bootloader.
```

Press **RESET** on the board or disconnect and reconnect it to start the factory
firmware again.

## Diagnostic files

Short Flash-read experiments can be stored under:

```text
backup\images\
```

For example:

```text
backup\images\read-cross-158000-nostub.bin
```

These files are also covered by the repository's `backup/` Git ignore rule.

## Safety notes

- `read-flash` is a read operation and does not erase or program the Flash.
- Do not run `erase-flash` before a verified factory backup exists.
- Do not assume an interrupted output file is a valid backup just because it exists.
- A valid full backup for this verified 16 MiB board must contain exactly `16777216` bytes.
- Preserve the SHA-256 checksum together with the backup metadata.
