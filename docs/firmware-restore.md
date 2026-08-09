# Restore the saved factory firmware

This document describes how to restore a **complete raw 16 MiB Flash backup**
previously created from the Waveshare **ESP32-S3-LCD-1.47**.

The matching backup procedure is here:

- [firmware-backup.md](firmware-backup.md)

> [!WARNING]
> Restoring firmware is a **write operation**. It overwrites the ESP32-S3 SPI
> Flash. Do not run the restore command until the backup file has been checked
> for the expected size and SHA-256.

## Important status

The backup path is already hardware-verified on the physical project board.
The restore procedure below follows Espressif's documented `write-flash`
semantics for writing a binary at a specified Flash offset, but **this exact
full 16 MiB restore has not yet been hardware-verified in this project**.

Until a real restore-and-boot test is completed, treat this page as the prepared
recovery procedure rather than a hardware-verified recovery result.

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

The project used esptool v5.3.1 during the backup work.

Check the installed version:

```powershell
py -m esptool version
```

You can inspect the exact options supported by the installed version with:

```powershell
py -m esptool write-flash -h
```

## 7. Restore the complete raw Flash image

Espressif's `write-flash` command takes pairs of Flash offsets and binary files.
Because this backup begins at Flash offset zero and contains the complete raw
Flash image, the prepared restore command is:

```powershell
py -m esptool `
    --chip esp32s3 `
    --port COM16 `
    write-flash `
    0x0 .\backup\esp32-s3-lcd-1.47-factory.bin
```

This is the **destructive step**. esptool erases the Flash sectors covered by the
write and programs the supplied image into them.

Do not run `erase-flash` separately for this normal restore. `write-flash`
already erases the sectors it needs before programming them.

### Why no explicit `--flash-mode` or `--flash-size` override is used

For an exact saved image, the restore command intentionally avoids forcing new
Flash mode/size header values. Espressif documents that these options are not
normally required and that forcing them can update the boot image header.

The goal here is to write back the captured raw image rather than reinterpret it
with new build-time settings.

## 8. What successful flashing should look like

During a normal write, esptool prints progress messages and then verifies the
written data.

Do not disconnect the board while Flash is being erased or programmed.

At the end, reset or reconnect the board if it does not automatically restart.
The factory firmware should then boot from the restored image.

Because this full restore has not yet been hardware-verified in this project,
record the complete esptool output when the first real restore is performed.

## 9. Strong post-restore verification

The strongest project-specific verification is to read the complete Flash back
again using the already verified `--no-stub` backup method, then compare the
SHA-256 with the saved factory image.

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
post-restore check may take tens of minutes.

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
- The first real full restore should be treated as a validation experiment and
  its output should be recorded before this procedure is marked hardware-verified.

## Related documentation

- [Back up the factory firmware](firmware-backup.md)
- [esptool notes](esptool.md)
- [Arduino IDE setup](arduino-ide.md)
- [Project README](../README.md)
