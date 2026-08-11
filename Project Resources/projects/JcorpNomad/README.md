# Jcorp Nomad on Waveshare ESP32-S3-LCD-1.47

Status: `PARTIAL`

## Source

- Original project: https://github.com/Jstudner/jcorp-nomad
- Author: Jackson Studner / Jcorp Tech
- License declared by upstream README: CC BY-NC-SA 4.0
- Date accessed: 2026-08-11
- Upstream branch inspected: `main`
- Exact upstream commit used in our local test: `6dd953f27f5403d41c7a14eb6ff8cd2ffe7ceca4`
- Companion tools: https://github.com/Jstudner/Nomad-Tools

## What this project actually is

Jcorp Nomad turns the Waveshare ESP32-S3-LCD-1.47 into a small autonomous offline media and knowledge appliance.

The board creates its own Wi-Fi network, serves a browser UI, reads content from microSD and uses the 1.47-inch LCD as a local status/control interface. The project is intended for offline use while travelling, camping, teaching, field work or any situation where a compact self-contained server is useful without internet access.

Upstream currently includes or documents:

- local Wi-Fi access point and captive portal;
- browser UI;
- movies and TV shows;
- music playback;
- image gallery;
- PDF, EPUB and comic reading;
- file browser and file management;
- persistent playback progress;
- admin panel;
- theme customization;
- background media-library indexing;
- HTTP Range streaming and seeking;
- offline ZIM archives such as Wikipedia/Gutenberg-style collections;
- multiple simultaneous clients/streams;
- LVGL-based local LCD UI;
- USB Mass Storage mode for the microSD card;
- optional 3D-printable enclosure;
- companion desktop tools for preparing the SD card and indexes;
- a HandBrake preset for making ESP32/browser-friendly video files.

This is one of the highest-relevance reference projects for our lab because it is written specifically for essentially the same Waveshare ESP32-S3-LCD-1.47 hardware.

## Hardware configuration confirmed in upstream source

The display configuration matches our board:

```text
LCD:       ST7789, 172 x 320
MOSI:      GPIO45
SCLK:      GPIO40
CS:        GPIO42
DC:        GPIO41
RESET:     GPIO39
Backlight: GPIO48
Offset X:  34
Offset Y:  0
SPI:       80 MHz
```

The microSD configuration also matches:

```text
SD CLK: GPIO14
SD CMD: GPIO15
SD D0:  GPIO16
SD D1:  GPIO18
SD D2:  GPIO17
SD D3:  GPIO21
Mode:   SD_MMC 4-bit
```

## Recommended Arduino IDE settings for this board/project

The configuration we are using for the current test is:

```text
Board:                         ESP32S3 Dev Module
USB CDC On Boot:               Enabled
USB DFU On Boot:               Disabled
CPU Frequency:                 240MHz (WiFi)
Flash Mode:                    QIO 80MHz
Flash Size:                    16MB (128Mb)
Partition Scheme:              16M Flash (3MB APP/9.9MB FATFS)
PSRAM:                         OPI PSRAM
USB Firmware MSC On Boot:      Disabled
Upload Mode:                   USB-OTG CDC (TinyUSB)
USB Mode:                      USB-OTG (TinyUSB)
Upload Speed:                  921600
JTAG Adapter:                  Disabled
Zigbee Mode:                   Disabled
```

For the first clean installation it is reasonable to enable:

```text
Erase All Flash Before Sketch Upload: Enabled
```

After a successful first boot, set it back to `Disabled` for normal development so NVS/settings are not erased on every upload.

The TinyUSB setting is important: upstream author guidance in the project issues specifically recommends `USB-OTG (TinyUSB)` for Nomad USB Mass Storage mode instead of `Hardware CDC and JTAG`.

## Library versions used by upstream

For reproduction we keep the declared versions before attempting modernization:

```text
ArduinoJson            7.3.0
AsyncTCP               3.4.7
ESP Async WebServer    3.7.1
LVGL                   8.3.10
SdFat                  2.3.0
```

Do not replace LVGL 8.3.10 with LVGL 9.x during the initial verification. That would turn a hardware verification into a separate porting project.

## What to do after flashing

Nomad is not just firmware. The useful installation is a pair:

```text
ESP32 firmware
+
microSD content/web assets
```

The repository contains an `SD_Card_Template` directory. Its contents are intended to be copied to the microSD card and then supplemented with your own media/content.

Typical structure includes areas for:

```text
Movies
Shows
Music
Books
Gallery
Files
ZIM / archive content
web UI assets
indexes / metadata
```

After the card is prepared:

1. Insert the microSD card into the Waveshare board.
2. Boot Nomad.
3. Connect a phone or PC to the Nomad Wi-Fi access point.
4. Open the captive portal/browser UI.
5. Use the admin/library controls to build or rebuild the media index.
6. Test files, media playback and seeking.
7. Only after the normal media mode is stable, test USB Mass Storage mode.

## Wi-Fi and browser UI

The normal operating model is:

```text
Waveshare ESP32-S3-LCD-1.47
          |
          +-- creates Wi-Fi AP
          |
          +-- browser/captive portal
          |
          +-- serves SD content
```

The small LCD is best understood as the local status/control surface. The browser is the rich UI.

That split is important: the 172 x 320 LCD does not need to implement a full media browser itself.

## USB Mass Storage mode

Nomad can expose the microSD card to the host computer through ESP32-S3 native USB MSC.

The implementation uses `USBMSC` and raw SD_MMC sector reads/writes. It also records whether the host changed the card. If data was written through USB, Nomad stores an NVS flag so the next media-mode boot knows that the library may need to be reindexed.

Conceptually:

```text
MEDIA MODE
   |
   +-- request USB mode
           |
           v
     reboot / USB MSC
           |
           +-- PC sees microSD as a drive
           +-- host may change files
           |
           v
     return to media mode
           |
           +-- if writes occurred -> reindex required
```

This is a particularly good embedded-appliance pattern because it avoids pretending that the ESP32 filesystem view is still valid while a PC has raw block-level ownership of the card.

## Companion project: Nomad-Tools

The author also maintains:

https://github.com/Jstudner/Nomad-Tools

This is important because it moves heavy preparation work away from the ESP32 and onto a desktop computer.

The tools currently provide a menu-driven workflow on Windows and Linux and can:

- add ZIM archives;
- split very large ZIM files when required;
- build/rebuild the ZIM search index;
- optimize movie/show/book cover images;
- rebuild the Nomad media index;
- operate directly on a selected Nomad SD card.

The tools require Python 3; Pillow is used for image optimization and Node.js is needed for the ZIM import workflow.

This is a strong architectural choice: preprocessing that is expensive in RAM/CPU/storage I/O happens on a PC, while the microcontroller gets compact prepared data.

## Offline Wikipedia / ZIM archives

Nomad can serve offline knowledge collections stored in ZIM format, including Wikipedia/Gutenberg-style archives.

The interesting engineering principle is not the ZIM format itself but the workflow:

```text
large source archive
      |
      v
PC preprocessing/index construction
      |
      v
prepared SD-card data
      |
      v
small ESP32 serving/search appliance
```

This pattern is directly reusable in engineering databases, offline documentation, component catalogs and future local-agent knowledge packs.

## HandBrake video preset supplied by the author

The main repository contains:

```text
tools/handbrake/
```

and a preset documented as:

```text
Nomad-480p-Web-DirectPlay-HandBrake-1.11.2.json
```

The upstream preset targets small browser-friendly MP4 files:

```text
Container:       MP4 / web optimized
Video:           H.264 Main Level 3.0
Maximum frame:   720 x 480
Frame rate:      up to 30 fps
Video bitrate:   about 600 kbps average
Encoding:        two-pass
Audio:           AAC stereo, about 80 kbps
Upscaling:       disabled
```

This is a sensible solution for an ESP32-class server: reduce the workload and network/storage bandwidth beforehand instead of expecting the microcontroller to transcode media.

The ESP32 acts as a file/range server, not as a video transcoder.

## 3D-printable enclosure

The project also includes/links an optional Mk4 3D-printable enclosure for the board.

This is more than cosmetic. A complete embedded product normally includes:

```text
firmware
+ storage layout
+ host-side preparation tools
+ mechanical enclosure
+ operating instructions
```

That product-level completeness is one of the strongest aspects of Nomad.

When we document or print the enclosure, preserve the upstream attribution and license information rather than importing the files into our public repository without checking their exact terms.

## Architecture in plain language

The firmware combines several layers:

```text
hardware
  |- ST7789 LCD
  |- SD_MMC
  |- USB
  |- BOOT/input
  |- RGB/backlight

system services
  |- Wi-Fi AP
  |- DNS/captive portal
  |- async HTTP server
  |- Preferences/NVS
  |- indexing/background tasks

application
  |- media library
  |- files
  |- books/images/music/video
  |- ZIM knowledge archives
  |- admin/configuration

user interfaces
  |- local LVGL display
  |- SD-hosted browser frontend
```

The firmware coordinates shared work with FreeRTOS queues/mutexes and uses bounded buffers/stream slots in several performance-sensitive areas.

## Professional engineering assessment

### Overall assessment

For a hobby/open-source ESP32 application, Nomad is unusually complete and technically ambitious.

My current engineering rating as a reference project for our Waveshare board is approximately:

```text
Board relevance:             10/10
Product completeness:         9/10
Interesting ESP32 usage:      9/10
Architecture cleanliness:     6/10
Build reproducibility:        6/10
Value as a learning source:   9/10
Value for direct code reuse:  limited by license and architecture
```

The strongest part is not any single algorithm. It is the successful integration of many ordinary embedded/web techniques into one usable appliance.

### What is standard / expected engineering

Individually, many Nomad techniques are standard professional solutions:

- Wi-Fi AP plus local web server for configuration;
- captive portal for a headless/self-contained appliance;
- Preferences/NVS for persistent settings;
- asynchronous HTTP handling;
- microSD for large assets instead of internal Flash;
- browser frontend for complex UI and a small LCD for local status;
- media indexing instead of rescanning the whole SD card per request;
- HTTP Range requests for browser audio/video seeking;
- mutexes around shared non-thread-safe resources;
- FreeRTOS queues for communication between tasks/UI contexts;
- bounded pools instead of unlimited simultaneous streams;
- preprocessing content on a PC rather than transcoding/optimizing on the MCU;
- explicit version pinning when a project depends on fragile library APIs;
- a companion desktop tool for SD-card preparation;
- a physical enclosure as part of the deliverable.

None of these should be regarded as exotic. They are good patterns worth remembering as the default toolbox for embedded network appliances.

### What is genuinely interesting here

The interesting part is the combination and the boundary decisions.

#### 1. Browser UI + tiny local UI

This is close to the ideal architecture for our board. Complex menus, settings, file operations and dashboards belong in the browser. The onboard LCD should show state, progress, warnings and a small set of immediate actions.

#### 2. SD-hosted frontend

Keeping large HTML/JS/assets on the SD card means the browser UI can evolve without consuming the application partition and without rebuilding every frontend change into the firmware.

For our own systems I would keep this pattern, but add explicit asset-version compatibility checks between firmware and SD content.

#### 3. Exclusive USB ownership of microSD

The most valuable low-level idea is the clean mode boundary between normal filesystem use and USB MSC block-device access.

A PC mounting the microSD as a raw USB disk and ESP32 simultaneously modifying the same FAT filesystem is unsafe. Nomad instead changes mode, remembers whether the host wrote data, and requests reindexing afterward.

That is a pattern worth adopting almost verbatim at the architectural level in our future storage appliances.

#### 4. PC preprocessing + MCU serving

The ZIM and HandBrake workflows express an important systems principle:

```text
Do expensive transformation where compute is cheap.
Do deterministic serving where power/resources are constrained.
```

For our projects this applies not only to media, but also to documentation, semantic indexes, thumbnails, search tables, firmware manifests and AI/agent knowledge packs.

#### 5. Whole-product thinking

Firmware + SD template + conversion presets + companion tools + enclosure is much closer to a product than a sketch.

This is worth remembering when we build our own hardware: software alone is not the finished artifact.

## Technical weaknesses / things I would improve

Nomad is useful, but I would not use its current firmware structure as our architectural template without refactoring.

### 1. Very large monolithic `.ino`

The main `JcorpNomadProject.ino` is very large and owns many unrelated responsibilities.

For long-term maintenance I would split it into modules such as:

```text
BoardSupport
DisplayService
StorageService
UsbStorageService
NetworkService
WebApi
MediaIndex
KnowledgeArchive
SettingsService
SystemState
UiBridge
```

The application entry point should mostly compose services and define state transitions.

### 2. Build should be machine-reproducible

Manual Arduino IDE menu settings and manually selected library versions are fragile.

For our own projects I prefer a checked-in reproducible build definition, for example PlatformIO or an equivalent pinned environment, plus a documented generated flash layout.

Arduino IDE can remain an optional user-facing route, but should not be the only authoritative build description.

### 3. Board configuration should be a board profile

GPIO, display geometry, offsets, SD pins and memory geometry should live in one board-definition module rather than being distributed through application code.

That makes later support for another ESP32-S3 board controlled instead of invasive.

### 4. SD/web assets need a version contract

If firmware and SD frontend evolve independently, a mismatch can cause subtle failures.

Our implementation should include something like:

```text
firmware API version
frontend asset version
SD schema version
index format version
```

and fail clearly when versions are incompatible.

### 5. More explicit health/diagnostic telemetry

For our own lab firmware I would expose a diagnostic endpoint/page containing:

- firmware version/commit;
- boot reason;
- free heap and largest free block;
- PSRAM total/free;
- SD capacity/free/health;
- active streams;
- request counters;
- indexing state;
- last reset/watchdog cause;
- Wi-Fi client count/RSSI where applicable;
- task high-water marks if useful.

This would make debugging dramatically easier than reproducing intermittent browser/heap problems blindly.

### 6. Avoid large temporary stack allocations

The display code contains a variable-size temporary buffer in `LCD_addWindow`. On embedded systems, large frame-dependent stack allocations deserve caution.

For our code I would normally prefer a fixed/chunked DMA-capable buffer or an explicitly managed reusable buffer with known maximum size.

## Standard solutions worth remembering for our own programming

These are the main reusable rules I would carry forward from this project.

### Standard 1 — separate control UI from rich UI

```text
LCD / buttons -> immediate local state and essential actions
Browser       -> configuration, files, complex navigation, diagnostics
```

### Standard 2 — separate firmware from large mutable assets

```text
internal Flash -> firmware + critical configuration
microSD        -> large content + replaceable web assets + indexes
```

### Standard 3 — never let two masters write one filesystem

When exporting an SD card over USB MSC, the embedded filesystem should relinquish ownership. After host writes, invalidate caches/indexes and remount/rebuild cleanly.

### Standard 4 — preprocess instead of transcoding on the MCU

Thumbnails, video conversion, search indexes, compressed datasets and semantic preparation belong on a PC/server whenever possible.

### Standard 5 — use explicit bounded resources

Bound concurrent connections/streams, buffer sizes, queues and background jobs. An MCU should have predictable upper resource limits rather than relying on heap availability until failure.

### Standard 6 — synchronize shared hardware resources

SD, SPI buses, shared indexes and other non-reentrant resources need one ownership model: mutex, queue, dedicated service task or another explicit serialization mechanism.

### Standard 7 — background work must not block UI/network responsiveness

Long indexing or scanning jobs should be incremental/background operations with progress and cancellation/reset semantics rather than one giant blocking loop.

### Standard 8 — persist state deliberately

NVS/Preferences is useful for settings and small state flags. Filesystems are for larger mutable data. Do not mix these responsibilities arbitrarily.

### Standard 9 — version every boundary

Firmware, SD schema, web API, indexes and companion tools should have explicit versions and compatibility rules.

### Standard 10 — a reproducible build is part of the product

Record exact commit, core/toolchain version, library versions, board options, partition layout and generated binaries/checksums.

### Standard 11 — design recovery before failure happens

Always preserve a path to ROM bootloader/reflash, a way to clear settings, and a known-good recovery image. USB/network mode changes should never be allowed to make the device practically unrecoverable.

### Standard 12 — diagnostics are a first-class feature

A production embedded appliance should be able to explain what it thinks is wrong: storage unavailable, asset mismatch, low memory, index stale, USB ownership active, etc.

## Ideas for our projects inspired by Nomad

### A. A common ESP32-S3 appliance framework

Instead of building every application from zero, our Waveshare lab could eventually have a reusable framework:

```text
board support
+ display/status UI
+ Wi-Fi AP/STA manager
+ web control plane
+ SD service
+ USB MSC service
+ settings/NVS
+ diagnostic endpoint
+ task/message bus
+ update/recovery layer
```

Then USB tools, knowledge appliances, sensor loggers, diagnostic instruments and agent peripherals become applications on top of the same base.

### B. Offline engineering knowledge appliance

The ZIM concept suggests a device carrying selected engineering documentation, pinouts, component data sheets, repair notes and our own project manuals.

Heavy indexing would be done on the PC; ESP32 would provide local browser access in the field.

### C. Agent-support data node

For future autonomous/agent systems, the ESP32 does not need to run a large AI model. It can be a deterministic peripheral node that provides:

- local sensors;
- files/reference packs;
- device state;
- logs;
- low-level actions;
- cached/preprocessed knowledge;
- a stable local API.

The stronger agent can consume this over Wi-Fi/USB while the ESP32 remains predictable and independently operable.

### D. Universal SD preparation utility for our lab

Nomad-Tools is a strong example. We should eventually have our own desktop utility that can:

```text
identify board/project
prepare SD structure
resize/convert assets
build indexes
write manifests/checksums
validate versions
backup/restore configuration
prepare firmware package
```

That utility could serve multiple Waveshare/ESP32 projects rather than one application only.

### E. Manifest-driven browser installer

Combining lessons from Nomad and browser flashing tools suggests a future distribution path:

```text
release manifest
   + firmware binaries
   + partition description
   + SD asset package/version
   + browser flasher
   + post-flash diagnostics
```

This is a much stronger end-user experience than asking someone to recreate our Arduino IDE menu manually.

## What we should measure during hardware verification

A meaningful hardware test should record more than "boots".

Useful measurements:

- boot time;
- free heap/PSRAM after startup;
- SD throughput;
- browser response time;
- maximum practical file size;
- MP4 seek/range behavior;
- one vs multiple simultaneous clients;
- heap behavior during indexing;
- simultaneous streaming + indexing stability;
- LCD responsiveness under network/SD load;
- Wi-Fi throughput/range;
- USB MSC mount time and stability;
- behavior after host writes to SD;
- recovery after reset/power loss;
- power consumption and temperature during sustained use.

## Local copy policy

Third-party source stays only under the ignored local research tree:

```text
Project Resources/
├── projects/
│   └── JcorpNomad/
│       └── README.md       <- our public research card
└── _local/
    └── jcorp-nomad/        <- upstream clone, NOT committed
```

The local ignore rule has been verified with `git check-ignore`.

## Licensing caution

The upstream README declares **CC BY-NC-SA 4.0**.

That is materially different from a permissive MIT/BSD-style software license. Because our lab can feed future commercial hardware/software work, we should not directly transplant Nomad source code into a commercial KONTAKTS product without separate license review or author permission.

The safe current use is:

- study the implementation;
- test it on our own hardware;
- document architectural principles;
- independently implement the general ideas we need.

## Our video

- Video: TODO after hardware verification

Future video description should clearly credit the original project:

```text
Original project:
https://github.com/Jstudner/jcorp-nomad

Companion tools:
https://github.com/Jstudner/Nomad-Tools

Our ESP32-S3-LCD-1.47 lab:
https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
```

## Current conclusion

Nomad is valuable not because it contains a mysterious breakthrough algorithm, but because it shows how to turn a very small ESP32-S3 board into a coherent appliance.

The key lesson is the system composition:

```text
native USB
+ microSD
+ local LCD
+ browser UI
+ Wi-Fi captive portal
+ HTTP Range streaming
+ background indexing
+ PC-side preprocessing
+ offline knowledge archives
+ physical enclosure
```

For our own work, the professional direction is to keep the good system boundaries while improving modularity, reproducible builds, version contracts, diagnostics and recovery.

## Provenance / attribution

This page documents an independent technical study of Jcorp Nomad. The original project, source code, media, product name, companion tools and related assets remain attributable to Jackson Studner / Jcorp Tech and are governed by their upstream licensing terms. No third-party source code is included in this repository by this research card.
