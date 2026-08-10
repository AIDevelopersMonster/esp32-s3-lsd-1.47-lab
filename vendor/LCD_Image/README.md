# Waveshare `LCD_Image` — compatibility-adapted vendor example

This directory contains the Waveshare **LCD_Image** Arduino example for the
**Waveshare ESP32-S3-LCD-1.47**, with a small set of compatibility fixes that
were required in the software environment used by this project.

It is intentionally kept under `vendor/` rather than mixed with the project's
own examples. The goal is to preserve the vendor example and its provenance
while documenting exactly what had to be changed to make it work on the tested
board.

## Source and attribution

Original example: Waveshare `LCD_Image` for ESP32-S3-LCD-1.47.

Official board documentation:

https://www.waveshare.com/wiki/ESP32-S3-LCD-1.47

The vendor `LCD_Image.ino` included in the downloaded demo explicitly declares:

```text
@author   Yongqin Ou
@version  V1.0
@date     2024-10-31
@license  MIT
@copyright Copyright (c) 2024, Waveshare
```

The companion `.cpp` and `.h` files in the downloaded example did not contain
individual license headers in the copy inspected for this project. Local headers
have therefore been added to preserve provenance and to point back to the MIT
declaration in the vendor's top-level sketch. See `LICENSE-MIT.txt` in this
directory.

## Why this adapted copy is included

The vendor example did **not run unchanged** in the current project environment.
The failures we observed were real and reproducible on the physical test board,
but they should not automatically be interpreted as proof that the vendor code
is universally broken.

A likely explanation is dependency/API drift between the versions originally
used when the vendor example was written and the newer Arduino/PNGdec versions
used here. The exact dependency versions used by Waveshare for the original
2024 example were not established during this test, so this repository does not
claim a specific vendor-version incompatibility unless it was directly observed.

What *was* directly observed:

1. The original PNG draw callback signature was incompatible with PNGdec 1.1.6:
   the installed library expects an `int` draw callback, while the vendor example
   used a `void` callback.
2. The vendor SD initialization used `SD_MMC.begin("/sdcard", true, true)`.
   In this project the root directory was not readable in that configuration,
   while the separately verified 4-bit configuration worked correctly.
3. After SD access was fixed, ordinary RGB PNG files displayed correctly, but
   the slideshow stopped on the first RGBA PNG (`pixel type: 6`).
4. The RGBA problem disappeared after adapting the RGB565 line-buffer path,
   using an explicit white background for conversion, and correcting the
   inclusive LCD window end coordinate.
5. After these changes the supplied PNG slideshow displayed correctly on the
   physical board, including the previously failing RGBA images.

## Hardware-verified environment

```text
Board: Waveshare ESP32-S3-LCD-1.47
Display: ST7789, 172 x 320
microSD: 16 GB, FAT32
Arduino IDE: 2.x
esp32 by Espressif Systems: 3.3.11
PNGdec: 1.1.6
```

Status for this adapted example:

```text
COMPILES: YES
SD_MMC 4-bit: VERIFIED
PNG file enumeration: VERIFIED
RGB PNG display: VERIFIED
RGBA PNG display: VERIFIED AFTER COMPATIBILITY FIXES
ST7789 output: VERIFIED
RGB LED loop: VERIFIED AS PART OF THE RUNNING EXAMPLE
```

## Local changes relative to the downloaded Waveshare example

### `SD_Card.cpp`

Vendor code used:

```cpp
SD_MMC.begin("/sdcard", true, true)
```

This adapted copy uses:

```cpp
SD_MMC.begin("/sdcard", false)
```

For the current Arduino-ESP32 API, `false` selects the 4-bit SD_MMC bus. This
matches the board wiring already verified independently by this project.

The change also removes the vendor example's `format_if_mount_failed = true`
behavior. The adapted copy does **not** automatically format the card when mount
fails.

### `LCD_Image.cpp`

The following compatibility changes were made:

- PNGdec draw callback changed from `void` to `int` and returns `1`, matching
  PNGdec 1.1.6.
- RGB565 line buffer is rounded up from 172 to 176 entries:
  `(MAX_IMAGE_WIDTH + 7) & ~7`.
- `getLineAsRGB565()` uses explicit white background `0x00FFFFFF` instead of the
  previous `0xFFFFFFFF` value used by the vendor example.
- the LCD window end X coordinate is changed from
  `xpos + pDraw->iWidth` to `xpos + pDraw->iWidth - 1` because
  `LCD_addWindow()` treats both start and end coordinates as inclusive.
- the Y start coordinate consistently includes `ypos`.

These changes removed the repeatable freeze seen when the slideshow reached the
first 172 x 320 RGBA PNG under PNGdec 1.1.6.

## Changes deliberately *not* hidden

Some quirks from the vendor source are retained so this remains recognizable as
an adapted vendor example rather than a rewrite. For example, `Flash_test()`
prints a banner labelled `RAM Test` even though it queries Flash size. This is a
vendor-source naming/output inconsistency and is left visible rather than being
silently rewritten.

`BOOT_KEY_PIN` is also still the value supplied by the vendor example. The
automatic slideshow path was hardware-tested; the manual `Image_Next()` button
path was not part of this verification and should be checked separately before
relying on it.

## SD card contents

The example searches the selected directory (normally `/`) for filenames that
contain `.png`, then cycles through those images.

Use the PNG files supplied with the Waveshare demo or compatible 172 x 320 PNG
files. Both RGB and RGBA PNG images were successfully displayed after the local
compatibility changes described above.

## Important distinction from the project's own examples

Files in this directory are **vendor-derived code with local compatibility
changes**.

The project's independently written and hardware-verified diagnostic examples
remain under:

```text
examples/
```

For example, the project's own SD test intentionally isolates microSD from LCD
and PNG decoding and performs a read-only SD_MMC check.

## License

The vendor top-level source identifies the example as MIT licensed. The MIT
license permits modification and redistribution provided the copyright and
permission notice are retained. The original Waveshare attribution is therefore
preserved, and local changes are explicitly identified rather than presented as
original Waveshare code.

See:

- `LICENSE-MIT.txt`
- the original header retained in `LCD_Image.ino`
- local provenance comments at the top of the companion source files

## Project

https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
