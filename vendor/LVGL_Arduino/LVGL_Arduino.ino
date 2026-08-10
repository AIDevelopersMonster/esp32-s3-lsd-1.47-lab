/**
 ******************************************************************************
 * @file     LVGL_Arduino.ino
 * @author   Yongqin Ou
 * @version  V1.0
 * @date     2024-10-30
 * @brief    Waveshare LVGL example, compatibility-adapted locally
 * @license  MIT
 * @copyright Copyright (c) 2024, Waveshare
 ******************************************************************************
 *
 * Local adaptation:
 * - LVGL 8.3.10 compatibility cleanup
 * - no Wi-Fi/BLE scans or background wireless task
 * - Wi-Fi and BLE are initialized once in setup(), status is saved, then both
 *   are switched off
 * - legacy "RAM Test" output removed
 * - board information is printed once to the active console with printf()
 *
 ******************************************************************************
 */

#include "SD_Card.h"
#include "Display_ST7789.h"
#include "LVGL_Driver.h"
#include "LVGL_Example.h"
#include "Wireless.h"
#include "RGB_lamp.h"
#include <esp_arduino_version.h>

static void Print_Board_Info()
{
  printf("\r\n");
  printf("========================================\r\n");
  printf(" Waveshare ESP32-S3-LCD-1.47\r\n");
  printf("========================================\r\n");

  printf("Chip model      : %s\r\n", ESP.getChipModel());
  printf("Chip revision   : %d\r\n", ESP.getChipRevision());
  printf("CPU cores       : %d\r\n", ESP.getChipCores());
  printf("CPU frequency   : %d MHz\r\n", ESP.getCpuFreqMHz());
  printf("Flash size      : %u MB\r\n", ESP.getFlashChipSize() / (1024 * 1024));
  printf("PSRAM size      : %u MB\r\n", ESP.getPsramSize() / (1024 * 1024));
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
  if (SDCard_Flag)
    printf("microSD         : %u MiB\r\n", SDCard_Size);
  else
    printf("microSD         : not mounted\r\n");

  printf("\r\n");
  printf("Wi-Fi init      : %s (now OFF)\r\n", WIFI_OK ? "OK" : "ERROR");
  printf("Bluetooth BLE   : %s (now OFF)\r\n", BLE_OK ? "OK" : "ERROR");

  printf("\r\n");
  printf("Arduino-ESP32   : %s\r\n", ESP_ARDUINO_VERSION_STR);
  printf("LVGL            : %d.%d.%d\r\n",
         LVGL_VERSION_MAJOR,
         LVGL_VERSION_MINOR,
         LVGL_VERSION_PATCH);

  printf("========================================\r\n\r\n");
}

void setup()
{
  // Vendor examples on this board use printf() for the active console.
  // This avoids depending on the Arduino Serial/USB CDC routing option.
  delay(1000);

  // Values used by the unchanged Waveshare LVGL panel.
  Flash_Size = ESP.getFlashChipSize() / (1024 * 1024);
  SD_Init();

  // One-time wireless initialization check. No scan, no task, no loop.
  // Both radios are OFF when this function returns.
  Wireless_Init_Check();

  Print_Board_Info();

  LCD_Init();
  Set_Backlight(90);
  Lvgl_Init();
  Lvgl_Example1();
}

void loop()
{
  Timer_Loop();
  RGB_Lamp_Loop(2);
  delay(5);
}
