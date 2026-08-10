/*
 * Vendor-derived source from the Waveshare ESP32-S3-LCD-1.47 LCD_Image demo.
 * Original top-level sketch: Copyright (c) 2024 Waveshare, MIT License.
 * See LCD_Image.ino and LICENSE-MIT.txt for attribution and license text.
 *
 * Local compatibility change, hardware-verified with Arduino-ESP32 3.3.11:
 *   - vendor code used SD_MMC.begin("/sdcard", true, true)
 *   - this adapted copy uses SD_MMC.begin("/sdcard", false)
 *
 * In the current Arduino-ESP32 API, false selects 4-bit SD_MMC mode. The same
 * 4-bit wiring was independently verified by this project on the physical
 * Waveshare ESP32-S3-LCD-1.47. The adapted call also avoids automatic
 * format-on-mount-failure behavior.
 *
 * The downloaded vendor example did not expose a readable root directory in
 * the tested environment until this change was made. This may reflect
 * dependency/API differences from the original vendor development environment.
 */
#include "SD_Card.h"

bool SDCard_Flag;
bool SDCard_Finish;

uint16_t SDCard_Size;
uint16_t Flash_Size;

void SD_Init() {
  // SD MMC pin assignment for the built-in microSD/TF slot.
  if(!SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN, SD_D1_PIN, SD_D2_PIN, SD_D3_PIN)){
    printf("SD MMC: Pin change failed!\r\n");
    return;
  }

  // Local compatibility fix: use the hardware-verified 4-bit configuration.
  // No automatic formatting is requested if mounting fails.
  if (SD_MMC.begin("/sdcard", false)) {                              
    printf("SD card initialization successful!\r\n");
  } else {
    printf("SD card initialization failed!\r\n");
  }
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    printf("No SD card attached\r\n");
    return;
  }
  else{
    printf("SD Card Type: ");
    if(cardType == CARD_MMC){
      printf("MMC\r\n");
    } else if(cardType == CARD_SD){
      printf("SDSC\r\n");
    } else if(cardType == CARD_SDHC){
      printf("SDHC\r\n");
    } else {
      printf("UNKNOWN\r\n");
    }
    uint64_t totalBytes = SD_MMC.totalBytes();
    uint64_t usedBytes = SD_MMC.usedBytes();
    SDCard_Size = totalBytes/(1024*1024);
    printf("Total space: %llu\n", totalBytes);
    printf("Used space: %llu\n", usedBytes);
    printf("Free space: %llu\n", totalBytes - usedBytes);
  }
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

void remove_file_extension(char *file_name) {
  char *last_dot = strrchr(file_name, '.');
  if (last_dot != NULL) {
    *last_dot = '\0'; 
  }
}
void Flash_test()
{
  // Note: the original vendor source labels this output "RAM Test" even though
  // it queries Flash size. The wording is intentionally retained as provenance.
  printf("/********** RAM Test**********/\r\n");
  // Get Flash size
  uint32_t flashSize = ESP.getFlashChipSize();
  Flash_Size = flashSize/1024/1024;
  printf("Flash size: %d MB \r\n", flashSize/1024/1024);

  printf("/******* RAM Test Over********/\r\n\r\n");
}