# Arduino IDE setup

## Board package

Install **esp32 by Espressif Systems**, version **3.0.2 or newer**.

The current Arduino IDE test environment uses **Arduino-ESP32 3.3.11**.

## Current hardware-tested Arduino IDE settings

The following configuration was rechecked on the physical **Waveshare ESP32-S3-LCD-1.47** on 2026-08-12. With these USB settings the COM port remained stable and a continuous `Serial.println()` test worked correctly.

| Option | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | **Enabled** |
| CPU Frequency | 240MHz (WiFi) |
| Core Debug Level | None |
| USB DFU On Boot | Disabled |
| Erase All Flash Before Sketch Upload | Disabled |
| Events Run On | Core 1 |
| Flash Mode | QIO 80MHz |
| Flash Size | 16MB (128Mb) |
| JTAG Adapter | Disabled |
| Arduino Runs On | Core 1 |
| USB Firmware MSC On Boot | Disabled |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| PSRAM | OPI PSRAM |
| Upload Mode | **UART0 / Hardware CDC** |
| Upload Speed | 921600 |
| USB Mode | **Hardware CDC and JTAG** |
| Zigbee Mode | Disabled |

The three USB-related settings that should be kept together for the current tested configuration are:

```text
USB CDC On Boot: Enabled
Upload Mode: UART0 / Hardware CDC
USB Mode: Hardware CDC and JTAG
```

Do not confuse this with the earlier experimental `USB-OTG CDC (TinyUSB)` / `USB-OTG (TinyUSB)` configuration. The current baseline for this board is **Hardware CDC and JTAG** with **USB CDC On Boot enabled**.

The selected `16M Flash (3MB APP/9.9MB FATFS)` partition scheme describes the ESP32-S3 internal flash layout. It is independent of the removable microSD card and of the board's `SD_MMC` wiring.

Install these libraries through Library Manager for the current KONTAKTS firmware:

- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- Adafruit NeoPixel

Open an example from the `examples` directory and upload it.

## Current KONTAKTS firmware

The firmware shown on the board as **KONTAKTS / ESP32-S3-LCD-1.47 / FW v0.1.0** is our project firmware, not the original Waveshare factory demo.

Canonical project/release identifier:

```text
KONTAKTS_ESP32_S3_LCD_1_47
```

The underscore form is used for the Arduino sketch folder, sketch file and release BIN names. The board model remains human-readable as `ESP32-S3-LCD-1.47`.

Keep the original factory backup separately. Do not confuse a project release image with the saved factory image.

## Build two BIN files in Arduino IDE

Open:

[`../firmware/KONTAKTS_ESP32_S3_LCD_1_47/KONTAKTS_ESP32_S3_LCD_1_47.ino`](../firmware/KONTAKTS_ESP32_S3_LCD_1_47/KONTAKTS_ESP32_S3_LCD_1_47.ino)

With Arduino-ESP32 3.x, use:

**Sketch -> Export Compiled Binary**

Arduino IDE places compiled files in the sketch directory. Depending on the exact Arduino IDE/core version, generated names can contain `.ino`, but the useful pair is:

- application image only;
- merged image containing bootloader, partition table and application.

For releases, use these canonical names:

```text
KONTAKTS_ESP32_S3_LCD_1_47.bin
KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

If Arduino exports:

```text
KONTAKTS_ESP32_S3_LCD_1_47.ino.bin
KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin
```

rename the release copies to the canonical names above.

For distribution and quick recovery, keep both files. The **merged BIN is the preferred one-file image** when flashing with `esptool` or the supplied batch file.

The application-only `.bin` is useful for normal Arduino-style updates when the bootloader and partition table already match. Its flash offset depends on the selected partition table; for the usual ESP32-S3 Arduino layout it is commonly `0x10000`, but do not hard-code that offset for another partition scheme. If in doubt, use the merged image or copy the exact offsets printed by Arduino IDE in verbose upload output.

## SHA-256 hashes

Always publish/check SHA-256 together with release BIN files.

### Windows PowerShell

```powershell
Get-FileHash .\KONTAKTS_ESP32_S3_LCD_1_47.bin -Algorithm SHA256
Get-FileHash .\KONTAKTS_ESP32_S3_LCD_1_47.merged.bin -Algorithm SHA256
```

### Windows CMD

```bat
certutil -hashfile KONTAKTS_ESP32_S3_LCD_1_47.bin SHA256
certutil -hashfile KONTAKTS_ESP32_S3_LCD_1_47.merged.bin SHA256
```

### Linux/macOS

```bash
sha256sum KONTAKTS_ESP32_S3_LCD_1_47.bin KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

For Windows, the repository also contains:

[`../tools/make-firmware-hashes.bat`](../tools/make-firmware-hashes.bat)

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
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x0 KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

Replace `COM7` with the actual board port.

A merged Arduino image is intentionally written from address `0x0`: the image already contains the components placed at their required offsets.

After writing, press **RESET** if the board does not restart automatically.

## Fast Windows BAT flashing

The repository contains:

[`../tools/flash-merged.bat`](../tools/flash-merged.bat)

From the repository root:

```bat
tools\flash-merged.bat COM7 firmware\KONTAKTS_ESP32_S3_LCD_1_47\KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

The batch file checks that the BIN exists and then invokes:

```text
py -m esptool --chip esp32s3 --port <PORT> --baud 921600 write-flash 0x0 <MERGED_BIN>
```

This is the fastest repeatable method when the firmware is already built: no need to reopen and rebuild the sketch in Arduino IDE.

## Application-only BIN: use with care

If you intentionally want to write only `KONTAKTS_ESP32_S3_LCD_1_47.bin`, first confirm the application offset from the selected partition table or from Arduino IDE verbose upload output.

For a layout where the application starts at `0x10000`, the command is:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x10000 KONTAKTS_ESP32_S3_LCD_1_47.bin
```

Do **not** use this command blindly with a different partition scheme. A wrong offset can produce an unbootable image while leaving other flash regions intact.

## Recommended release workflow

For each KONTAKTS firmware release:

1. Set/check the firmware version in the source.
2. Build successfully in Arduino IDE.
3. Use **Sketch -> Export Compiled Binary**.
4. Keep both the application `.bin` and `.merged.bin`.
5. Rename release copies to `KONTAKTS_ESP32_S3_LCD_1_47.bin` and `KONTAKTS_ESP32_S3_LCD_1_47.merged.bin` if necessary.
6. Generate `SHA256SUMS.txt` with [`../tools/make-firmware-hashes.bat`](../tools/make-firmware-hashes.bat).
7. Test-flash the `.merged.bin` on the real ESP32-S3-LCD-1.47.
8. Record the version, Arduino-ESP32 version, build settings and hashes with the release.

The GitHub Actions workflow [`../.github/workflows/build-kontakts-firmware.yml`](../.github/workflows/build-kontakts-firmware.yml) performs the same naming automatically and publishes both BIN files plus `SHA256SUMS.txt` as the build artifact.

## Entering download mode manually

When automatic upload fails:

1. Hold **BOOT**.
2. Press and release **RESET**.
3. Release **BOOT**.
4. Start uploading again.
5. Press RESET once after a successful upload if the program does not start automatically.
