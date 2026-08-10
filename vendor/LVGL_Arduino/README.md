# Waveshare `LVGL_Arduino` — compatibility-adapted vendor example

This directory contains the Waveshare **LVGL_Arduino** example for the
**Waveshare ESP32-S3-LCD-1.47**, with a small set of compatibility and behavior
changes made during hardware testing.

The example is intentionally kept under `vendor/` so that vendor-derived code
remains separate from the project's independently written diagnostics under
`examples/`.

## Source and provenance

Source: Waveshare `LVGL_Arduino` example from the ESP32-S3-LCD-1.47 demo package.

Official board documentation:

https://www.waveshare.com/wiki/ESP32-S3-LCD-1.47

The top-level vendor sketch identifies the example as:

```text
@author   Yongqin Ou
@version  V1.0
@date     2024-10-30
@license  MIT
@copyright Copyright (c) 2024, Waveshare
```

Some companion `.cpp` and `.h` files in the downloaded example did not carry
individual license headers. This repository therefore preserves the vendor
provenance at directory level and marks local modifications explicitly instead
of presenting the adapted files as original Waveshare releases.

## Why this copy was adapted

The goal was not to redesign the Waveshare panel. The goal was to get the vendor
LVGL example running predictably in the current Arduino environment while
removing misleading or unrelated pseudo-diagnostics.

The following issues were observed during testing:

1. **LVGL 9.5.0 was not compatible with this example without a port.**
   The source uses the LVGL 8 display-driver API (`lv_disp_drv_t`,
   `lv_disp_draw_buf_t`, `lv_disp_drv_register()`, etc.), so the test environment
   was changed to LVGL **8.3.10**.
2. **`lv_conf.h` handling caused build failures.**
   LVGL itself expects `lv_conf.h` next to the `lvgl` library directory in the
   tested Arduino layout. The vendor `LVGL_Driver.h` also included
   `<lv_conf.h>` directly; that explicit include was unnecessary and failed in
   the Arduino include path used here, so it was removed.
3. **`<demos/lv_demos.h>` was included but not used by this example.**
   The include caused another build failure when the demos tree was not present,
   so the unused dependency was removed.
4. **The original SD setup was not the configuration already verified on this
   board.** The vendor code used `SD_MMC.begin("/sdcard", true, true)`, which
   selects 1-bit mode and enables formatting on mount failure in the current
   Arduino-ESP32 API. This adapted copy uses the independently hardware-verified
   4-bit bus and does not auto-format the card:

   ```cpp
   SD_MMC.begin("/sdcard", false)
   ```
5. **The vendor `Flash_test()` output was misleading.** It printed `RAM Test`
   while reading Flash size. The test-style banner/function was removed; Flash
   size is now read once as board information.
6. **The original Wi-Fi/BLE routines were environment scans, not hardware
   diagnostics.** The result depended on how many nearby Wi-Fi/BLE advertisers
   happened to be visible, and the scan was unrelated to LVGL itself. The
   adapted code now performs only a one-time initialization check during
   `setup()`, stores `W: OK/ERR` and `B: OK/ERR`, and immediately switches both
   radios off again. No wireless scan and no background `WirelessScanTask` are
   used.
7. **Console information was changed to `printf()`.** The Waveshare examples on
   the tested board already used `printf()` successfully, while Arduino
   `Serial` output depends on the selected USB/CDC routing options on ESP32-S3.
   Board information is therefore printed once through `printf()` during
   startup.

## What was deliberately kept unchanged

The visible LVGL panel layout is intentionally retained. During testing it
matched the panel seen in the factory firmware on this physical board. That is
an observation only; it is not a claim that the factory binary was built from
these exact source files.

The panel still shows the same three information areas:

```text
SD Card
Flash Size
Wireless scan
```

The wireless value is now an initialization result rather than a count of nearby
radios:

```text
W: OK  B: OK
```

After this result has been recorded, Wi-Fi and BLE are OFF.

The vendor ST7789 initialization, LVGL panel structure, and RGB LED animation are
otherwise kept recognizable rather than rewritten into a new application.

## Hardware-verified environment

```text
Board: Waveshare ESP32-S3-LCD-1.47
MCU: ESP32-S3
Display: ST7789, 172 x 320
Arduino IDE: 2.x
esp32 by Espressif Systems: 3.3.11
LVGL: 8.3.10
microSD cards tested: nominal 16 GB, 32 GB and 64 GB
```

Status of this adapted example:

```text
COMPILES: YES
LVGL 8.3.10: VERIFIED
ST7789 output: VERIFIED
LVGL panel: VERIFIED
microSD 16 GB mount/size display: VERIFIED
microSD 32 GB mount/size display: VERIFIED
microSD 64 GB mount/size display: VERIFIED
Wi-Fi initialization: VERIFIED
BLE initialization: VERIFIED
Wi-Fi/BLE scans: NOT USED
Wi-Fi/BLE after setup: OFF
RGB LED loop: VERIFIED AS PART OF THE RUNNING EXAMPLE
```

## Hardware verification video

The adapted example running on the physical ESP32-S3-LCD-1.47 is shown here:

**[ESP32-S3-LCD-1.47 — Waveshare LVGL 8.3.10 example](https://youtube.com/shorts/OSkb3rakpZM)**

## microSD capacity verification

The adapted firmware was additionally tested on the same physical board with
three different nominal microSD capacities:

```text
16 GB  -> detected and size displayed correctly
32 GB  -> detected and size displayed correctly
64 GB  -> detected and size displayed correctly
```

This verifies the **tested board + adapted 4-bit SD_MMC firmware path** for these
three card capacities. It supersedes the earlier conservative project assumption
that only cards up to 16 GB should be treated as verified for this adapted LVGL
firmware. It does not by itself establish compatibility with every card model,
filesystem, or capacity above 64 GB.

Video demonstration and hardware confirmation:

**[ESP32-S3-LCD-1.47 — microSD 16/32/64 GB capacity test](https://youtube.com/shorts/tdd-AJH6-_c)**

## Hardware-verified console output

The following output was captured from the physical board after the final
adaptation:

```text
========================================
 Waveshare ESP32-S3-LCD-1.47
========================================
Chip model      : ESP32-S3
Chip revision   : 2
CPU cores       : 2
CPU frequency   : 240 MHz
Flash size      : 16 MB
PSRAM size      : 8 MB
Free heap       : 211776 bytes
Free PSRAM      : 8356164 bytes

Display         : ST7789
Resolution      : 172 x 320
LCD MOSI        : GPIO45
LCD SCLK        : GPIO40
LCD CS          : GPIO42
LCD DC          : GPIO41
LCD RESET       : GPIO39
LCD backlight   : GPIO48
RGB LED         : GPIO38

SD CMD          : GPIO15
SD CLK          : GPIO14
SD D0           : GPIO16
SD D1           : GPIO18
SD D2           : GPIO17
SD D3           : GPIO21
microSD         : 14984 MiB

Wi-Fi init      : OK (now OFF)
Bluetooth BLE   : OK (now OFF)

Arduino-ESP32   : 3.3.11
LVGL            : 8.3.10
========================================
```

`Free heap` and `Free PSRAM` are runtime values and can vary between builds and
boots. The output is informational; `W: OK` / `B: OK` means that initialization
completed successfully, not that an RF link, antenna performance, range, or a
complete wireless self-test was verified.

## Required LVGL configuration

This example is for **LVGL 8.3.10**, not LVGL 9.x.

In the tested Arduino library layout, `lv_conf.h` is placed next to the `lvgl`
directory:

```text
Documents/Arduino/libraries/
├── lv_conf.h
└── lvgl/
    ├── lvgl.h
    ├── lv_conf_template.h
    └── src/
```

A practical way to create it is to copy `lv_conf_template.h` from the LVGL
library and rename the copy to `lv_conf.h`, then enable the configuration block
in that file (`#if 1` in the standard LVGL 8 template).

`LVGL_Driver.h` in this adapted example intentionally does **not** include
`<lv_conf.h>` directly and does **not** include `<demos/lv_demos.h>`.

## Local changes by file

### `LVGL_Arduino.ino`

- preserves the original Waveshare attribution;
- initializes SD, wireless status, LCD and LVGL in `setup()`;
- prints board information once with `printf()`;
- removes the legacy `Flash_test()` / `RAM Test` behavior;
- does not launch wireless scans or background wireless tasks.

### `LVGL_Driver.h`

- keeps the LVGL 8 driver declarations;
- removes direct `<lv_conf.h>` inclusion;
- removes unused `<demos/lv_demos.h>` inclusion.

### `Wireless.cpp` / `Wireless.h`

- replace Wi-Fi/BLE scanning with a one-time initialization check;
- store `WIFI_OK`, `BLE_OK`, and `Wireless_checked`;
- turn Wi-Fi off immediately after its initialization check;
- deinitialize/release BLE immediately after its initialization check;
- contain no scan loop and no FreeRTOS wireless task.

### `SD_Card.cpp` / `SD_Card.h`

- initialize SD_MMC in 4-bit mode;
- disable automatic formatting on mount failure;
- expose only the mounted-state and card-size information needed by the panel;
- retain vendor file helper functions for provenance/compatibility.

### `LVGL_Example.cpp` / `LVGL_Example.h`

- preserve the vendor panel layout;
- replace scan counts with `W: OK/ERR  B: OK/ERR`;
- retain the rest of the vendor UI scaffolding rather than turning this into a
  new UI example.

### `Display_ST7789.*` and `RGB_lamp.*`

These remain essentially vendor logic. Local additions are documentation and
provenance comments so the role of each file is clear.

## Project

https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
