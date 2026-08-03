#include <Arduino.h>
#include <WiFi.h>
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
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init(172, 320, SPI_MODE0);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  digitalWrite(TFT_BL, HIGH);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(8, 8);
  tft.println("Wi-Fi scan");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false, false);
  delay(100);

  const int count = WiFi.scanNetworks(false, true);
  tft.setCursor(8, 28);
  tft.printf("Found: %d", count);

  const int shown = min(count, 12);
  for (int i = 0; i < shown; ++i) {
    String ssid = WiFi.SSID(i);
    if (ssid.length() > 16) {
      ssid = ssid.substring(0, 13) + "...";
    }
    tft.setCursor(8, 50 + i * 20);
    tft.printf("%2d %-16s", i + 1, ssid.c_str());
    tft.setCursor(110, 62 + i * 20);
    tft.printf("%ld", static_cast<long>(WiFi.RSSI(i)));
  }

  WiFi.scanDelete();
}

void loop() {
}
