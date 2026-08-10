/**
 ******************************************************************************
 * @file     Wireless.h
 * @brief    Wireless initialization-status interface for adapted LVGL demo
 ******************************************************************************
 * Vendor-derived Waveshare example, locally simplified.
 *
 * The adapted code exposes only one-time initialization state. It deliberately
 * does not expose scan counts because nearby-device counts are environmental
 * observations, not a deterministic board diagnostic.
 ******************************************************************************
 */

#pragma once

#include "WiFi.h"
#include <BLEDevice.h>

extern bool WIFI_OK;
extern bool BLE_OK;
extern bool Wireless_checked;

// Run once from setup(). No scan is performed and both radios are OFF again
// before the function returns.
void Wireless_Init_Check();
