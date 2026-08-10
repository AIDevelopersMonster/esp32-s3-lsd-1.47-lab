/**
 ******************************************************************************
 * @file     Wireless.cpp
 * @brief    One-time Wi-Fi/BLE initialization check for the adapted LVGL demo
 ******************************************************************************
 * Vendor-derived Waveshare example, locally simplified.
 *
 * The original demo performed BLE and Wi-Fi environment scans and reported the
 * number of nearby devices. That result depends on the surrounding radio
 * environment and is not a meaningful LVGL or board self-test.
 *
 * This adapted version only checks whether each wireless subsystem can be
 * initialized during setup(). No scan is started and no background FreeRTOS
 * task is created. Wi-Fi is switched off immediately after its check; BLE is
 * deinitialized and its resources are released immediately after its check.
 *
 * W: OK / B: OK therefore means "initialization succeeded", not RF validation,
 * antenna validation, connection validation, or a complete hardware test.
 ******************************************************************************
 */

#include "Wireless.h"

bool WIFI_OK = false;
bool BLE_OK = false;
bool Wireless_checked = false;

void Wireless_Init_Check()
{
  // Wi-Fi: initialize station mode only; do not scan or connect.
  WIFI_OK = WiFi.mode(WIFI_STA);

  // The status has been captured, so leave the radio disabled for the rest of
  // the LVGL demonstration.
  WiFi.mode(WIFI_OFF);

  // BLE: initialize the stack only. Arduino-ESP32 3.3.11 returns bool here.
  BLE_OK = BLEDevice::init("ESP32-S3-LCD-1.47");

  // Release the BLE stack/resources if initialization reached an initialized
  // state. No advertising, scanning, or connection is attempted.
  if (BLEDevice::getInitialized()) {
    BLEDevice::deinit(true);
  }

  Wireless_checked = true;
}
