# Pinout and onboard resources

This repository targets the **Waveshare ESP32-S3-LCD-1.47** with the USB-A plug.
It is not the later `1.47B` USB-C board and not the touch-screen model.

## LCD

| LCD signal | ESP32-S3 GPIO |
|---|---:|
| MOSI | 45 |
| SCLK | 40 |
| CS | 42 |
| DC | 41 |
| RESET | 39 |
| Backlight | 48 |

Display controller: **ST7789**  
Panel resolution: **172 x 320**

## RGB LED

| Device | GPIO |
|---|---:|
| Addressable RGB LED | 38 |

## microSD / TF card

The onboard card slot is wired for 4-bit SD_MMC mode.

| SD signal | ESP32-S3 GPIO |
|---|---:|
| CMD | 15 |
| CLK | 14 |
| D0 | 16 |
| D1 | 18 |
| D2 | 17 |
| D3 | 21 |

## Buttons

| Button | GPIO / function |
|---|---|
| BOOT | GPIO0 |
| RESET | Hardware reset |

## Memory and radio

- ESP32-S3R8
- 16 MB external Flash
- 8 MB octal PSRAM
- 2.4 GHz Wi-Fi
- Bluetooth LE 5

Source: official Waveshare documentation and the board markings.
