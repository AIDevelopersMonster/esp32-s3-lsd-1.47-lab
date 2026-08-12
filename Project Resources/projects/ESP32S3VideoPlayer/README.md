# ESP32-S3 MJPEG Video Player on Waveshare ESP32-S3-LCD-1.47

Status: `HARDWARE_VERIFIED`

## Hardware demo

[Waveshare ESP32-S3-LCD-1.47 — MJPEG Video Player hardware demo](https://youtube.com/shorts/Db57oCUU31w)

The video shows the port running on the physical board with automatic MJPEG playback from microSD and BOOT/NEXT control.

## Source / provenance

- Upstream: https://github.com/thelastoutpostworkshop/ESP32-C6-LCD-1.47_video_player
- Upstream author: thelastoutpostworkshop
- Upstream license: MIT
- Upstream commit used for the port: `6085a7e339de19356d399e8e2b48a0af152ce7a5`
- Upstream demo MJPEG files: https://github.com/thelastoutpostworkshop/ESP32-C6-LCD-1.47_video_player/tree/main/SD%20Content/mjpeg
- Video Conversion Studio: https://thelastoutpostworkshop.github.io/video_conversion/
- Tutorial: https://youtu.be/JqQEG0eipic
- Our public S3-only firmware: [`../../../firmware/MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47/README.md`](../../../firmware/MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47/README.md)

## Why it was interesting

This project is a compact demonstration of a useful embedded-media pipeline: compressed frames can be streamed from removable storage, buffered in PSRAM, decoded on ESP32-S3 and pushed directly to a small ST7789 display fast enough for convincing animation/video.

The result is more general than a novelty video player. The same pattern can drive animated status screens, autonomous product demos, short instructions, exhibition props, robot faces/panels, instrument UI animations and other offline visual terminals.

## What had to change for our board

The upstream project targets ESP32-C6-LCD-1.47. A direct SD port did not work because the Waveshare ESP32-S3-LCD-1.47 onboard card slot is naturally used through 4-bit SD_MMC with GPIO14/15/16/18/17/21 rather than the upstream C6 SPI-SD path.

The successful S3 port therefore uses:

```text
ST7789:  MOSI45 SCLK40 CS42 DC41 RESET39 BL48
SD_MMC:  CLK14 CMD15 D0=16 D1=18 D2=17 D3=21
BOOT:    GPIO0
PSRAM:   OPI, 8 MB
Flash:   16 MB
```

The MJPEG buffer was also moved to PSRAM and enlarged to 256 KiB. BOOT was simplified from an interrupt-driven skip flag to polling between decoded frames: short press means NEXT, while normal operation remains an automatic continuous demo loop.

## Hardware result

With a good microSD card the complete 20-file upstream sample set played successfully. Representative measurements were about 32.2 FPS for `alien_eye.mjpeg` (120 frames) and 21.7 FPS for `winston.mjpeg` (103 frames).

One especially useful negative result was the discovery that a failing microSD can look like a decoder/software bug: it may mount, list files and report correct sizes while selected videos return zero decoded frames. Replacing the card resolved the apparent parser failures.

## Publication policy

The upstream MJPEG media is **not** copied into this repository. We link to the upstream media and converter instead.

The S3-only adapted source is published because the upstream project is MIT licensed; the original MIT notice is retained in the firmware directory. Release binaries should be generated only from the final public sketch and hardware-checked before being marked complete.
