# KONTAKTS_ESP32_S3_LCD_1_47 firmware

Independent project firmware for the **Waveshare ESP32-S3-LCD-1.47**.

This is our project firmware, not the Waveshare factory firmware and not a renamed copy of the vendor demo. It is an independently written Arduino-ESP32 application built on hardware behavior verified in this repository.

Current version:

```text
0.1.0
```

Canonical firmware/project identifier:

```text
KONTAKTS_ESP32_S3_LCD_1_47
```

Keep this underscore form for sketch/build/release file names. The human-readable board model remains `ESP32-S3-LCD-1.47`.

## Release file names

For each Arduino export keep both useful BIN files and rename them to the canonical project name:

```text
KONTAKTS_ESP32_S3_LCD_1_47.bin
KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
SHA256SUMS.txt
```

- `KONTAKTS_ESP32_S3_LCD_1_47.bin` — application image only;
- `KONTAKTS_ESP32_S3_LCD_1_47.merged.bin` — preferred one-file image containing bootloader, partition table and application at their normal ESP32-S3 offsets;
- `SHA256SUMS.txt` — SHA-256 hashes of the release BIN files.

Arduino/Arduino CLI can initially generate names containing `.ino`, for example:

```text
KONTAKTS_ESP32_S3_LCD_1_47.ino.bin
KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin
```

Those are build-output names. For release/distribution the repository uses the cleaner canonical names shown above.

## v0.1 goals

The first version is intentionally small and deterministic:

- show a KONTAKTS board-information screen on the built-in ST7789 LCD;
- report the ESP32-S3, Flash, PSRAM, display and microSD state;
- mount the built-in microSD slot in the hardware-verified 4-bit SD_MMC mode;
- initialize Wi-Fi once, record success, then turn Wi-Fi off;
- initialize BLE once, record success, then deinitialize BLE;
- print a one-shot boot report with `printf()`;
- keep Wi-Fi/BLE scans and background diagnostic tasks out of the firmware;
- leave the running firmware static after `setup()` except for the retained LCD image and a small RGB status indication.

## Hardware target

```text
Board       Waveshare ESP32-S3-LCD-1.47 (USB-A variant)
MCU         ESP32-S3
Flash       16 MB
PSRAM       8 MB OPI
Display     ST7789 IPS, 172 x 320
RGB LED     GPIO38
```

Verified LCD wiring used by the firmware:

```text
MOSI        GPIO45
SCLK        GPIO40
CS          GPIO42
DC          GPIO41
RESET       GPIO39
Backlight   GPIO48
```

Verified microSD wiring:

```text
CMD         GPIO15
CLK         GPIO14
D0          GPIO16
D1          GPIO18
D2          GPIO17
D3          GPIO21
```

## Build environment

The project build is pinned to:

```text
esp32 by Espressif Systems  3.3.11
Board                       ESP32S3 Dev Module
CPU                         240 MHz
Flash mode                  QIO
Flash size                  16 MB
PSRAM                       OPI PSRAM
USB                         Hardware CDC/JTAG
```

Required Arduino libraries:

```text
Adafruit GFX Library
Adafruit ST7735 and ST7789 Library
```

The source does not require LVGL. LVGL 8.3.10 remains documented under the adapted vendor example, but this KONTAKTS firmware deliberately starts from the independently verified minimal ST7789 path instead.

## Reproducible GitHub build

The repository contains:

[`../../.github/workflows/build-kontakts-firmware.yml`](../../.github/workflows/build-kontakts-firmware.yml)

The workflow installs Arduino CLI, pins Arduino-ESP32 3.3.11, installs the required display libraries, compiles the firmware and exports both release images:

```text
KONTAKTS_ESP32_S3_LCD_1_47.bin
KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
SHA256SUMS.txt
```

## Build two BIN files in Arduino IDE

Open:

[`KONTAKTS_ESP32_S3_LCD_1_47.ino`](KONTAKTS_ESP32_S3_LCD_1_47.ino)

Then use:

```text
Sketch -> Export Compiled Binary
```

Arduino IDE exports an application BIN and a merged BIN. If the raw generated names contain `.ino`, rename the release copies to the canonical names above.

Full Arduino IDE settings and commands:

[`../../docs/arduino-ide.md`](../../docs/arduino-ide.md)

## SHA-256

From the directory containing both release BIN files, run:

```bat
..\..\tools\make-firmware-hashes.bat
```

This creates:

```text
SHA256SUMS.txt
```

## Flashing the merged image

The merged image is intended to be written from offset `0x0`.

From the repository root on Windows:

```bat
tools\flash-merged.bat COM7 firmware\KONTAKTS_ESP32_S3_LCD_1_47\KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

Or directly with esptool 5.x:

```powershell
py -m esptool `
    --chip esp32s3 `
    --port COM7 `
    --baud 921600 `
    write-flash `
    0x0 .\firmware\KONTAKTS_ESP32_S3_LCD_1_47\KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

Change `COM7` to the actual port of your board.

A separate `erase-flash` step is not required for this operation; `write-flash` erases the sectors it needs to replace.

## Factory recovery

Before flashing this firmware, keep the already verified factory backup in a safe location. The project documentation is here:

- [Firmware backup](../../docs/firmware-backup.md)
- [Firmware restore](../../docs/firmware-restore.md)

The factory backup and the KONTAKTS merged project firmware are different objects:

- the factory backup is a full raw 16 MiB read of a tested physical board state;
- the KONTAKTS merged image is a reproducible firmware generated from source.

## Verification status

The individual hardware paths used by the firmware have been verified on the physical board:

```text
ST7789 display path     VERIFIED
16 MB Flash             VERIFIED
8 MB PSRAM              VERIFIED
SD_MMC 4-bit            VERIFIED
Wi-Fi initialization    VERIFIED
BLE initialization      VERIFIED
```

The combined **KONTAKTS_ESP32_S3_LCD_1_47 v0.1.0** firmware has also been flashed and observed on the real board. The current v0.1.0 line is therefore **HARDWARE-VERIFIED**.

## Source

Main sketch:

[`KONTAKTS_ESP32_S3_LCD_1_47.ino`](KONTAKTS_ESP32_S3_LCD_1_47.ino)

Project repository:

https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
