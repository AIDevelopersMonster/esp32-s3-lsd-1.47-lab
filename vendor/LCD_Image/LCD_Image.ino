/**
 ******************************************************************************
 * @file     LCD_Image.ino
 * @author   Yongqin Ou
 * @version  V1.0
 * @date     2024-10-31
 * @brief    Setup experiment for SD card, display, and RGB lamp
 * @license  MIT
 * @copyright Copyright (c) 2024, Waveshare
 ******************************************************************************
 *
 * LOCAL PROJECT NOTE
 * ------------------
 * This is a vendor-derived Waveshare example kept under vendor/ with local
 * compatibility fixes in its companion source files.
 *
 * Hardware-verified environment used by this repository:
 *   - Waveshare ESP32-S3-LCD-1.47
 *   - esp32 by Espressif Systems 3.3.11
 *   - PNGdec 1.1.6
 *
 * The downloaded vendor example did not run unchanged in this environment.
 * The observed incompatibilities may be caused by dependency/API differences
 * from the versions used when the original 2024 example was prepared. This
 * repository therefore documents the tested behavior and local fixes without
 * claiming that every version of the original vendor package is affected.
 *
 * See README.md in this directory for provenance, exact changes and test status.
 ******************************************************************************
 * 
 * Experiment Objective: Learn how to set up and use SD card for image display and control an RGB lamp.
 *
 * Hardware Resources and Pin Assignment: 
 * 1. SD Card Interface --> As configured in SD_Card.h.
 * 2. Display Interface --> As configured in Display_ST7789.h.
 * 3. RGB Lamp Interface --> As configured in RGB_lamp.h.
 *
 * Experiment Phenomenon:
 * 1. Runs tests and initializes SD card, display, and sets backlight.
 * 2. Continuously loops through images on the SD card and controls the RGB lamp.
 * 
 * Notes:
 * None
 * 
 ******************************************************************************
 * 
 * Development Platform: ESP32
 * Support Forum: service.waveshare.com
 * Company Website: www.waveshare.com
 *
 ******************************************************************************
 */

/*
   You must copy the PNG File from the SD Card File folder to the SD card and connect the SD card to the device
*/
#include "SD_Card.h"
#include "Display_ST7789.h"
#include "LCD_Image.h"
#include "RGB_lamp.h"

void setup()
{
  Flash_test();
  SD_Init();     
  LCD_Init();
  Set_Backlight(90);  
}

void loop()
{
  Image_Next_Loop("/",".png",300);
  RGB_Lamp_Loop(2);
  delay(5);
}