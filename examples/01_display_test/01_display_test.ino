/*
  ESP32-S3-LCD-1.47 — Display Test
  =================================

  Board:
    Waveshare ESP32-S3-LCD-1.47

  Display:
    ST7789 IPS LCD, 172 x 320 pixels

  Purpose:
    Minimal Arduino IDE test of the built-in LCD. The sketch initializes the
    display over SPI, turns on the backlight and draws text plus a rectangle.
    It intentionally avoids LVGL, microSD, Wi-Fi and other peripherals so the
    LCD path can be tested independently.

  Verified on hardware:
    Successfully compiled, uploaded and tested on a physical
    Waveshare ESP32-S3-LCD-1.47. Arduino/esptool reported successful write
    verification ("Hash of data verified"), and the LCD displayed
    "Display test OK" as expected.

  Video proof:
    https://youtube.com/shorts/Xg4yZbiRnrQ

  Required Arduino libraries:
    - SPI                         (included with the ESP32 Arduino core)
    - Adafruit GFX Library
    - Adafruit ST7735 and ST7789 Library

  Arduino IDE board setup used by this project:
    Board:             ESP32S3 Dev Module
    USB Mode:          Hardware CDC and JTAG
    USB CDC On Boot:   Enabled
    Flash Size:        16MB (128Mb)
    PSRAM:             OPI PSRAM
    Flash Mode:        QIO 80MHz
    Partition Scheme:  a scheme suitable for 16 MB Flash

  Detailed installation and troubleshooting instructions:
    examples/01_display_test/README.md

  Project:
    https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47

  Notes:
    - This pinout is for ESP32-S3-LCD-1.47 with USB-A.
    - Do not assume the same pinout for ESP32-S3-LCD-1.47B or Touch variants.
    - Before replacing the factory firmware, see docs/firmware-backup.md.
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// -----------------------------------------------------------------------------
// Built-in ST7789 pinout for Waveshare ESP32-S3-LCD-1.47
// -----------------------------------------------------------------------------
constexpr int TFT_MOSI = 45;  // SPI data from ESP32-S3 to LCD
constexpr int TFT_SCLK = 40;  // SPI clock
constexpr int TFT_CS   = 42;  // LCD chip select
constexpr int TFT_DC   = 41;  // Data/command select
constexpr int TFT_RST  = 39;  // LCD hardware reset
constexpr int TFT_BL   = 48;  // LCD backlight control

// Adafruit_ST7789 uses the global ESP32 SPI object configured below in setup().
// MISO is not required here because this test only writes data to the display.
Adafruit_ST7789 tft(&SPI, TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Keep the backlight off while the LCD controller and framebuffer are being
  // initialized. This avoids showing random or partially initialized pixels.
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);

  // Configure the ESP32-S3 SPI bus explicitly for the board's built-in LCD.
  // Arguments: SCLK, MISO, MOSI, SS. MISO is -1 because it is unused here.
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  // Initialize the ST7789 for the physical 172 x 320 panel used on this board.
  // SPI_MODE0 is the normal mode for this display/controller combination.
  tft.init(172, 320, SPI_MODE0);

  // Rotation values 0..3 can be tested if another screen orientation is
  // desired. Rotation 0 is used as the baseline for this example.
  tft.setRotation(0);

  // Clear any previous or undefined LCD contents before enabling backlight.
  tft.fillScreen(ST77XX_BLACK);

  // The LCD is initialized and clean, so make it visible now.
  digitalWrite(TFT_BL, HIGH);

  // ---------------------------------------------------------------------------
  // Draw a small visual test pattern.
  // If all elements appear with the expected colors and positions, the SPI
  // connection, LCD controller, dimensions and basic graphics library path are
  // working.
  // ---------------------------------------------------------------------------

  // Prevent long text from automatically wrapping onto the next line.
  tft.setTextWrap(false);

  // First line: board family.
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(12, 36);
  tft.println("ESP32-S3");

  // Second line: display identification.
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(12, 70);
  tft.println("LCD 1.47");

  // Green frame provides an easy visual check of geometry and color output.
  tft.drawRect(8, 110, 156, 90, ST77XX_GREEN);

  // Final status text inside the frame.
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(22, 145);
  tft.println("Display test OK");
}

void loop() {
  // Nothing to update continuously. The ST7789 keeps the drawn image in its
  // display memory after setup() finishes.
}
