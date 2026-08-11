# Loki Miss Minutes GIF Animation Clock

Status: `NOT_TESTED`

## Source

- Original project: https://github.com/moononournation/animation_clock
- Author: moononournation / 陳亮
- YouTube: https://www.youtube.com/watch?v=e-Z9bq2xT2E
- Guide: https://www.instructables.com/GIF-Animation-Clock/
- Dev Device Pins library: https://github.com/moononournation/Dev_Device_Pins
- Display library: https://github.com/moononournation/Arduino_GFX
- Date accessed: 2026-08-11
- Upstream branch inspected: `main`
- Exact upstream commit inspected: `c034e2f4eca3ad3c92bc056bc0a4e67fe60bf6b8`
- License: no explicit project license found in the repository at the time of inspection

## Why this project is interesting

This is a compact animated desk clock inspired by Miss Minutes from Loki. It combines animated GIF playback on the 1.47-inch ST7789 display with a large digital clock synchronized from an NTP server over Wi-Fi.

Compared with the much heavier media-server projects, this is a small and focused embedded application. It is useful for our lab because it demonstrates a clean pattern for:

- animated UI on the Waveshare display;
- storing small assets in internal Flash via LittleFS;
- using Wi-Fi only for time synchronization;
- combining animation and overlaid text without LVGL;
- keeping board-specific display definitions outside the application sketch.

The project is also explicitly listed by Waveshare among third-party project resources for the ESP32-S3-LCD-1.47.

## What it uses

- Display: ST7789, 172 x 320, driven by Arduino_GFX
- Wi-Fi: station mode, used to reach an NTP server
- Bluetooth: no
- microSD: no
- USB: only normal programming/debug path; no custom USB application
- Buttons/input: not required by the clock application
- Storage: LittleFS in internal Flash for GIF assets
- Time source: `pool.ntp.org`
- Framework: Arduino
- GIF decoder: bundled `GifClass.h`, based on/reworked from BasementCat `arduino-tft-gif`
- Board definitions: external `Dev_Device_Pins` library by the same author

## Upstream application flow

The sketch is approximately this simple:

```text
boot
  -> initialize ST7789 display
  -> mount LittleFS
  -> allocate one indexed GIF frame buffer
  -> show TVA logo
  -> play Miss Minutes greeting animation
  -> connect Wi-Fi while playing waiting animation
  -> synchronize time from NTP
  -> open walking animation
  -> repeatedly:
       draw HH:MM
       play one complete Miss Minutes animation
       rewind GIF
```

The display is not driven by LVGL. Rendering is direct through Arduino_GFX using `drawIndexedBitmap()`.

## GIF assets

The sketch expects these files in LittleFS:

```text
/TVA_logo.gif
/MM_hi.gif
/MM_wait.gif
/MM_walk.gif
```

The upstream repository contains the corresponding files under:

```text
miss_minutes_clock/data/
```

The source comments say the GIFs were optimized with ezgif.com.

The repository README attributes the Miss Minutes GIF sources to Giphy/Marvel/Disney-related source pages. These media assets and character/trademark rights are not ours and should not be copied into this public lab repository.

## Compatibility with Waveshare ESP32-S3-LCD-1.47

Compatibility is unusually good.

The main sketch contains both board-profile includes:

```cpp
#include "PINS_ESP32-C6-LCD-1_47.h"
// #include "PINS_ESP32-S3-LCD-1_47.h"
```

At the current upstream commit the C6 board is selected by default, but the immediately previous code state selected the S3 profile. Therefore S3 support is clearly intentional rather than an inferred port.

For our board the required change is simply:

```cpp
// #include "PINS_ESP32-C6-LCD-1_47.h"
#include "PINS_ESP32-S3-LCD-1_47.h"
```

The author's current `Dev_Device_Pins` library includes an explicit `PINS_ESP32-S3-LCD-1_47.h` profile with the same hardware mapping we have already confirmed independently:

```text
LCD controller: ST7789
LCD size:       172 x 320
LCD DC:         GPIO41
LCD CS:         GPIO42
LCD SCK:        GPIO40
LCD MOSI:       GPIO45
LCD RST:        GPIO39
LCD backlight:  GPIO48
X offset:       34
Y offset:       0

BOOT button:    GPIO0
NeoPixel:       GPIO38

SD CLK:         GPIO14
SD CMD:         GPIO15
SD D0:          GPIO16
SD D1:          GPIO18
SD D2:          GPIO17
SD D3:          GPIO21
```

The clock itself does not use the SD interface.

## Required local changes before build

### 1. Select our S3 board profile

Change:

```cpp
#include "PINS_ESP32-C6-LCD-1_47.h"
// #include "PINS_ESP32-S3-LCD-1_47.h"
```

to:

```cpp
// #include "PINS_ESP32-C6-LCD-1_47.h"
#include "PINS_ESP32-S3-LCD-1_47.h"
```

### 2. Enter Wi-Fi credentials

Upstream placeholders are:

```cpp
const char *SSID_NAME = "YourAP";
const char *SSID_PASSWORD = "PleaseInputYourPasswordHere";
```

Replace them locally with the test network credentials. Do not commit real Wi-Fi credentials to the public repository.

### 3. Set local UTC offset

Upstream defaults to UTC+8:

```cpp
const long gmtOffset_sec = 8 * 60 * 60;
```

For UTC+3 use:

```cpp
const long gmtOffset_sec = 3 * 60 * 60;
```

This implementation uses a fixed offset and has no timezone/DST database. That is acceptable for a fixed-offset clock but less portable than a proper TZ configuration.

### 4. Install the required Arduino libraries

The application needs:

```text
GFX Library for Arduino / Arduino_GFX
Dev Device Pins
```

`LittleFS`, `WiFi` and `WiFiMulti` come from the ESP32 Arduino core.

The GIF decoder is included with the project as `GifClass.h`.

### 5. Upload the LittleFS data folder

The firmware alone is not sufficient. The four GIF files must be written into LittleFS.

The sketch itself points to the Arduino LittleFS upload extension:

https://github.com/earlephilhower/arduino-littlefs-upload

The expected source folder is:

```text
miss_minutes_clock/data/
```

The normal workflow is therefore:

```text
upload LittleFS data image
        +
compile/upload sketch
```

## Arduino IDE starting point for our board

For the first test we should keep the environment simple and close to our already working S3 setup:

```text
Board:       ESP32S3 Dev Module
Flash Size:  16MB
PSRAM:       OPI PSRAM
CPU:         240MHz
```

This sketch does not need the Nomad-specific TinyUSB/MSC configuration. There is no reason to introduce USB Mass Storage complexity into this test.

We should record the exact Arduino-ESP32 core and Arduino_GFX / Dev Device Pins versions that actually compile successfully rather than assuming the newest versions are automatically correct.

## Memory model

The code allocates the GIF output buffer as:

```cpp
malloc(gfx->width() * gfx->height())
```

For the 172 x 320 display this is approximately:

```text
172 x 320 = 55,040 bytes
```

The buffer is one byte per pixel because the GIF renderer works with palette indexes and Arduino_GFX converts the palette to RGB565 when drawing. This is substantially lighter than a full 16-bit framebuffer for the whole screen.

## Technical observations

### Good points

The project is much smaller and easier to reason about than a full media server. There are few moving parts:

```text
Arduino_GFX
LittleFS
Wi-Fi/NTP
GIF decoder
board profile
```

The external `Dev_Device_Pins` library is a useful architectural idea: the application does not need to contain a second copy of all Waveshare GPIO and ST7789 geometry constants.

The animation logic also preserves the GIF frame delay rather than blasting frames as quickly as possible.

### Limitations

The current repository README is extremely minimal and does not document exact library/core versions or a tested release matrix.

There is no explicit project license file visible in the repository, so code reuse terms are not clearly defined. For our purposes the safe approach is study/test/reference unless the author adds a clear license or grants permission.

The clock uses plaintext Wi-Fi credentials in source code. For a demo this is normal, but a reusable product should move credentials into a setup/configuration path or local uncommitted configuration file.

Time zone handling is only a fixed second offset. There is no automatic regional DST handling.

The code stops permanently with `delay(INT_MAX)` if LittleFS or a required GIF cannot be opened. That is acceptable for a small demo but a product-quality application should display a recoverable diagnostic state.

## Comparison with Jcorp Nomad

This is a very different class of project.

```text
Jcorp Nomad:
Wi-Fi server + SD filesystem + indexing + streaming + browser + USB MSC

Miss Minutes Clock:
LittleFS assets + direct LCD rendering + Wi-Fi NTP
```

Therefore the risk surface is much smaller. There is no SD filesystem mutation, async web server, USB MSC or multi-client media streaming. It should be a much cleaner next hardware test for the Waveshare board.

## Local test

Status: `NOT_TESTED`

Planned first test:

```text
1. obtain upstream source locally under Project Resources/_local/
2. select PINS_ESP32-S3-LCD-1_47.h
3. configure temporary Wi-Fi credentials
4. set UTC offset
5. install/verify Arduino_GFX and Dev Device Pins
6. upload LittleFS data
7. compile/upload firmware
8. verify display orientation and animation
9. verify Wi-Fi connection and NTP time
10. record exact versions and result
```

Success criteria:

```text
TVA logo visible
-> greeting animation plays
-> waiting animation while Wi-Fi/NTP connects
-> correct HH:MM
-> walking Miss Minutes animation repeats smoothly
```

## Our video

- Video: TODO after hardware verification

The video description must include:

```text
Original project:
https://github.com/moononournation/animation_clock

Original guide:
https://www.instructables.com/GIF-Animation-Clock/

Original video:
https://www.youtube.com/watch?v=e-Z9bq2xT2E

Our ESP32-S3-LCD-1.47 lab:
https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
```

## What we can learn from it

The most reusable engineering ideas are not the Miss Minutes artwork itself but the implementation pattern:

- keep board-specific GPIO/display geometry in a reusable board profile;
- use LittleFS for a small immutable asset pack when SD is unnecessary;
- use indexed/paletted animation to reduce RAM requirements;
- keep the display pipeline direct and small when LVGL is not needed;
- synchronize time from NTP instead of adding an RTC when internet/Wi-Fi is acceptable;
- use a loading animation to make network synchronization visually meaningful instead of freezing the screen.

## Own-project ideas inspired by the general principle

This pattern can become a generic `animated appliance display` for our own projects:

```text
LittleFS asset pack
+ state machine
+ animated status character/icon
+ text/telemetry overlay
+ network/device events
```

Examples independent of the Loki artwork:

- animated laboratory clock;
- animated device status mascot;
- Wi-Fi/NTP diagnostic screen;
- sensor dashboard with contextual animations;
- boot/update/recovery animations;
- compact animated notification display.

## Provenance / attribution note

This page documents an independent technical study of the project linked above. The original source code remains attributable to moononournation / 陳亮. Miss Minutes, Loki, TVA and associated artwork/trademarks belong to their respective rights holders. The upstream repository references Giphy sources for the animation assets. No third-party source code or GIF media is included in this public lab repository by this research card.
