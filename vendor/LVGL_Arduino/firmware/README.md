# Prebuilt adapted Waveshare LVGL firmware

This directory contains the prebuilt binary image for the hardware-tested,
locally adapted Waveshare `LVGL_Arduino` example for the
**Waveshare ESP32-S3-LCD-1.47**.

## Files

- [`ESP32-S3-LCD-1.47-Waveshare-LVGL-adapted.bin`](ESP32-S3-LCD-1.47-Waveshare-LVGL-adapted.bin)
  — 16 MiB merged firmware image.
- [`ESP32-S3-LCD-1.47-Waveshare-LVGL-adapted.bin.sha256`](ESP32-S3-LCD-1.47-Waveshare-LVGL-adapted.bin.sha256)
  — SHA-256 checksum file for the binary.

Expected SHA-256:

```text
f7e50d1ed8f5f5da802d16761a6b08bf02d2735ff01810dfcc54ca4fc04ed368
```

Expected binary size:

```text
16777216 bytes
```

## What this image is

This is a compiled image of the **adapted Waveshare LVGL demo** kept under
`vendor/LVGL_Arduino/`. It is based on the Waveshare example but includes the
local compatibility and behavior changes documented in the parent README:

- Arduino-ESP32 3.3.11;
- LVGL 8.3.10;
- SD_MMC changed to the tested 4-bit configuration without auto-format;
- Wi-Fi/BLE scans removed and replaced by one-time initialization status;
- Wi-Fi and BLE are switched off after the initialization check;
- misleading vendor `RAM Test` / `Flash_test()` presentation removed;
- board information printed through `printf()`;
- original LVGL panel structure retained.

The adapted source has been exercised on the physical board. The same firmware
path has also been tested with nominal 16 GB, 32 GB and 64 GB microSD cards, all
of which were detected and had their capacities displayed correctly.

This file is **not**:

- the original Waveshare factory firmware;
- the raw 16 MiB factory backup captured from the physical board;
- the independent `KONTAKTS_ESP32-S3-LCD-1.47` firmware under `firmware/`.

It is intentionally stored under `vendor/LVGL_Arduino/firmware/` so the compiled
vendor-derived adaptation remains clearly separated from independently written
project firmware.

## Hardware evidence

Adapted LVGL example:

https://youtube.com/shorts/OSkb3rakpZM

microSD 16/32/64 GB verification:

https://youtube.com/shorts/tdd-AJH6-_c

## Verify the downloaded binary

PowerShell:

```powershell
Get-FileHash `
  .\ESP32-S3-LCD-1.47-Waveshare-LVGL-adapted.bin `
  -Algorithm SHA256
```

The returned hash must be:

```text
F7E50D1ED8F5F5DA802D16761A6B08BF02D2735FF01810DFCC54CA4FC04ED368
```

The repository also provides the lowercase checksum in the standard
`.sha256` companion file.

## Flashing

This is a merged image intended to be written from Flash offset `0x0`.

Example with esptool 5.x:

```powershell
py -m esptool `
    --chip esp32s3 `
    --port COM16 `
    write-flash `
    0x0 .\ESP32-S3-LCD-1.47-Waveshare-LVGL-adapted.bin
```

Change `COM16` to the actual port of the board.

A separate `erase-flash` command is not required; `write-flash` erases the
sectors that it replaces.

## Verification status

The adapted source and its hardware behavior are verified on the physical
ESP32-S3-LCD-1.47, including ST7789/LVGL output, SD_MMC operation, Wi-Fi/BLE
initialization status and the tested 16/32/64 GB microSD capacities.

The repository currently records this `.bin` as the exported prebuilt image of
that adapted build. A separate full cycle of **flash this exact published `.bin`
from `0x0` -> boot -> verify all functions** should be recorded before describing
the binary file itself as independently re-flash-verified.

See the parent documentation:

[`../README.md`](../README.md)
