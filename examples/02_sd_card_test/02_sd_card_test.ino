/*
  ESP32-S3-LCD-1.47 — microSD / SD_MMC Test
  ===========================================

  Board:
    Waveshare ESP32-S3-LCD-1.47

  Purpose:
    Read-only test of the built-in microSD/TF slot using the ESP32-S3 SD_MMC
    peripheral in 4-bit mode. The sketch mounts the card, prints its type and
    capacity to Serial Monitor, then lists files and directories in the root.

  Video:
    Hardware test demonstration:
    https://youtube.com/shorts/eVARX5pUqp4

  Libraries:
    - Arduino.h   (included with the ESP32 Arduino core)
    - SD_MMC.h    (included with the ESP32 Arduino core)

  No additional Arduino Library Manager package is required for this example.

  Recommended card for this project:
    - microSD/TF card no larger than 16 GB
    - FAT32 formatting is recommended

  Observed with the original factory firmware on the physical test board:
    - a 16 GB card was recognized, but the factory screen showed capacity "0";
    - a 32 GB card caused visible noise / corruption on the LCD.

  These two observations describe this specific board + factory-firmware test.
  They do NOT establish the root cause and should not be generalized to every
  ESP32-S3-LCD-1.47. For the project examples, cards <= 16 GB are therefore the
  conservative supported test range until larger cards are separately verified.

  Arduino IDE:
    Board:             ESP32S3 Dev Module
    USB Mode:          Hardware CDC and JTAG
    USB CDC On Boot:   Enabled
    Flash Size:        16MB (128Mb)
    PSRAM:             OPI PSRAM
    Flash Mode:        QIO 80MHz
    Serial Monitor:    115200 baud

  Pinout used by the built-in slot:
    CMD  = GPIO15
    CLK  = GPIO14
    D0   = GPIO16
    D1   = GPIO18
    D2   = GPIO17
    D3   = GPIO21

  Detailed instructions:
    examples/02_sd_card_test/README.md

  Project:
    https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47

  Safety / behavior:
    - This sketch is READ ONLY. It does not create, modify, format or delete
      files on the card.
    - Cards larger than 16 GB are reported as outside the current verified test
      range and the sketch stops before directory enumeration.
*/

#include <Arduino.h>
#include <SD_MMC.h>

// -----------------------------------------------------------------------------
// Built-in microSD/TF slot pinout for Waveshare ESP32-S3-LCD-1.47
// -----------------------------------------------------------------------------
constexpr int SD_CMD = 15;
constexpr int SD_CLK = 14;
constexpr int SD_D0  = 16;
constexpr int SD_D1  = 18;
constexpr int SD_D2  = 17;
constexpr int SD_D3  = 21;

// Conservative project limit based on the currently tested setup.
// 16 GiB is used as the binary threshold so ordinary marketed 16 GB cards are
// comfortably inside the accepted range while 32 GB cards are clearly outside.
constexpr uint64_t MAX_TESTED_CARD_BYTES = 16ULL * 1024ULL * 1024ULL * 1024ULL;

const char* cardTypeName(uint8_t type) {
  switch (type) {
    case CARD_MMC:
      return "MMC";
    case CARD_SD:
      return "SDSC";
    case CARD_SDHC:
      return "SDHC/SDXC";
    case CARD_NONE:
      return "NONE";
    default:
      return "UNKNOWN";
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("ESP32-S3-LCD-1.47 microSD test");
  Serial.println("Mode: READ ONLY / SD_MMC 4-bit");

  // Assign the actual board pins to the ESP32-S3 SD_MMC peripheral.
  if (!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3)) {
    Serial.println("ERROR: SD_MMC.setPins failed");
    return;
  }

  // The second argument is mode1bit. false selects the 4-bit bus and therefore
  // uses D0..D3, matching the built-in slot wiring of this board.
  if (!SD_MMC.begin("/sdcard", false)) {
    Serial.println("ERROR: Card mount failed");
    Serial.println("Check card insertion, FAT32 formatting and the <=16 GB test range.");
    return;
  }

  const uint8_t type = SD_MMC.cardType();
  if (type == CARD_NONE) {
    Serial.println("ERROR: No card inserted");
    SD_MMC.end();
    return;
  }

  const uint64_t cardBytes = SD_MMC.cardSize();
  const uint64_t cardMiB = cardBytes / (1024ULL * 1024ULL);

  Serial.printf("Card type: %s\n", cardTypeName(type));
  Serial.printf("Card size: %llu MiB\n",
                static_cast<unsigned long long>(cardMiB));

  // Keep cards beyond the current project test range from being treated as
  // validated by this example. Mounting succeeded, but we deliberately stop
  // before further access and ask for a <=16 GB card for the baseline test.
  if (cardBytes > MAX_TESTED_CARD_BYTES) {
    Serial.println("WARNING: Card is larger than the current <=16 GB project test range.");
    Serial.println("Use a <=16 GB microSD/TF card for the baseline hardware test.");
    SD_MMC.end();
    return;
  }

  // Open and enumerate the root directory. No files are created or modified.
  File root = SD_MMC.open("/");
  if (!root) {
    Serial.println("ERROR: Failed to open root directory");
    SD_MMC.end();
    return;
  }

  if (!root.isDirectory()) {
    Serial.println("ERROR: Root path is not a directory");
    root.close();
    SD_MMC.end();
    return;
  }

  Serial.println("Root directory:");

  bool foundAnyEntry = false;
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break;
    }

    foundAnyEntry = true;
    Serial.printf("%s%s  %llu bytes\n",
                  entry.isDirectory() ? "[DIR] " : "       ",
                  entry.name(),
                  static_cast<unsigned long long>(entry.size()));
    entry.close();
  }

  if (!foundAnyEntry) {
    Serial.println("(root directory is empty)");
  }

  root.close();

  Serial.println("SD card test OK");
  Serial.println("No files were changed.");
}

void loop() {
  // One-shot diagnostic test. Re-run with RESET after changing the card.
}
