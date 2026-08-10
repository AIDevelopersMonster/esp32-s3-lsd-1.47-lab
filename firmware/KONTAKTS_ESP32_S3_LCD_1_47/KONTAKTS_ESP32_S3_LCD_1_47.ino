/*
 * KONTAKTS_ESP32-S3-LCD-1.47 firmware
 * ====================================
 * Version: 0.1.0
 * Target:  Waveshare ESP32-S3-LCD-1.47 (USB-A variant)
 * License: MIT (same project license)
 *
 * This is an independently written project firmware, not a copy of the
 * Waveshare factory demo. It uses hardware details that were verified in this
 * repository before the firmware was created:
 *
 *   - ESP32-S3, 16 MB Flash, 8 MB OPI PSRAM
 *   - ST7789 LCD, 172 x 320
 *   - LCD: MOSI45 / SCLK40 / CS42 / DC41 / RST39 / BL48
 *   - RGB LED: GPIO38
 *   - SD_MMC 4-bit: CMD15 / CLK14 / D0=16 / D1=18 / D2=17 / D3=21
 *
 * Behavior:
 *   - initializes the LCD and shows a KONTAKTS information screen;
 *   - mounts microSD read-only in normal use (no format request is made);
 *   - initializes Wi-Fi once, records success, then switches Wi-Fi OFF;
 *   - initializes BLE once, records success, then deinitializes BLE;
 *   - prints a one-shot hardware/firmware report with printf();
 *   - performs no Wi-Fi/BLE scan and starts no diagnostic background task.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47
 */

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <esp_arduino_version.h>

// -----------------------------------------------------------------------------
// Firmware identity
// -----------------------------------------------------------------------------
static constexpr const char* FW_NAME    = "KONTAKTS_ESP32-S3-LCD-1.47";
static constexpr const char* FW_VERSION = "0.1.0";

// -----------------------------------------------------------------------------
// Verified board pinout
// -----------------------------------------------------------------------------
static constexpr int TFT_MOSI = 45;
static constexpr int TFT_SCLK = 40;
static constexpr int TFT_CS   = 42;
static constexpr int TFT_DC   = 41;
static constexpr int TFT_RST  = 39;
static constexpr int TFT_BL   = 48;
static constexpr int RGB_LED  = 38;

static constexpr int SD_CMD = 15;
static constexpr int SD_CLK = 14;
static constexpr int SD_D0  = 16;
static constexpr int SD_D1  = 18;
static constexpr int SD_D2  = 17;
static constexpr int SD_D3  = 21;

Adafruit_ST7789 tft(&SPI, TFT_CS, TFT_DC, TFT_RST);

struct BootInfo {
  bool sdMounted = false;
  uint32_t sdMiB = 0;
  bool wifiOK = false;
  bool bleOK = false;
};

static BootInfo bootInfo;

// -----------------------------------------------------------------------------
// One-shot subsystem initialization
// -----------------------------------------------------------------------------
static void initStorage()
{
  if (!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3)) {
    return;
  }

  // false = 4-bit mode. No formatting is requested on failure.
  if (!SD_MMC.begin("/sdcard", false)) {
    return;
  }

  if (SD_MMC.cardType() == CARD_NONE) {
    SD_MMC.end();
    return;
  }

  bootInfo.sdMiB = static_cast<uint32_t>(SD_MMC.totalBytes() / (1024ULL * 1024ULL));
  bootInfo.sdMounted = true;
}

static void initWirelessOnce()
{
  // Wi-Fi: initialize only long enough to verify that the Arduino subsystem can
  // enter STA mode. This is not an RF/link/range test.
  bootInfo.wifiOK = WiFi.mode(WIFI_STA);
  WiFi.mode(WIFI_OFF);

  // Arduino-ESP32 3.3.11 BLEDevice::init() returns bool.
  // No scan, advertising or connection is started.
  bootInfo.bleOK = BLEDevice::init(FW_NAME);
  if (BLEDevice::getInitialized()) {
    BLEDevice::deinit(true);
  }
}

// -----------------------------------------------------------------------------
// Console report
// -----------------------------------------------------------------------------
static void printBootReport()
{
  printf("\r\n");
  printf("================================================\r\n");
  printf(" %s\r\n", FW_NAME);
  printf(" Firmware v%s\r\n", FW_VERSION);
  printf("================================================\r\n");
  printf("Chip model      : %s\r\n", ESP.getChipModel());
  printf("Chip revision   : %d\r\n", ESP.getChipRevision());
  printf("CPU cores       : %d\r\n", ESP.getChipCores());
  printf("CPU frequency   : %d MHz\r\n", ESP.getCpuFreqMHz());
  printf("Flash size      : %u MB\r\n", ESP.getFlashChipSize() / (1024U * 1024U));
  printf("PSRAM size      : %u MB\r\n", ESP.getPsramSize() / (1024U * 1024U));
  printf("Free heap       : %u bytes\r\n", ESP.getFreeHeap());
  printf("Free PSRAM      : %u bytes\r\n", ESP.getFreePsram());
  printf("\r\n");
  printf("Display         : ST7789\r\n");
  printf("Resolution      : 172 x 320\r\n");
  printf("LCD MOSI        : GPIO45\r\n");
  printf("LCD SCLK        : GPIO40\r\n");
  printf("LCD CS          : GPIO42\r\n");
  printf("LCD DC          : GPIO41\r\n");
  printf("LCD RESET       : GPIO39\r\n");
  printf("LCD backlight   : GPIO48\r\n");
  printf("RGB LED         : GPIO38\r\n");
  printf("\r\n");
  printf("SD CMD          : GPIO15\r\n");
  printf("SD CLK          : GPIO14\r\n");
  printf("SD D0           : GPIO16\r\n");
  printf("SD D1           : GPIO18\r\n");
  printf("SD D2           : GPIO17\r\n");
  printf("SD D3           : GPIO21\r\n");
  if (bootInfo.sdMounted) {
    printf("microSD         : %u MiB\r\n", bootInfo.sdMiB);
  } else {
    printf("microSD         : not mounted\r\n");
  }
  printf("\r\n");
  printf("Wi-Fi init      : %s (now OFF)\r\n", bootInfo.wifiOK ? "OK" : "ERROR");
  printf("Bluetooth BLE   : %s (now OFF)\r\n", bootInfo.bleOK ? "OK" : "ERROR");
  printf("\r\n");
  printf("Arduino-ESP32   : %s\r\n", ESP_ARDUINO_VERSION_STR);
  printf("Firmware        : v%s\r\n", FW_VERSION);
  printf("================================================\r\n\r\n");
}

// -----------------------------------------------------------------------------
// KONTAKTS information screen
// -----------------------------------------------------------------------------
static void drawRow(int16_t y, const char* label, const char* value, uint16_t valueColor = ST77XX_WHITE)
{
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.setCursor(10, y);
  tft.print(label);

  tft.setTextColor(valueColor, ST77XX_BLACK);
  tft.setCursor(76, y);
  tft.print(value);
}

static void drawScreen()
{
  char value[40];

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 10);
  tft.print("KONTAKTS");

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(10, 34);
  tft.print("ESP32-S3-LCD-1.47");

  tft.drawFastHLine(8, 50, 156, ST77XX_BLUE);

  drawRow(62,  "CHIP",  ESP.getChipModel());

  snprintf(value, sizeof(value), "%d MHz / %d cores", ESP.getCpuFreqMHz(), ESP.getChipCores());
  drawRow(82,  "CPU", value);

  snprintf(value, sizeof(value), "%u MB", ESP.getFlashChipSize() / (1024U * 1024U));
  drawRow(102, "FLASH", value);

  snprintf(value, sizeof(value), "%u MB", ESP.getPsramSize() / (1024U * 1024U));
  drawRow(122, "PSRAM", value);

  if (bootInfo.sdMounted) {
    snprintf(value, sizeof(value), "%u MiB", bootInfo.sdMiB);
    drawRow(142, "SD", value, ST77XX_GREEN);
  } else {
    drawRow(142, "SD", "NOT MOUNTED", ST77XX_RED);
  }

  drawRow(162, "WI-FI", bootInfo.wifiOK ? "OK / OFF" : "ERROR", bootInfo.wifiOK ? ST77XX_GREEN : ST77XX_RED);
  drawRow(182, "BLE", bootInfo.bleOK ? "OK / OFF" : "ERROR", bootInfo.bleOK ? ST77XX_GREEN : ST77XX_RED);
  drawRow(202, "DISPLAY", "ST7789");
  drawRow(222, "RES", "172 x 320");

  snprintf(value, sizeof(value), "v%s", FW_VERSION);
  drawRow(242, "FW", value, ST77XX_YELLOW);

  tft.drawRect(7, 270, 158, 36, ST77XX_GREEN);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(18, 282);
  tft.print("KONTAKTS firmware ready");
}

void setup()
{
  // Give the active ESP32-S3 console time to become available. printf() is used
  // because it worked reliably in the hardware-verified vendor experiment.
  delay(1000);

  initStorage();
  initWirelessOnce();
  printBootReport();

  // LCD path is based on the independently hardware-verified display test from
  // examples/01_display_test, not on the Waveshare LVGL demo driver.
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init(172, 320, SPI_MODE0);
  tft.setRotation(0);
  drawScreen();

  digitalWrite(TFT_BL, HIGH);

  // A dim green indication that setup completed. This uses the ESP32 Arduino
  // built-in helper for addressable RGB LEDs; no animation/background task.
  neopixelWrite(RGB_LED, 0, 24, 0);
}

void loop()
{
  // v0.1 is intentionally static and deterministic. The LCD controller retains
  // the image and both wireless subsystems remain OFF after setup().
  delay(1000);
}
