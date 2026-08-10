#include "Wireless.h"

bool WIFI_OK = false;
bool BLE_OK = false;
bool Wireless_checked = false;

void Wireless_Init_Check()
{
  // Wi-Fi: initialize STA mode only, then immediately stop the radio.
  WIFI_OK = WiFi.mode(WIFI_STA);
  WiFi.mode(WIFI_OFF);

  // BLE: initialize the stack only, then immediately release it again.
  // Arduino-ESP32 3.3.11 BLEDevice::init() returns bool.
  BLE_OK = BLEDevice::init("ESP32-S3-LCD-1.47");

  if (BLEDevice::getInitialized()) {
    BLEDevice::deinit(true);
  }

  Wireless_checked = true;
}
