# Restore the saved factory firmware

This document describes how to restore a **complete raw 16 MiB Flash backup**
previously created from the Waveshare **ESP32-S3-LCD-1.47**.

The matching backup procedure is here:

- [firmware-backup.md](firmware-backup.md)

> [!WARNING]
> Restoring firmware is a **write operation**. It overwrites the ESP32-S3 SPI
> Flash. Do not run the restore command until the backup file has been checked
> for the expected size and SHA-256.

## Hardware-verified status

The complete 16 MiB factory-image restore has now been performed successfully on
the physical project board with **esptool v5.3.1**.

The verified restore wrote the canonical image from Flash offset `0x00000000`
through `0x00ffffff` and esptool reported:

```text
Flash will be erased from 0x00000000 to 0x00ffffff...
Wrote 16777216 bytes (1018650 compressed) at 0x00000000 in 99.9 seconds.
Hash of data verified.
Hard resetting via RTS pin...
```

The board's factory firmware was restored successfully after the reset.

This establishes the **restore/write path as hardware-verified** for the tested
board and saved image.

A separate full 16 MiB post-restore read-back and SHA-256 comparison remains the
strongest optional byte-for-byte verification. It is not required to call the
restore successful because esptool already verified the data it programmed, but
it can provide an independent end-to-end confirmation of the entire Flash image.

### Important observation about the flasher stub

Full Flash **reads** through the normal flasher stub were unreliable on this
board and repeatedly stopped near `0x00158000`, which is why factory backup uses
`--no-stub`.

The full 16 MiB **write**, however, succeeded with the normal esptool flasher
stub and passed esptool's data-hash verification. Therefore the observed stub
problem should not be generalized from reads to writes.

## What the backup contains

The canonical local backup created by this project is:

```text
backup\esp32-s3-lcd-1.47-factory.bin
```

It is a raw dump of the complete 16 MiB external SPI Flash, beginning at address
`0x00000000`.

For the physical board used in this project, three independent reads produced
exactly the same 16 MiB image. Its recorded SHA-256 is:

```text
E4CD5C150C4C90C3D0DE1DB375BFAF882CFB4A97776B5C7E17A45758D49BCA00
```

This hash is specific to **that physical board and captured factory state**. Do
not expect another board to have the same hash.

## What this restore does and does not restore

Writing the complete raw image back to offset `0x00000000` restores the contents
of the external SPI Flash represented by the backup image.

It does **not** restore or modify:

- eFuse values;
- the chip's factory MAC/eFuse identity;
- data on the microSD card;
- anything outside the external SPI Flash image.

eFuse is one-time-programmable storage and is intentionally outside this restore
workflow.

## 1. Work from the repository root

Open PowerShell in the repository root, for example:

```text
C:\Users\CHUWI\Documents\GitHub\lab-esp32-s3-lcd-1.47
```

The commands below assume that the backup is stored at:

```text
.\backup\esp32-s3-lcd-1.47-factory.bin
```

## 2. Check that the backup file exists

Run:

```powershell
Get-Item .\backup\esp32-s3-lcd-1.47-factory.bin
```

Do not continue if the intended backup file is missing or you are unsure which
image should be restored.

## 3. Verify the exact file size

Run:

```powershell
(Get-Item .\backup\esp32-s3-lcd-1.47-factory.bin).Length
```

For this board the expected result is:

```text
16777216
```

That is exactly 16 MiB (`0x01000000` bytes).

Do not write a partial or interrupted backup.

## 4. Verify SHA-256

Run:

```powershell
Get-FileHash .\backup\esp32-s3-lcd-1.47-factory.bin -Algorithm SHA256
```

For the original project board and the canonical captured image, the expected
hash is:

```text
E4CD5C150C4C90C3D0DE1DB375BFAF882CFB4A97776B5C7E17A45758D49BCA00
```

If restoring another physical board from its own backup, compare against that
board's saved `.sha256.txt` file instead.

If the checksum does not match the checksum recorded when the backup was made,
**stop** and investigate before writing Flash.

## 5. Identify the COM port

Connect the board and determine its current serial port in Windows or Arduino
IDE. Example:

```text
COM16
```

Replace `COM16` below with the actual port.

## 6. Confirm esptool is available

The project used esptool v5.3.1 during the verified backup and restore work.

Check the installed version:

```powershell
py -m esptool version
```

You can inspect the exact options supported by the installed version with:

```powershell
py -m esptool write-flash -h
```

## 7. Restore the complete raw Flash image

The command hardware-verified on the project board is:

```powershell
py -m esptool `
    --chip esp32s3 `
    --port COM16 `
    write-flash `
    0x0 .\backup\esp32-s3-lcd-1.47-factory.bin
```

Replace `COM16` with the actual port.

This is the **destructive step**. esptool erases the Flash sectors covered by the
write and programs the supplied image into them.

Do not run `erase-flash` separately for this normal restore. `write-flash`
already erases the sectors it needs before programming them.

### Why no explicit `--flash-mode` or `--flash-size` override is used

For an exact saved image, the restore command intentionally avoids forcing new
Flash mode/size header values. The goal is to write back the captured raw image
rather than reinterpret it with new build-time settings.

## 8. Verified successful output

On the physical project board, esptool v5.3.1 connected to the ESP32-S3, started
the flasher stub, erased the complete 16 MiB Flash address range, wrote all
`16777216` bytes and verified the programmed data hash.

The key successful lines were:

```text
Stub flasher running.
Configuring flash size...
Flash will be erased from 0x00000000 to 0x00ffffff...
Wrote 16777216 bytes (1018650 compressed) at 0x00000000 in 99.9 seconds.
Hash of data verified.
Hard resetting via RTS pin...
```

Unique per-device identifiers printed by esptool are intentionally omitted from
public documentation.

After the hard reset, the saved factory firmware was restored successfully.

## 9. Optional strongest post-restore verification

For an independent byte-for-byte check, read the complete Flash back again using
the already verified `--no-stub` backup method, then compare its SHA-256 with the
saved factory image.

For example:

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
    .\backup\factory-after-restore-verify.bin
```

Then calculate:

```powershell
$original = (Get-FileHash .\backup\esp32-s3-lcd-1.47-factory.bin -Algorithm SHA256).Hash
$restored = (Get-FileHash .\backup\factory-after-restore-verify.bin -Algorithm SHA256).Hash

$original
$restored
$original -eq $restored
```

The ideal result is:

```text
True
```

A matching full-image SHA-256 demonstrates that the entire 16 MiB Flash content
read back after restore is byte-for-byte identical to the saved image.

The verified no-stub full read is slow on this board/PC combination, so this
optional post-restore check may take tens of minutes.

## 10. If the board does not enter download mode automatically

Use the normal ESP32-S3 manual bootloader sequence:

1. Hold **BOOT**.
2. Press and release **RESET**.
3. Release **BOOT**.
4. Run the esptool command again.

After a successful restore, press RESET once more if necessary to start the
factory firmware.

## Safety rules

- Keep at least one untouched copy of the verified factory `.bin` somewhere
  outside the active repository working directory.
- Never restore from a file whose size or checksum is unknown.
- Do not confuse `read-flash` with `write-flash`.
- Do not add `erase-flash` unless there is a separately justified reason.
- Do not burn or change eFuse as part of firmware restoration.
- Do not assume another physical board has the same factory-image SHA-256.
- Do not publish unique identifiers printed by esptool logs.

## Related documentation

- [Back up the factory firmware](firmware-backup.md)
- [esptool notes](esptool.md)
- [Arduino IDE setup](arduino-ide.md)
- [Project README](../README.md)
