#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <SD_MMC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_NeoPixel.h>

#include "board_config.h"

namespace {

Adafruit_ST7789 display(&SPI, Board::TFT_CS, Board::TFT_DC, Board::TFT_RST);
Adafruit_NeoPixel statusLed(1, Board::RGB_LED, NEO_GRB + NEO_KHZ800);

enum class Page : uint8_t {
  System = 0,
  WiFi = 1,
  Pinout = 2,
};

struct NetworkInfo {
  String ssid;
  int32_t rssi = 0;
  wifi_auth_mode_t auth = WIFI_AUTH_OPEN;
};

constexpr uint16_t COLOR_DARK_GREY = 0x7BEF;
constexpr uint16_t COLOR_LIGHT_GREY = 0xC618;

Page currentPage = Page::System;
bool sdMounted = false;
uint64_t sdSizeMb = 0;
NetworkInfo networks[8];
int networkCount = -1;

void setLed(uint8_t red, uint8_t green, uint8_t blue) {
  statusLed.setPixelColor(0, statusLed.Color(red, green, blue));
  statusLed.show();
}

void drawHeader(const __FlashStringHelper* title, uint16_t color) {
  display.fillScreen(ST77XX_BLACK);
  display.fillRect(0, 0, Board::TFT_WIDTH, 28, color);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);
  display.setCursor(8, 7);
  display.print(title);
  display.drawFastHLine(0, 29, Board::TFT_WIDTH, ST77XX_WHITE);
}

void drawFooter() {
  display.drawFastHLine(0, 298, Board::TFT_WIDTH, COLOR_DARK_GREY);
  display.setTextColor(COLOR_LIGHT_GREY);
  display.setTextSize(1);
  display.setCursor(7, 305);
  display.print(F("BOOT: next page"));
}

void printKeyValue(int16_t y, const __FlashStringHelper* key, const String& value,
                   uint16_t valueColor = ST77XX_CYAN) {
  display.setTextSize(1);
  display.setTextColor(ST77XX_WHITE);
  display.setCursor(8, y);
  display.print(key);
  display.setTextColor(valueColor);
  display.setCursor(74, y);
  display.print(value);
}

String megabytes(uint64_t bytes) {
  return String(static_cast<double>(bytes) / (1024.0 * 1024.0), 1) + F(" MB");
}

bool initializeSdCard() {
  Serial.println(F("[SD] Configuring SD_MMC pins..."));

  if (!SD_MMC.setPins(Board::SD_CLK, Board::SD_CMD, Board::SD_D0,
                      Board::SD_D1, Board::SD_D2, Board::SD_D3)) {
    Serial.println(F("[SD] setPins failed"));
    return false;
  }

  if (!SD_MMC.begin("/sdcard", false, false, SDMMC_FREQ_DEFAULT, 5)) {
    Serial.println(F("[SD] Mount failed or no card inserted"));
    return false;
  }

  if (SD_MMC.cardType() == CARD_NONE) {
    Serial.println(F("[SD] No card detected"));
    SD_MMC.end();
    return false;
  }

  sdSizeMb = SD_MMC.cardSize() / (1024ULL * 1024ULL);
  Serial.printf("[SD] Mounted, capacity: %llu MB\n",
                static_cast<unsigned long long>(sdSizeMb));
  return true;
}

void initializeDisplay() {
  pinMode(Board::TFT_BL, OUTPUT);
  digitalWrite(Board::TFT_BL, LOW);

  SPI.begin(Board::TFT_SCLK, -1, Board::TFT_MOSI, Board::TFT_CS);
  display.init(Board::TFT_WIDTH, Board::TFT_HEIGHT, SPI_MODE0);
  display.setRotation(0);
  display.setTextWrap(false);
  display.fillScreen(ST77XX_BLACK);

  digitalWrite(Board::TFT_BL, HIGH);
}

void drawSystemPage() {
  drawHeader(F("SYSTEM"), ST77XX_BLUE);

  printKeyValue(43, F("Chip"), String(ESP.getChipModel()));
  printKeyValue(61, F("Cores"), String(ESP.getChipCores()));
  printKeyValue(79, F("CPU"), String(ESP.getCpuFreqMHz()) + F(" MHz"));
  printKeyValue(97, F("Flash"), megabytes(ESP.getFlashChipSize()));
  printKeyValue(115, F("PSRAM"), megabytes(ESP.getPsramSize()));
  printKeyValue(133, F("Free heap"), megabytes(ESP.getFreeHeap()));
  printKeyValue(151, F("Free PSRAM"), megabytes(ESP.getFreePsram()));

  if (sdMounted) {
    printKeyValue(178, F("SD card"), String(sdSizeMb) + F(" MB"), ST77XX_GREEN);
  } else {
    printKeyValue(178, F("SD card"), F("not mounted"), ST77XX_YELLOW);
  }

  printKeyValue(196, F("Wi-Fi"), F("ready"));
  printKeyValue(214, F("Display"), F("ST7789 172x320"));
  printKeyValue(232, F("USB"), F("native CDC"));

  display.setTextColor(COLOR_LIGHT_GREY);
  display.setCursor(8, 260);
  display.print(F("Waveshare ESP32-S3"));
  display.setCursor(8, 274);
  display.print(F("LCD 1.47 Lab"));

  drawFooter();
}

void scanWiFi() {
  setLed(0, 0, 32);
  networkCount = 0;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false, false);
  delay(120);

  Serial.println(F("[WiFi] Scanning..."));
  const int found = WiFi.scanNetworks(false, true);

  if (found <= 0) {
    networkCount = found;
    Serial.printf("[WiFi] Scan result: %d\n", found);
    setLed(sdMounted ? 0 : 32, sdMounted ? 32 : 12, 0);
    return;
  }

  networkCount = min(found, 8);
  for (int i = 0; i < networkCount; ++i) {
    networks[i].ssid = WiFi.SSID(i);
    networks[i].rssi = WiFi.RSSI(i);
    networks[i].auth = WiFi.encryptionType(i);
  }
  WiFi.scanDelete();

  Serial.printf("[WiFi] Found %d network(s), showing %d\n", found, networkCount);
  setLed(0, 32, 0);
}

String clippedSsid(const String& ssid) {
  if (ssid.length() <= 16) {
    return ssid;
  }
  return ssid.substring(0, 13) + F("...");
}

void drawWiFiPage() {
  drawHeader(F("WI-FI SCAN"), ST77XX_MAGENTA);

  display.setTextSize(1);
  display.setCursor(8, 40);

  if (networkCount < 0) {
    display.setTextColor(ST77XX_YELLOW);
    display.print(F("Scan not started."));
  } else if (networkCount == 0) {
    display.setTextColor(ST77XX_YELLOW);
    display.print(F("No networks found."));
  } else {
    for (int i = 0; i < networkCount; ++i) {
      const int16_t y = 40 + i * 29;
      display.setTextColor(ST77XX_WHITE);
      display.setCursor(8, y);
      display.printf("%d. %s", i + 1, clippedSsid(networks[i].ssid).c_str());

      display.setCursor(18, y + 13);
      display.setTextColor(networks[i].rssi > -65 ? ST77XX_GREEN : ST77XX_YELLOW);
      display.printf("%ld dBm", static_cast<long>(networks[i].rssi));

      display.setTextColor(COLOR_LIGHT_GREY);
      display.setCursor(93, y + 13);
      display.print(networks[i].auth == WIFI_AUTH_OPEN ? F("OPEN") : F("LOCK"));
    }
  }

  drawFooter();
}

void drawPinoutPage() {
  drawHeader(F("ONBOARD PINS"), ST77XX_GREEN);

  display.setTextSize(1);
  display.setTextColor(ST77XX_WHITE);

  const char* lines[] = {
      "LCD MOSI   GPIO45",
      "LCD SCLK   GPIO40",
      "LCD CS     GPIO42",
      "LCD DC     GPIO41",
      "LCD RESET  GPIO39",
      "LCD BL     GPIO48",
      "",
      "RGB LED    GPIO38",
      "BOOT       GPIO0",
      "",
      "SD CLK     GPIO14",
      "SD CMD     GPIO15",
      "SD D0      GPIO16",
      "SD D1      GPIO18",
      "SD D2      GPIO17",
      "SD D3      GPIO21",
  };

  int16_t y = 41;
  for (const char* line : lines) {
    display.setCursor(8, y);
    display.print(line);
    y += 15;
  }

  drawFooter();
}

void drawCurrentPage() {
  switch (currentPage) {
    case Page::System:
      drawSystemPage();
      break;
    case Page::WiFi:
      drawWiFiPage();
      break;
    case Page::Pinout:
      drawPinoutPage();
      break;
  }
}

void nextPage() {
  const auto next = (static_cast<uint8_t>(currentPage) + 1U) % 3U;
  currentPage = static_cast<Page>(next);

  if (currentPage == Page::WiFi) {
    drawHeader(F("WI-FI SCAN"), ST77XX_MAGENTA);
    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(1);
    display.setCursor(8, 45);
    display.print(F("Scanning..."));
    scanWiFi();
  }

  drawCurrentPage();
}

void handleBootButton() {
  static bool lastRaw = HIGH;
  static bool stableState = HIGH;
  static uint32_t changedAt = 0;

  const bool raw = digitalRead(Board::BOOT_BUTTON);
  if (raw != lastRaw) {
    lastRaw = raw;
    changedAt = millis();
  }

  if ((millis() - changedAt) > 35 && raw != stableState) {
    stableState = raw;
    if (stableState == LOW) {
      nextPage();
    }
  }
}

}  // namespace

void setup() {
  pinMode(Board::BOOT_BUTTON, INPUT_PULLUP);

  Serial.begin(Board::SERIAL_BAUD);
  delay(350);
  Serial.println();
  Serial.println(F("ESP32-S3-LCD-1.47 Lab"));

  statusLed.begin();
  statusLed.setBrightness(40);
  setLed(0, 0, 32);

  initializeDisplay();

  drawHeader(F("STARTING"), ST77XX_BLUE);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(1);
  display.setCursor(8, 45);
  display.print(F("Checking hardware..."));

  sdMounted = initializeSdCard();
  setLed(sdMounted ? 0 : 32, sdMounted ? 32 : 12, 0);

  drawSystemPage();
}

void loop() {
  handleBootButton();
  delay(5);
}
