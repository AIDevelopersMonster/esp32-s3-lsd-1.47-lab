#include <Arduino.h>
#include <SD_MMC.h>

constexpr int SD_CMD = 15;
constexpr int SD_CLK = 14;
constexpr int SD_D0 = 16;
constexpr int SD_D1 = 18;
constexpr int SD_D2 = 17;
constexpr int SD_D3 = 21;

void setup() {
  Serial.begin(115200);
  delay(500);

  if (!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3)) {
    Serial.println("SD_MMC.setPins failed");
    return;
  }

  if (!SD_MMC.begin("/sdcard", false)) {
    Serial.println("Card mount failed");
    return;
  }

  if (SD_MMC.cardType() == CARD_NONE) {
    Serial.println("No card inserted");
    return;
  }

  Serial.printf("Card size: %llu MB\n",
                static_cast<unsigned long long>(
                    SD_MMC.cardSize() / (1024ULL * 1024ULL)));

  File root = SD_MMC.open("/");
  File entry = root.openNextFile();
  while (entry) {
    Serial.printf("%s%s  %llu bytes\n",
                  entry.isDirectory() ? "[DIR] " : "",
                  entry.name(),
                  static_cast<unsigned long long>(entry.size()));
    entry = root.openNextFile();
  }
}

void loop() {
}
