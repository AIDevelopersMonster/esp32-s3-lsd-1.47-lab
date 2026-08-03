#pragma once

#include <Arduino.h>

namespace Board {

// Display: ST7789, 172 x 320
inline constexpr int TFT_MOSI = 45;
inline constexpr int TFT_SCLK = 40;
inline constexpr int TFT_CS = 42;
inline constexpr int TFT_DC = 41;
inline constexpr int TFT_RST = 39;
inline constexpr int TFT_BL = 48;
inline constexpr uint16_t TFT_WIDTH = 172;
inline constexpr uint16_t TFT_HEIGHT = 320;

// Onboard addressable RGB LED
inline constexpr int RGB_LED = 38;

// BOOT button
inline constexpr int BOOT_BUTTON = 0;

// microSD / TF card in 4-bit SD_MMC mode
inline constexpr int SD_CMD = 15;
inline constexpr int SD_CLK = 14;
inline constexpr int SD_D0 = 16;
inline constexpr int SD_D1 = 18;
inline constexpr int SD_D2 = 17;
inline constexpr int SD_D3 = 21;

inline constexpr uint32_t SERIAL_BAUD = 115200;

}  // namespace Board
