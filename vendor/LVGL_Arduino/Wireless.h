#pragma once

#include "WiFi.h"
#include <BLEDevice.h>

extern bool WIFI_OK;
extern bool BLE_OK;
extern bool Wireless_checked;

// One-time initialization check. No scan is performed.
// Wi-Fi and BLE are switched off again before this function returns.
void Wireless_Init_Check();
