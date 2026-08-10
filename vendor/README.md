# Vendor sources

This directory contains third-party/vendor code kept separate from the project's independently written examples.

## Rules for vendor-derived code

- Preserve the original vendor attribution and license information.
- Mark local changes explicitly.
- Document the tested hardware and dependency versions.
- Do not imply that locally modified code is an unmodified vendor release.
- Do not copy unrelated files from vendor archives unless their license and purpose are clear.

Current adapted vendor example:

- [`LCD_Image/`](LCD_Image/) — Waveshare `LCD_Image` example adapted and hardware-tested with Arduino-ESP32 3.3.11 and PNGdec 1.1.6.

Project repository:

https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
