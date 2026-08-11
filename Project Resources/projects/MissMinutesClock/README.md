# Loki Miss Minutes GIF Animation Clock

Status: `HARDWARE_VERIFIED`

Hardware test date: `2026-08-11`

## Source

- Original project: https://github.com/moononournation/animation_clock
- Author: moononournation / 陳亮
- Original YouTube: https://www.youtube.com/watch?v=e-Z9bq2xT2E
- Guide: https://www.instructables.com/GIF-Animation-Clock/
- Dev Device Pins library: https://github.com/moononournation/Dev_Device_Pins
- Display library: https://github.com/moononournation/Arduino_GFX
- LittleFS uploader: https://github.com/earlephilhower/arduino-littlefs-upload
- Date accessed: 2026-08-11
- Upstream branch inspected: `main`
- Exact upstream commit inspected: `c034e2f4eca3ad3c92bc056bc0a4e67fe60bf6b8`
- License: no explicit project license found in the repository at the time of inspection

## Why this project is interesting

This is a compact animated desk clock inspired by Miss Minutes from Loki. It combines animated GIF playback on the 1.47-inch ST7789 display with a large digital clock synchronized from an NTP server over Wi-Fi.

Compared with much heavier ESP32 applications, this is a small and focused example. It demonstrates a useful pattern for:

- animated UI on the Waveshare display;
- storing small assets in internal Flash via LittleFS;
- using Wi-Fi only for time synchronization;
- combining animation and overlaid text without LVGL;
- keeping board-specific display definitions outside the application sketch.

The project is a particularly good fit for our lab because the author provides an explicit board profile for the Waveshare ESP32-S3-LCD-1.47.

## What it uses

- Display: ST7789, 172 x 320, driven by Arduino_GFX
- Wi-Fi: station mode, used to reach an NTP server
- Bluetooth: no
- microSD: no
- USB: normal programming/debug path only
- Buttons/input: not required by the clock application
- Storage: LittleFS in internal Flash for GIF assets
- Time source: `pool.ntp.org`
- Framework: Arduino
- GIF decoder: bundled `GifClass.h`, based on/reworked from BasementCat `arduino-tft-gif`
- Board definitions: `Dev_Device_Pins` profile by the same author

## Application flow

```text
boot
  -> initialize ST7789 display
  -> mount LittleFS
  -> allocate indexed GIF frame buffer
  -> show TVA logo
  -> play greeting animation
  -> connect Wi-Fi while playing waiting animation
  -> synchronize time from NTP
  -> open walking animation
  -> repeatedly:
       draw HH:MM
       play animation
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

The upstream repository contains them under:

```text
miss_minutes_clock/data/
```

The source comments say the GIFs were optimized with ezgif.com.

The animation artwork and related character/trademark rights belong to their respective rights holders. These media assets are not copied into this public lab repository.

## Compatibility with Waveshare ESP32-S3-LCD-1.47

Compatibility is excellent and has now been confirmed on real hardware.

The inspected upstream sketch contains both board-profile includes:

```cpp
#include "PINS_ESP32-C6-LCD-1_47.h"
// #include "PINS_ESP32-S3-LCD-1_47.h"
```

For our board the required change is:

```cpp
// #include "PINS_ESP32-C6-LCD-1_47.h"
#include "PINS_ESP32-S3-LCD-1_47.h"
```

The author's `Dev_Device_Pins` library contains the expected mapping for our board:

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

## Verified installation procedure

The following procedure was actually used successfully on our Waveshare ESP32-S3-LCD-1.47.

### 1. Keep the upstream source local

The original project remains under the ignored research tree:

```text
Project Resources/_local/animation_clock/
```

The working sketch is:

```text
animation_clock/miss_minutes_clock/miss_minutes_clock.ino
```

### 2. Select the S3 board profile

Use:

```cpp
// #include "PINS_ESP32-C6-LCD-1_47.h"
#include "PINS_ESP32-S3-LCD-1_47.h"
```

For our test the matching `PINS_ESP32-S3-LCD-1_47.h` file from `Dev_Device_Pins` was copied directly into the sketch directory so the include resolved without additional Arduino library-path configuration.

### 3. Install Arduino_GFX

Verified in our test:

```text
Arduino_GFX 1.6.7
```

The required header must actually exist at something like:

```text
Arduino_GFX/src/Arduino_GFX_Library.h
```

A useful practical lesson from this test: a pre-existing but incomplete `Arduino_GFX` directory can look like an installed library while still failing with:

```text
fatal error: Arduino_GFX_Library.h: No such file or directory
```

Check the real header file, not just the folder name.

### 4. Enter Wi-Fi credentials locally

Upstream placeholders are:

```cpp
const char *SSID_NAME = "YourAP";
const char *SSID_PASSWORD = "PleaseInputYourPasswordHere";
```

Replace them locally. Do not commit real Wi-Fi credentials to the public repository.

### 5. Set the local time offset

Upstream defaults to UTC+8:

```cpp
const long gmtOffset_sec = 8 * 60 * 60;
```

For our UTC+3 test:

```cpp
const long gmtOffset_sec = 3 * 60 * 60;
```

This was verified on hardware. Before correction the clock was exactly five hours ahead; after changing UTC+8 to UTC+3 the displayed time matched local time.

The implementation uses a fixed offset and does not provide automatic timezone/DST handling.

### 6. Select a partition layout with a filesystem partition

The first run inherited an unsuitable partition layout from the previous media-server experiment and produced:

```text
esp_littlefs: partition "spiffs" could not be found
```

The working Arduino IDE choice was:

```text
Huge APP (3MB No OTA/1MB SPIFFS)
```

Important detail: the sketch still uses `LittleFS`. In Arduino-ESP32, LittleFS uses the partition label `spiffs` by default, so the partition name in the menu can be confusing. The label does not mean that the sketch is using the old SPIFFS filesystem implementation.

### 7. Install the LittleFS uploader

Successfully used:

```text
arduino-littlefs-upload 1.1.8
```

After installing the `.vsix` extension and restarting Arduino IDE, the command becomes available through:

```text
Ctrl + Shift + P
Upload LittleFS to Pico/ESP8266/ESP32
```

### 8. Upload the `data` directory

The uploader automatically builds a LittleFS image from:

```text
miss_minutes_clock/data/
```

and uploads:

```text
MM_hi.gif
MM_wait.gif
MM_walk.gif
TVA_logo.gif
```

The firmware alone is therefore not a complete installation. The successful workflow is:

```text
select correct partition scheme
        +
upload LittleFS data image
        +
compile/upload sketch
```

The LittleFS image only needs to be uploaded again when the assets or the filesystem partition layout change.

## Verified Arduino environment

The successful hardware test used:

```text
Board:              ESP32S3 Dev Module
Arduino-ESP32 core: 3.3.11
Flash Size:         16MB
PSRAM:              OPI PSRAM
CPU Frequency:      240MHz
Partition Scheme:   Huge APP (3MB No OTA/1MB SPIFFS)
Arduino_GFX:        1.6.7
LittleFS uploader:  1.1.8
```

This sketch does not need the TinyUSB/MSC complexity used by some other projects.

## Memory model

The code allocates the GIF output buffer as:

```cpp
malloc(gfx->width() * gfx->height())
```

For the 172 x 320 display:

```text
172 x 320 = 55,040 bytes
```

The buffer is one byte per pixel because the GIF renderer works with palette indexes and Arduino_GFX converts the palette to RGB565 when drawing. This is substantially lighter than a full 16-bit framebuffer for the whole screen.

## Local hardware test

Status: `HARDWARE_VERIFIED`

Verified on our real Waveshare ESP32-S3-LCD-1.47:

```text
PASS  firmware compiles
PASS  firmware uploads
PASS  ST7789 initializes correctly
PASS  display geometry/orientation is usable
PASS  LittleFS mounts with the correct partition layout
PASS  all four GIF assets upload through the LittleFS uploader
PASS  TVA logo displays
PASS  greeting/waiting/walking animations play
PASS  Wi-Fi connection works
PASS  NTP synchronization works
PASS  HH:MM clock display works
PASS  UTC+3 correction verified against local time
```

Observed sequence:

```text
TVA logo
-> greeting animation
-> Wi-Fi/NTP waiting animation
-> digital HH:MM
-> repeating walking animation
```

This is a functional hardware verification, not a long-duration endurance test.

## Impressions after the real test

This project made a very good engineering impression for its scope. It is small enough that failures remain understandable, and the successful path is easy to reproduce once the missing installation details are documented.

The application has only a few important moving parts:

```text
Arduino_GFX
LittleFS
Wi-Fi/NTP
GIF decoder
board profile
```

There is no microSD, USB MSC, async web server, background indexing or multi-client media layer. That makes debugging much cleaner than in a full storage/media appliance.

The most confusing part was actually the Arduino storage tooling rather than the application itself: `LittleFS` looks for a partition labelled `spiffs`, while the Arduino menu also calls the partition `SPIFFS`. That naming mismatch can make it appear that the sketch is using SPIFFS when it is in fact using LittleFS.

A second important point is that the asset uploader is part of the real installation. Uploading only the sketch produces an incomplete device. For this class of application, firmware and LittleFS assets should ideally be released together.

Current assessment:

```text
Board compatibility:        10/10
Ease of understanding:       9/10
Visual result:               9/10
Build reproducibility:       8/10 after documenting dependencies
Operational complexity:      low
Value as learning example:   high
Hardware test result:        successful
```

## Interesting engineering points

### 1. Animation can represent system state

The project does not merely loop one decorative GIF. Different animations correspond to different phases:

```text
startup / greeting
waiting for network/time
normal running animation
```

That pattern is reusable with original artwork for device states, errors, updates, sensor conditions or agent status.

### 2. Indexed animation is efficient

For a 172 x 320 display, direct indexed bitmap rendering is a good alternative to a full UI framework when the task only needs animation and a few text overlays.

### 3. Internal Flash is enough for small immutable media packs

The four GIF files are small enough that adding an SD card would only increase complexity. LittleFS is a better fit for fixed UI assets.

### 4. Board profiles should be reusable

The author's separate S3 board definition confirms a pattern worth keeping: display geometry, offsets and GPIO mapping should live in a board-support layer rather than inside each application.

### 5. Firmware and assets should be versioned together

A polished release for this type of project should treat:

```text
firmware binary
+ LittleFS image
+ partition layout
+ board profile
+ version manifest
```

as one release package.

## Limitations

- Upstream README is very minimal and does not document a full tested version matrix.
- No explicit project license file was found at the time of inspection, so reuse terms are not clearly defined.
- Wi-Fi credentials are stored in source code.
- Timezone support is a fixed offset only; no automatic DST handling.
- Missing LittleFS/assets lead to a hard stop rather than a recoverable diagnostic mode.
- Distribution is effectively two-part: firmware plus LittleFS assets.

## Our video

### Hardware verification / final result

- Video: https://youtube.com/shorts/ZK_HQWrQ_ig
- Title: `Loki Miss Minutes Clock на Waveshare ESP32-S3-LCD-1.47 | GIF-анимация + NTP часы`
- Verification shown: real hardware, GIF animation, Wi-Fi/NTP synchronization and working HH:MM clock.

Useful links for the video description:

```text
Original project:
https://github.com/moononournation/animation_clock

Original guide:
https://www.instructables.com/GIF-Animation-Clock/

Original author video:
https://www.youtube.com/watch?v=e-Z9bq2xT2E

Our ESP32-S3-LCD-1.47 lab:
https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47

Our hardware test video:
https://youtube.com/shorts/ZK_HQWrQ_ig
```

## What we learned

The most reusable ideas are not the character artwork itself but the implementation pattern:

- keep board-specific GPIO/display geometry in a reusable board profile;
- use LittleFS for a small immutable asset pack when SD is unnecessary;
- use indexed/paletted animation to reduce RAM requirements;
- keep the display pipeline direct and small when LVGL is unnecessary;
- synchronize time from NTP instead of adding an RTC when Wi-Fi is acceptable;
- use a loading animation to make network synchronization visibly meaningful;
- document filesystem partition and asset upload as part of firmware installation;
- verify actual library contents, not merely the presence of a library directory.

## Own-project ideas inspired by the general principle

This pattern can become a generic animated appliance display:

```text
LittleFS asset pack
+ state machine
+ animated status character/icon
+ text/telemetry overlay
+ network/device events
```

Possible independent applications:

- animated laboratory clock;
- animated device status mascot;
- Wi-Fi/NTP diagnostic screen;
- sensor dashboard with contextual animations;
- boot/update/recovery animations;
- compact animated notification display.

## Provenance / attribution note

This page documents an independent technical study and hardware verification of the project linked above. The original source code remains attributable to moononournation / 陳亮. Character artwork and associated trademarks remain attributable to their respective rights holders. No third-party source code or GIF media is included in this public lab repository by this research card.
