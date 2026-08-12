# MJPEG Video Player release files

Release version planned for the first public hardware-verified build: `0.1.0`.

Expected files after a fresh Arduino IDE **Sketch -> Export Compiled Binary** and running:

```bat
tools\export-mjpeg-player-release.bat 0.1.0
```

are:

```text
MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47_v0.1.0.bin
MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
SHA256SUMS.txt
```

The binaries are intentionally not fabricated from an older build. They must be exported from the final public sketch and hardware-tested before this release directory is considered complete.

- `.bin` — application-only image, expected application offset `0x10000` for the documented Arduino layout.
- `.merged.bin` — complete one-file Arduino flash image, written from `0x0`.

See [`../README.md`](../README.md) for Arduino IDE settings, flashing commands, SD preparation and attribution.
