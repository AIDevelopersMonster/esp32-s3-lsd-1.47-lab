# USB Army Knife on Waveshare ESP32-S3-LCD-1.47

Status: `REFERENCE_ONLY`

## Source

- Original project: https://github.com/i-am-shodan/USBArmyKnife
- Author: i-am-shodan
- License: MIT
- Manufacturer-listed video: https://www.youtube.com/watch?v=eMnJFsxMS94
- Related video: https://www.youtube.com/watch?v=roNRKHlf9f0
- Browser flasher referenced by the video: https://esp.huhn.me/
- Auxiliary Espressif binary referenced by the video: https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/boot_app0.bin
- Date accessed: 2026-08-10

## What this project actually is

USB Army Knife is a multi-function ESP32-S3 USB/Wi-Fi/Bluetooth platform aimed at penetration-testing / red-team laboratory use. The upstream project combines several device roles in one small ESP32-S3-based USB device.

The upstream README describes capabilities including:

- USB HID / DuckyScript automation;
- USB mass-storage emulation;
- USB network-device emulation;
- Wi-Fi and Bluetooth functionality derived in part from ESP32 Marauder;
- web-based control;
- display-driven status/UI;
- microSD-backed files on boards that provide an SD interface.

This is therefore not just a display demo. It is a relatively large application framework that uses the ESP32-S3 native USB capabilities together with Wi-Fi/Bluetooth and optional local UI/storage.

## Why it is relevant to our board

The upstream project explicitly contains a PlatformIO environment named:

```text
Waveshare-ESP32-S3-LCD-1_47
```

That environment defines the same main onboard interfaces we have already verified independently for our board:

```text
TFT RESET  GPIO39
TFT DC     GPIO41
TFT MOSI   GPIO45
TFT CS     GPIO42
TFT SCLK   GPIO40
TFT BL     GPIO48

SD D0      GPIO16
SD D1      GPIO18
SD D2      GPIO17
SD D3      GPIO21
SD CLK     GPIO14
SD CMD     GPIO15

BOOT       GPIO0
RGB LED    GPIO38
```

It also declares a 172 × 320 logical TFT configuration and 16 MB Flash.

That makes USB Army Knife especially interesting for this lab: it is not merely a generic ESP32-S3 project that might be portable; the upstream project intentionally includes our exact Waveshare board as a supported build target.

## Important distinction: one project, two supporting flashing resources

The video description lists three links, but they have different roles.

### 1. USB Army Knife

```text
https://github.com/i-am-shodan/USBArmyKnife
```

This is the actual application/project.

### 2. `boot_app0.bin`

```text
https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/boot_app0.bin
```

This file belongs to Espressif's Arduino-ESP32 distribution, not to USB Army Knife itself. It is a small auxiliary boot/OTA-selection data image used in traditional Arduino-ESP32 flash layouts. In a manual multi-image flashing procedure, such support images may be written alongside the bootloader, partition table and application image at their required offsets.

It should therefore be treated as a **flashing/layout component**, not as an independent project and not as USB Army Knife application code.

### 3. ESPWebTool / Huhn web flasher

```text
https://esp.huhn.me/
```

This is a browser-based ESP flashing utility. It is the delivery tool used to place prepared binary images into the ESP32 Flash without requiring the user to install a local development environment.

It is also **not part of the USB Army Knife application architecture**.

The relationship is:

```text
USB Army Knife source/release binaries
             │
             ├── application firmware
             ├── bootloader / partitions / support image(s)
             │
             ▼
        browser flasher
             │
             ▼
     Waveshare ESP32-S3-LCD-1.47
```

## Framework and build system

The upstream repository uses PlatformIO and Arduino-ESP32. Its current configuration contains a dedicated Waveshare environment extending an ESP32-S3 core configuration.

The project is substantially more complicated than our current lab firmware. Its dependency tree includes, among other components:

- ArduinoJson;
- DuckyScript interpreter code;
- TinyUSB-related code;
- asynchronous web-server/networking components;
- NimBLE-Arduino;
- LovyanGFX;
- FastLED;
- ESP32 Marauder-derived functionality.

For this reason we should not begin by flashing unknown prebuilt binaries. The preferred research route is to inspect the exact release/build instructions, identify the expected flash layout, then build the Waveshare target locally in our isolated `_local/` research area.

## Security and safety scope for our lab

The upstream software includes offensive-security / red-team capabilities such as HID automation, network emulation and wireless attack tooling.

Our Project Resources investigation is therefore limited to controlled, owned hardware and defensive/educational analysis. The useful engineering questions for this lab are primarily:

- how ESP32-S3 native USB composite devices are implemented;
- how the project switches USB roles;
- how the display and button are integrated into a larger application;
- how microSD is used for scripts/assets;
- how a local web interface controls an embedded device;
- how one codebase supports multiple ESP32-S3 boards;
- how a browser-flashing workflow is packaged for end users.

We do not need to reproduce intrusive payloads to learn these architectural lessons.

## What we should test first

Before any full application experiment:

1. Clone/download the upstream source into:

```text
Project Resources/_local/USBArmyKnife/source/
```

2. Record the exact upstream commit/release used.
3. Inspect its Waveshare PlatformIO target.
4. Build only:

```text
Waveshare-ESP32-S3-LCD-1_47
```

5. Compare generated flash layout with our known-good 16 MB board configuration.
6. Preserve our current firmware/backup before writing anything.
7. Prefer a locally built image over an unidentified binary from a video description.
8. First verify benign functions: boot, display, button, SD and web UI.

Only after those checks should the status move from `REFERENCE_ONLY` to `BUILD_VERIFIED` or `HARDWARE_VERIFIED`.

## What we can learn for our own projects

The most valuable ideas are broader than the security payloads themselves.

### A. Multi-role USB appliance

Our board's USB-A form factor and ESP32-S3 native USB make it suitable for applications that appear to a host as different USB device classes. A benign KONTAKTS project could explore, for example, a configurable diagnostic USB appliance.

### B. On-device launcher

USB Army Knife demonstrates the value of combining a small LCD, one physical button and stored scripts/assets into a self-contained launcher. We could develop our own menu-driven laboratory toolkit for hardware diagnostics, serial/I2C utilities, file viewing or test automation.

### C. Web control plane

A device-local web interface is an attractive way to configure a small screen-equipped ESP32-S3 without adding many buttons. This idea can be reused independently for our own tools.

### D. Browser installation

The use of a browser flasher shows how a finished project can be distributed to non-developers without asking them to install PlatformIO or Arduino IDE. This is directly relevant to a future KONTAKTS web installer.

## Our current conclusion

USB Army Knife is a **high-relevance reference project** because its upstream source directly supports `Waveshare-ESP32-S3-LCD-1_47` and exercises far more of the ESP32-S3 feature set than a simple display demo.

For our purposes, the right first goal is not to copy the project. It is to use it as a case study in:

```text
native USB
+ LCD UI
+ microSD
+ Wi-Fi/Bluetooth
+ web control
+ multi-board PlatformIO architecture
+ end-user web flashing
```

A later independent project can reuse those general engineering patterns with our own functionality and code.

## Provenance / attribution

This page is an independent technical study. USB Army Knife remains the work of its upstream author and is distributed under its upstream MIT license. Espressif's `boot_app0.bin` and the Huhn browser flasher are separate upstream resources. No third-party source code is included in this repository by this research card.
