# Jcorp Nomad on Waveshare ESP32-S3-LCD-1.47

Status: `REFERENCE_ONLY`

## Source

- Original project: https://github.com/Jstudner/jcorp-nomad
- Author: Jackson Studner / Jcorp Tech
- License declared by upstream README: CC BY-NC-SA 4.0
- Date accessed: 2026-08-11
- Upstream branch inspected: `main`
- Upstream commit inspected: `6dd953f27f5403d41c7a14eb6ff8cd2ffe7ceca4`

## What this project actually is

Jcorp Nomad is a portable offline media and knowledge server built specifically around the Waveshare ESP32-S3 board with the 1.47-inch LCD.

The device creates its own Wi-Fi hotspot and serves a browser interface from the ESP32-S3. Media and web assets are stored on microSD. The project is intended for offline use while travelling, camping, teaching, remote work or any situation where a compact local server is useful without internet access.

The current Mk4 feature set described by upstream includes:

- local Wi-Fi access point and browser UI;
- captive portal;
- movies and TV shows;
- music playback;
- image gallery;
- PDF, EPUB and comic reading;
- file browser and file management;
- persistent playback progress;
- admin panel;
- theme customization;
- background media-library indexing;
- large-file streaming with HTTP range requests;
- offline ZIM archives, including Wikipedia-style content after preprocessing with companion tools;
- multiple simultaneous clients/streams;
- LVGL-based local LCD UI;
- firmware/USB mode support;
- optional 3D-printed enclosure.

This is therefore one of the highest-relevance reference projects found so far for this lab: it is not merely portable to our hardware, but intentionally targets essentially the same Waveshare ESP32-S3-LCD-1.47 platform.

## Why it is especially relevant to our board

Upstream explicitly states that Nomad is built specifically for the Waveshare ESP32-S3 development board with the 1.47-inch LCD.

That makes it valuable for studying how far this exact hardware can be pushed as a real application platform rather than a simple display demo.

The project combines most of the resources available on our board:

```text
ESP32-S3
+ 1.47-inch ST7789 LCD
+ microSD
+ Wi-Fi access point
+ asynchronous HTTP server
+ browser frontend
+ LVGL local UI
+ Preferences/SPIFFS
+ native USB-related modes
+ BOOT button
+ RGB/status control
```

## Architecture in plain language

At a high level the system is split between firmware and files stored on the SD card.

The ESP32-S3 firmware:

- configures the board and display;
- starts Wi-Fi and DNS/captive-portal services;
- runs an asynchronous web server;
- provides API endpoints;
- serves media and files from microSD;
- handles HTTP Range requests for streaming and seeking;
- manages indexing and storage state;
- stores persistent device settings;
- drives the local LVGL display;
- coordinates background work with FreeRTOS queues, tasks and mutexes.

The microSD card contains both user media and much of the browser frontend. This separation is particularly interesting because the web UI can be changed or expanded without embedding every page directly into the firmware binary.

## Interesting engineering details

### 1. HTTP Range streaming

The firmware contains dedicated range-request handling and persistent stream handles. This is important for real video/audio playback because browsers often request only byte ranges rather than downloading a file sequentially from start to finish.

The current code also maintains a bounded set of concurrent stream handles and tracks active streaming state.

This is directly useful for our own experiments with ESP32-S3 as a local file/media server.

### 2. Large microSD libraries

Nomad maintains indexes rather than scanning the whole card for every browser operation.

The project has background indexing, queue-based work, cached storage statistics and logic to avoid unnecessary rescans at boot.

This is a useful reference for any future KONTAKTS file browser, local knowledge base or autonomous data appliance.

### 3. FreeRTOS and memory-pressure lessons

The firmware shows evidence of real stability work rather than only feature development.

The code uses:

- mutexes around shared SD/streaming state;
- FreeRTOS queues for LVGL messages;
- fixed-size buffers in memory-sensitive areas;
- explicit heap checks before indexing work;
- bounded stream counts;
- explicit cleanup of strings, vectors and file handles.

Upstream release notes also mention fixes for crashes involving files larger than 2 GB, heap corruption during concurrent indexing/storage work, and unnecessary full-screen LVGL redraws.

These are valuable negative lessons for our own firmware architecture.

### 4. LCD plus browser UI

Nomad uses both the local 1.47-inch screen and a richer browser interface.

This is a strong pattern for our board:

```text
small local display = status / basic control
browser UI          = configuration / rich interaction
```

It avoids trying to force a full desktop-style interface onto a 172 × 320 display while still keeping the device independently usable.

### 5. Offline knowledge archives

Mk4 adds support for ZIM archives such as offline Wikipedia, Gutenberg and similar collections.

The important engineering pattern is not to make the ESP32 search a huge raw archive directly. A companion desktop tool preprocesses the archive and builds a compact index, while the ESP32 serves the resulting content locally.

This is highly relevant to future offline knowledge-base or agent-support experiments: expensive preprocessing can happen on a PC, while the ESP32 becomes the portable serving/index-access layer.

### 6. Browser assets on SD

The repository includes a substantial `SD_Card_Template` containing HTML/JavaScript assets for movies, shows, music, books, gallery, files, admin functions and archives.

For our own firmware this suggests a clean architecture where the embedded binary provides APIs and hardware services while richer UI assets live on removable storage.

### 7. Admin/control plane

The upstream README describes device administration through the browser, including settings, library indexing, Wi-Fi, display brightness, RGB controls, restart/shutdown and file management.

This is directly relevant to the broader idea of turning ESP32-S3-LCD-1.47 into a configurable appliance rather than a one-purpose sketch.

## Framework and dependencies

Upstream currently documents Arduino IDE and exact versions for several libraries:

```text
ArduinoJson      7.3.0
Async TCP        3.4.7
ESP AsyncWebServer 3.7.1
LVGL             8.3.10
SdFat            2.3.0
```

Other visible firmware components include WiFi, ESPmDNS, DNSServer, SPIFFS, Preferences, ArduinoJson, asynchronous networking, LVGL and board-specific display/USB helpers.

Exact dependency versions are worth preserving when we create the local research copy because this is a large application where small library-version changes may affect behavior.

## Local copy policy

As with USB Army Knife, the third-party source should stay only in the ignored local research area, for example:

```text
Project Resources/
├── projects/
│   └── JcorpNomad/
│       └── README.md       <- only our public research card
└── _local/
    └── jcorp-nomad/        <- upstream clone, NOT committed
```

Recommended local clone from `Project Resources`:

```powershell
cd .\_local
git clone https://github.com/Jstudner/jcorp-nomad.git
cd .\jcorp-nomad
git rev-parse HEAD
```

Do not add the upstream source tree to our Git repository.

## Compatibility with Waveshare ESP32-S3-LCD-1.47

### Hardware differences

At first inspection this is unusually favorable: upstream specifically targets the Waveshare ESP32-S3 1.47-inch LCD board.

Therefore the main question is not basic porting but whether the exact board revision, Arduino core, SD-card wiring, flash layout and library versions match our known-good environment.

### Required adaptation

Unknown until the local source is built.

The first build should be made with the exact dependency versions declared by upstream before changing code.

Particular areas to compare with our already verified board data:

- TFT GPIO and orientation;
- microSD mode and pins;
- Flash/PSRAM assumptions;
- USB mode implementation;
- partition layout;
- BOOT-button handling;
- library versions;
- Arduino-ESP32 core version.

## Local test

Status: `REFERENCE_ONLY`

Not yet independently built or flashed in this lab at the time this card was created.

Recommended first-stage test sequence:

1. Clone upstream only into `_local/jcorp-nomad`.
2. Record exact commit with `git rev-parse HEAD`.
3. Inspect the firmware folder and board-specific support files.
4. Reproduce the author's exact Arduino/library environment.
5. Compile without modifications.
6. Inspect Flash/partition assumptions before flashing.
7. Preserve our current known-good firmware/backup.
8. Test benign functionality first: boot, LCD, SD, Wi-Fi hotspot and browser UI.
9. Only then test media indexing and streaming.
10. Record final status as `BUILD_VERIFIED`, `HARDWARE_VERIFIED`, `PARTIAL` or `NOT_COMPATIBLE`.

## What we should specifically measure

Nomad is interesting enough that simply confirming "it boots" is not sufficient.

Useful measurements for our lab would include:

- boot time;
- free heap/PSRAM after startup;
- SD-card throughput;
- browser response time;
- maximum practical file size;
- MP4 seek/range-request behavior;
- one versus multiple simultaneous clients;
- heap behavior during indexing;
- stability during simultaneous streaming and indexing;
- LCD responsiveness while network/SD tasks are active;
- Wi-Fi range and throughput;
- power consumption and temperature during sustained streaming.

This would give us a practical performance envelope for the board.

## What we can learn for our own projects

### A. ESP32-S3 as an autonomous micro-server

Nomad demonstrates that the board can be treated as a small appliance with its own network, UI, storage and application services, not merely as a microcontroller peripheral.

### B. Split local-display / browser-interface architecture

This may be one of the best UI patterns for future KONTAKTS applications: use the LCD for local state and immediate actions while using the browser for complex configuration and content navigation.

### C. SD-backed application content

Keeping large resources and web assets outside the firmware can make our own systems easier to update and customize.

### D. Preprocess on PC, serve on ESP32

The ZIM workflow demonstrates a useful general principle:

```text
heavy preprocessing on PC
          ↓
compact indexes/assets
          ↓
portable ESP32-S3 appliance
```

The same pattern could later apply to documentation, component databases, local semantic indexes or engineering reference material.

### E. Real concurrency lessons

The project's history of heap, large-file and indexing problems makes it valuable as a source of architectural lessons about what fails when Wi-Fi, SD, LVGL and asynchronous tasks are all active together.

## Licensing caution

The upstream README declares **CC BY-NC-SA 4.0**.

That is materially different from the MIT license of USB Army Knife. Because our broader lab may eventually feed commercial hardware/software projects, we should not copy Nomad source code directly into a commercial KONTAKTS product without a separate license review or permission from the author.

For now the correct use is:

- study the implementation;
- measure the behavior on our board;
- document general engineering principles;
- write our own independent implementation when needed.

## Our video

- Video: TODO after hardware verification

The future video description should include both:

```text
Original project:
https://github.com/Jstudner/jcorp-nomad

Our ESP32-S3-LCD-1.47 lab:
https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
```

## Current conclusion

Jcorp Nomad is a **very high-priority reference project** for this lab because it is a substantial real-world application built for the same Waveshare ESP32-S3-LCD-1.47 hardware.

The most valuable areas to investigate are:

```text
HTTP Range streaming
+ large microSD files
+ background indexing
+ AsyncWebServer
+ captive portal
+ SD-hosted web frontend
+ LVGL plus network workload
+ FreeRTOS/mutex architecture
+ USB/boot-mode handling
+ offline ZIM knowledge archives
+ enclosure/mechanical integration
```

The next gate is a reproducible local build from `_local/jcorp-nomad`, followed by controlled hardware verification.

## Provenance / attribution

This page documents an independent technical study of Jcorp Nomad. The original project, source code, media, product name and related assets remain attributable to Jackson Studner / Jcorp Tech and are governed by the upstream project's stated licensing terms. No third-party source code is included in this repository by this research card.
