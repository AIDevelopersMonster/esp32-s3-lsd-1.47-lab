/**
 ******************************************************************************
 * @file     SD_Card.cpp
 * @brief    SD_MMC support for the adapted Waveshare LVGL_Arduino example
 ******************************************************************************
 * Vendor-derived Waveshare source with a local compatibility change.
 *
 * The original demo used:
 *
 *   SD_MMC.begin("/sdcard", true, true)
 *
 * In the current Arduino-ESP32 API that selects 1-bit mode and enables automatic
 * formatting when mounting fails. On the physical ESP32-S3-LCD-1.47 used for
 * this project, the independently verified configuration is 4-bit SD_MMC.
 *
 * This adapted copy therefore uses:
 *
 *   SD_MMC.begin("/sdcard", false)
 *
 * which keeps the full D0-D3 bus and does not auto-format the card on failure.
 * The function below only records mount state and card size for the existing
 * LVGL information panel; it is not presented as a complete SD-card diagnostic.
 ******************************************************************************
 */

#include "SD_Card.h"

bool SDCard_Flag;
bool SDCard_Finish;

uint16_t SDCard_Size;
uint16_t Flash_Size;

void SD_Init()
{
  SDCard_Flag = false;
  SDCard_Size = 0;

  if (!SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN, SD_D1_PIN, SD_D2_PIN, SD_D3_PIN)) {
    return;
  }

  // Hardware-verified board wiring: SD_MMC 4-bit, no automatic formatting.
  if (!SD_MMC.begin("/sdcard", false)) {
    return;
  }

  if (SD_MMC.cardType() == CARD_NONE) {
    return;
  }

  // The panel expects a compact capacity value in MiB.
  SDCard_Size = SD_MMC.totalBytes() / (1024 * 1024);
  SDCard_Flag = true;
}

bool File_Search(const char* directory, const char* fileName)
{
  File Path = SD_MMC.open(directory);
  if (!Path) {
    printf("Path: <%s> does not exist\r\n",directory);
    return false;
  }
  File file = Path.openNextFile();
  while (file) {
    if (strcmp(file.name(), fileName) == 0) {
      if (strcmp(directory, "/") == 0)
        printf("File '%s%s' found in root directory.\r\n",directory,fileName);
      else
        printf("File '%s/%s' found in root directory.\r\n",directory,fileName);
      Path.close();
      return true;
    }
    file = Path.openNextFile();
  }
  if (strcmp(directory, "/") == 0)
    printf("File '%s%s' not found in root directory.\r\n",directory,fileName);
  else
    printf("File '%s/%s' not found in root directory.\r\n",directory,fileName);
  Path.close();
  return false;
}

uint16_t Folder_retrieval(const char* directory, const char* fileExtension, char File_Name[][100],uint16_t maxFiles)
{
  File Path = SD_MMC.open(directory);
  if (!Path) {
    printf("Path: <%s> does not exist\r\n",directory);
    return false;
  }

  uint16_t fileCount = 0;
  char filePath[100];
  File file = Path.openNextFile();
  while (file && fileCount < maxFiles) {
    if (!file.isDirectory() && strstr(file.name(), fileExtension)) {
      strncpy(File_Name[fileCount], file.name(), sizeof(File_Name[fileCount]));
      if (strcmp(directory, "/") == 0) {
        snprintf(filePath, 100, "%s%s", directory, file.name());
      } else {
        snprintf(filePath, 100, "%s/%s", directory, file.name());
      }
      printf("File found: %s\r\n", filePath);
      fileCount++;
    }
    file = Path.openNextFile();
  }
  Path.close();
  if (fileCount > 0) {
    printf("%d <%s> files were retrieved\r\n",fileCount,fileExtension);
    return fileCount;
  } else {
    printf("No files with extension '%s' found in directory: %s\r\n", fileExtension, directory);
    return 0;
  }
}

void remove_file_extension(char *file_name)
{
  char *last_dot = strrchr(file_name, '.');
  if (last_dot != NULL) {
    *last_dot = '\0';
  }
}
