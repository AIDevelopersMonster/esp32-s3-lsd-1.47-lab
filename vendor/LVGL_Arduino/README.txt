Corrected files for Waveshare ESP32-S3-LCD-1.47 / LVGL_Arduino

Test target:
- Arduino-ESP32 3.3.11
- LVGL 8.3.10

Replace these files in the Waveshare LVGL_Arduino example folder:
- LVGL_Arduino.ino
- LVGL_Driver.h
- Wireless.cpp
- Wireless.h
- LVGL_Example.cpp
- LVGL_Example.h
- SD_Card.cpp
- SD_Card.h

Important:
- Keep lv_conf.h next to the lvgl library folder.
- LVGL_Driver.h no longer directly includes lv_conf.h or demos/lv_demos.h.
- Wi-Fi/BLE scans and the background WirelessScanTask are removed.
- Wi-Fi and BLE are initialized once, status saved, and both are then switched off.
- The existing LVGL panel layout is unchanged; its wireless value becomes W: OK/ERR B: OK/ERR.
- SD_MMC is initialized in 4-bit mode with no automatic formatting on mount failure.
- The legacy Flash_test()/"RAM Test" output is removed.
