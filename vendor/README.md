# Vendor sources

This directory contains third-party/vendor code kept separate from the project's independently written examples.

## Rules for vendor-derived code

- Preserve the original vendor attribution and license information.
- Mark local changes explicitly.
- Document the tested hardware and dependency versions.
- Do not imply that locally modified code is an unmodified vendor release.
- Do not copy unrelated files from vendor archives unless their license and purpose are clear.

Current adapted vendor examples:

- [`LCD_Image/`](LCD_Image/) — Waveshare `LCD_Image` example adapted and hardware-tested with Arduino-ESP32 3.3.11 and PNGdec 1.1.6.
- [`LVGL_Arduino/`](LVGL_Arduino/) — Waveshare `LVGL_Arduino` example adapted and hardware-tested with Arduino-ESP32 3.3.11 and LVGL 8.3.10. See the [example README](LVGL_Arduino/README.md) for the compatibility changes, hardware-verified console output, microSD 16/32/64 GB verification, videos, and the published prebuilt merged image.

A prebuilt binary is intentionally kept with the vendor-derived source rather than under the independent project firmware tree:

- [`LVGL_Arduino/firmware/ESP32-S3-LCD-1.47-Waveshare-LVGL-adapted.bin`](LVGL_Arduino/firmware/ESP32-S3-LCD-1.47-Waveshare-LVGL-adapted.bin) — 16 MiB merged image of the locally adapted Waveshare LVGL demo.
- [`LVGL_Arduino/firmware/README.md`](LVGL_Arduino/firmware/README.md) — binary purpose, SHA-256, verification notes, and flashing instructions.

The prebuilt vendor-derived image is not the original Waveshare factory firmware, not the raw factory backup, and not the independent KONTAKTS firmware under the repository's top-level `firmware/` directory.

Project repository:

https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
