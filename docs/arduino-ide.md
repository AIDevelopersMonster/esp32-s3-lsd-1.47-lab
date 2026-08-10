# Arduino IDE setup

## Board package

Install **esp32 by Espressif Systems**, version **3.0.2 or newer**.

Use these settings:

| Option | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| USB Mode | Hardware CDC and JTAG |
| USB CDC On Boot | Enabled |
| Flash Size | 16 MB |
| PSRAM | OPI PSRAM |
| Flash Mode | QIO 80 MHz |
| Partition Scheme | 16 MB default / suitable 16 MB scheme |

Install these libraries through Library Manager:

- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- Adafruit NeoPixel

Open an example from the `examples` directory and upload it.

## Current KONTAKTS firmware

The firmware shown on the board as **KONTAKTS / ESP32-S3-LCD-1.47 / FW v0.1.0** is our project firmware, not the original Waveshare factory demo.

Keep the original factory backup separately. Do not confuse a project release image with the saved factory image.

## Build two BIN files in Arduino IDE

With Arduino-ESP32 3.x, use:

**Sketch -> Export Compiled Binary**

Arduino IDE places compiled files in the sketch directory. Depending on the exact Arduino IDE/core version, names can differ slightly, but the useful pair is normally:

- `SketchName.bin` - application image only;
- `SketchName.merged.bin` - merged image containing the bootloader, partition table and application at their correct flash offsets.

For distribution and quick recovery, keep both files. The **merged BIN is the preferred one-file image** when flashing with `esptool` or the supplied batch file.

The application-only `.bin` is useful for normal Arduino-style updates when the bootloader and partition table already match. Its flash offset depends on the selected partition table; for the usual ESP32-S3 Arduino layout it is commonly `0x10000`, but do not hard-code that offset for another partition scheme. If in doubt, use the merged image or copy the exact offsets printed by Arduino IDE in verbose upload output.

## SHA-256 hashes

Always publish/check SHA-256 together with release BIN files.

### Windows PowerShell

```powershell
Get-FileHash .\KONTAKTS.bin -Algorithm SHA256
Get-FileHash .\KONTAKTS.merged.bin -Algorithm SHA256
```

### Windows CMD

```bat
certutil -hashfile KONTAKTS.bin SHA256
certutil -hashfile KONTAKTS.merged.bin SHA256
```

### Linux/macOS

```bash
sha256sum KONTAKTS.bin KONTAKTS.merged.bin
```

For Windows, the repository also contains:

```text
tools\make-firmware-hashes.bat
```

Run it in a directory containing the release `.bin` files. It creates `SHA256SUMS.txt`.

## Upload directly from Arduino IDE

For normal development this is the simplest path:

1. Select **ESP32S3 Dev Module** and the settings from the table above.
2. Select the board COM port under **Tools -> Port**.
3. Open the sketch.
4. Click **Upload**.
5. If upload does not start automatically, enter download mode as described below.

Arduino IDE itself invokes `esptool` and writes the bootloader/partitions/application using the offsets generated for the selected build.

## Flash a merged BIN manually with esptool

Install/update esptool if necessary:

```powershell
py -m pip install -U esptool
```

Then flash a merged image:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x0 KONTAKTS.merged.bin
```

Replace `COM7` and the file name with your actual values.

A merged Arduino image is intentionally written from address `0x0`: the image already contains the components placed at their required offsets.

After writing, press **RESET** if the board does not restart automatically.

## Fast Windows BAT flashing

The repository contains:

```text
tools\flash-merged.bat
```

Usage:

```bat
tools\flash-merged.bat COM7 path\to\KONTAKTS.merged.bin
```

The batch file checks that the BIN exists and then invokes:

```text
py -m esptool --chip esp32s3 --port <PORT> --baud 921600 write-flash 0x0 <MERGED_BIN>
```

This is the fastest repeatable method when the firmware is already built: no need to reopen and rebuild the sketch in Arduino IDE.

## Application-only BIN: use with care

If you intentionally want to write only `KONTAKTS.bin`, first confirm the application offset from the selected partition table or from Arduino IDE verbose upload output.

For a layout where the application starts at `0x10000`, the command is:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x10000 KONTAKTS.bin
```

Do **not** use this command blindly with a different partition scheme. A wrong offset can produce an unbootable image while leaving other flash regions intact.

## Recommended release workflow

For each KONTAKTS firmware release:

1. Set/check the firmware version in the source.
2. Build successfully in Arduino IDE.
3. Use **Sketch -> Export Compiled Binary**.
4. Keep both the application `.bin` and `.merged.bin`.
5. Generate `SHA256SUMS.txt` with `tools\make-firmware-hashes.bat`.
6. Test-flash the `.merged.bin` on the real ESP32-S3-LCD-1.47.
7. Record the version, Arduino-ESP32 version, build settings and hashes with the release.

## Entering download mode manually

When automatic upload fails:

1. Hold **BOOT**.
2. Press and release **RESET**.
3. Release **BOOT**.
4. Start uploading again.
5. Press RESET once after a successful upload if the program does not start automatically.
