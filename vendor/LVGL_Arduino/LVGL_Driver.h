/**
 ******************************************************************************
 * @file     LVGL_Driver.h
 * @brief    LVGL 8 display-driver declarations for Waveshare ESP32-S3-LCD-1.47
 ******************************************************************************
 * Vendor-derived source from the Waveshare LVGL_Arduino example.
 *
 * Local compatibility notes:
 * - tested with Arduino-ESP32 3.3.11 and LVGL 8.3.10;
 * - direct #include <lv_conf.h> was removed because LVGL loads its configuration
 *   internally from the standard lv_conf.h location next to the lvgl folder;
 * - unused #include <demos/lv_demos.h> was removed because this example does not
 *   call the LVGL demo API and the include caused a build failure when demos were
 *   not installed.
 *
 * See README.md in this directory for the complete adaptation history.
 ******************************************************************************
 */

#pragma once

#include <lvgl.h>
#include <esp_heap_caps.h>
#include "Display_ST7789.h"

#define LVGL_WIDTH    LCD_WIDTH
#define LVGL_HEIGHT   LCD_HEIGHT
#define LVGL_BUF_LEN  (LVGL_WIDTH * LVGL_HEIGHT / 20)

#define EXAMPLE_LVGL_TICK_PERIOD_MS  5

void Lvgl_print(const char * buf);
void Lvgl_Display_LCD(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void Lvgl_Touchpad_Read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
void example_increase_lvgl_tick(void *arg);

void Lvgl_Init(void);
void Timer_Loop(void);
