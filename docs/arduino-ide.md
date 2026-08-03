# Arduino IDE setup

## Board package

Install **esp32 by Espressif Systems**, version **3.0.2 or newer**.

Use these settings:

| Option | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| USB Mode | Hardware CDC and JTAG |
| USB CDC On Boot | Enabled |
| Flash Size | 16 MB |
| PSRAM | OPI PSRAM |
| Flash Mode | QIO 80 MHz |
| Partition Scheme | 16 MB default / suitable 16 MB scheme |

Install these libraries through Library Manager:

- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- Adafruit NeoPixel

Open an example from the `examples` directory and upload it.

## Entering download mode manually

When automatic upload fails:

1. Hold **BOOT**.
2. Press and release **RESET**.
3. Release **BOOT**.
4. Start uploading again.
5. Press RESET once after a successful upload if the program does not start automatically.
