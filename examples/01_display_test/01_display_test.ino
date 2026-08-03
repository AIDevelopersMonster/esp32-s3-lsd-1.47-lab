#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

constexpr int TFT_MOSI = 45;
constexpr int TFT_SCLK = 40;
constexpr int TFT_CS = 42;
constexpr int TFT_DC = 41;
constexpr int TFT_RST = 39;
constexpr int TFT_BL = 48;

Adafruit_ST7789 tft(&SPI, TFT_CS, TFT_DC, TFT_RST);

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init(172, 320, SPI_MODE0);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  digitalWrite(TFT_BL, HIGH);

  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(12, 36);
  tft.println("ESP32-S3");

  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(12, 70);
  tft.println("LCD 1.47");

  tft.drawRect(8, 110, 156, 90, ST77XX_GREEN);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(22, 145);
  tft.println("Display test OK");
}

void loop() {
}
