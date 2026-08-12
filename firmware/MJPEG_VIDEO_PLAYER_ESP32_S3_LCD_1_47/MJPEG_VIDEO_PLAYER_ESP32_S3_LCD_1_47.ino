/*
  MJPEG Video Player for Waveshare ESP32-S3-LCD-1.47

  Target hardware only:
    Waveshare ESP32-S3-LCD-1.47
    ST7789 172x320 LCD
    16 MB Flash / 8 MB OPI PSRAM
    onboard microSD slot in SD_MMC 4-bit mode
    BOOT button on GPIO0

  Behavior:
    - scans /mjpeg on the microSD card
    - plays .mjpeg files continuously
    - BOOT = NEXT (short press skips the current clip)
    - Serial output is diagnostic only; playback does not require Serial Monitor

  Port/adaptation based on:
    thelastoutpostworkshop/ESP32-C6-LCD-1.47_video_player
    tested upstream commit: 6085a7e339de19356d399e8e2b48a0af152ce7a5

  Upstream project is MIT licensed. See THIRD_PARTY_LICENSES.md.
  This repository's own changes are covered by the repository MIT license.
*/

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <SD_MMC.h>
#include "MjpegClass.h"

#define GFX_BRIGHTNESS 255
#define GFX_SPEED 80000000UL

// Waveshare ESP32-S3-LCD-1.47 onboard hardware
static constexpr int BTN_A = 0;
static constexpr int GFX_BL = 48;

static constexpr int SD_CLK = 14;
static constexpr int SD_CMD = 15;
static constexpr int SD_D0 = 16;
static constexpr int SD_D1 = 18;
static constexpr int SD_D2 = 17;
static constexpr int SD_D3 = 21;

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    41 /* DC */, 42 /* CS */, 40 /* SCK */, 45 /* MOSI */,
    GFX_NOT_DEFINED /* MISO */);

Arduino_ST7789 *gfx = new Arduino_ST7789(
    bus,
    39 /* RST */,
    0 /* rotation */,
    true /* IPS */,
    172,
    320,
    34,
    0,
    34,
    0);

const char *MJPEG_FOLDER = "/mjpeg";

#define MAX_FILES 20
String mjpegFileList[MAX_FILES];
uint32_t mjpegFileSizes[MAX_FILES] = {0};
int mjpegCount = 0;
static int currentMjpegIndex = 0;

MjpegClass mjpeg;
int total_frames;
unsigned long total_read_video;
unsigned long total_decode_video;
unsigned long total_show_video;
unsigned long start_ms, curr_ms;
long output_buf_size, estimateBufferSize;
uint8_t *mjpeg_buf;
uint16_t *output_buf;

void setDisplayBrightness();
void loadMjpegFilesList();
void playSelectedMjpeg(int mjpegIndex);
void mjpegPlayFromSDCard(const char *mjpegFilename);
int jpegDrawCallback(JPEGDRAW *pDraw);

void setup()
{
    Serial.begin(115200);
    Serial.setTxTimeoutMs(0); // Do not stop playback when USB Serial Monitor is closed.
    delay(2000);

    if (!gfx->begin(GFX_SPEED))
    {
        Serial.println("Display initialization failed!");
        while (true)
        {
            delay(1000);
        }
    }

    gfx->setRotation(0);
    gfx->fillScreen(RGB565_BLACK);
    setDisplayBrightness();

    // Waveshare ESP32-S3-LCD-1.47 onboard microSD: SD_MMC 4-bit.
    if (!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3))
    {
        Serial.println("ERROR: SD_MMC.setPins failed!");
        while (true)
        {
            delay(1000);
        }
    }

    if (!SD_MMC.begin("/sdcard", false, false, SDMMC_FREQ_DEFAULT, 5))
    {
        Serial.println("ERROR: SD_MMC mount failed!");
        while (true)
        {
            delay(1000);
        }
    }

    Serial.printf("SD_MMC mounted: %llu MB\n",
                  SD_MMC.cardSize() / (1024ULL * 1024ULL));

    // Kept from the upstream decoder path.
    output_buf_size = gfx->width() * 4 * 2;
    output_buf = (uint16_t *)heap_caps_aligned_alloc(
        16,
        output_buf_size * sizeof(uint16_t),
        MALLOC_CAP_DMA);

    if (!output_buf)
    {
        Serial.println("output_buf aligned_alloc failed!");
        while (true)
        {
            delay(1000);
        }
    }

    // The original C6 sketch estimated only ~22 KiB for a 172x320 display.
    // Some MJPEG frames exceed that size. Use a large PSRAM-backed frame buffer.
    estimateBufferSize = 256 * 1024;
    mjpeg_buf = (uint8_t *)heap_caps_malloc(
        estimateBufferSize,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (!mjpeg_buf)
    {
        Serial.println("mjpeg_buf PSRAM allocation failed!");
        while (true)
        {
            delay(1000);
        }
    }

    Serial.printf("MJPEG buffer: %ld bytes\n", estimateBufferSize);

    loadMjpegFilesList();

    // BOOT/GPIO0 is active LOW. Polling is intentionally used instead of ISR.
    pinMode(BTN_A, INPUT_PULLUP);
}

void setDisplayBrightness()
{
    ledcAttachChannel(GFX_BL, 1000, 8, 1);
    ledcWrite(GFX_BL, GFX_BRIGHTNESS);
}

void loop()
{
    if (mjpegCount <= 0)
    {
        delay(1000);
        return;
    }

    playSelectedMjpeg(currentMjpegIndex);

    currentMjpegIndex++;
    if (currentMjpegIndex >= mjpegCount)
    {
        currentMjpegIndex = 0;
    }
}

void playSelectedMjpeg(int mjpegIndex)
{
    String fullPath = String(MJPEG_FOLDER) + "/" + mjpegFileList[mjpegIndex];
    char mjpegFilename[128];
    fullPath.toCharArray(mjpegFilename, sizeof(mjpegFilename));

    Serial.printf("Playing %s\n", mjpegFilename);
    mjpegPlayFromSDCard(mjpegFilename);
}

int jpegDrawCallback(JPEGDRAW *pDraw)
{
    unsigned long s = millis();
    gfx->draw16bitBeRGBBitmap(
        pDraw->x,
        pDraw->y,
        pDraw->pPixels,
        pDraw->iWidth,
        pDraw->iHeight);
    total_show_video += millis() - s;
    return 1;
}

void mjpegPlayFromSDCard(const char *mjpegFilename)
{
    File mjpegFile = SD_MMC.open(mjpegFilename, "r");

    if (!mjpegFile || mjpegFile.isDirectory())
    {
        Serial.printf("ERROR: Failed to open %s for reading\n", mjpegFilename);
        return;
    }

    Serial.println(F("MJPEG start"));
    gfx->fillScreen(RGB565_BLACK);

    start_ms = millis();
    curr_ms = millis();
    total_frames = 0;
    total_read_video = 0;
    total_decode_video = 0;
    total_show_video = 0;

    if (!mjpeg.setup(
            &mjpegFile,
            mjpeg_buf,
            jpegDrawCallback,
            true /* useBigEndian */,
            0 /* x */,
            0 /* y */,
            gfx->width() /* widthLimit */,
            gfx->height() /* heightLimit */))
    {
        Serial.println("ERROR: MJPEG decoder setup failed");
        mjpegFile.close();
        return;
    }

    while (mjpegFile.available())
    {
        // BOOT = NEXT. Poll between frames; no hardware interrupt is required.
        if (digitalRead(BTN_A) == LOW)
        {
            delay(25); // debounce
            if (digitalRead(BTN_A) == LOW)
            {
                while (digitalRead(BTN_A) == LOW)
                {
                    delay(5);
                }
                Serial.println("BOOT: next video");
                break;
            }
        }

        if (!mjpeg.readMjpegBuf())
        {
            break;
        }

        total_read_video += millis() - curr_ms;
        curr_ms = millis();

        mjpeg.drawJpg();
        total_decode_video += millis() - curr_ms;

        curr_ms = millis();
        total_frames++;
    }

    int time_used = millis() - start_ms;
    Serial.println(F("MJPEG end"));
    mjpegFile.close();

    if (time_used <= 0)
    {
        time_used = 1;
    }

    float fps = 1000.0f * total_frames / time_used;
    if (total_decode_video >= total_show_video)
    {
        total_decode_video -= total_show_video;
    }
    else
    {
        total_decode_video = 0;
    }

    Serial.printf("Total frames: %d\n", total_frames);
    Serial.printf("Time used: %d ms\n", time_used);
    Serial.printf("Average FPS: %0.1f\n", fps);
    Serial.printf("Read MJPEG: %lu ms (%0.1f %%)\n",
                  total_read_video,
                  100.0 * total_read_video / time_used);
    Serial.printf("Decode video: %lu ms (%0.1f %%)\n",
                  total_decode_video,
                  100.0 * total_decode_video / time_used);
    Serial.printf("Show video: %lu ms (%0.1f %%)\n",
                  total_show_video,
                  100.0 * total_show_video / time_used);
}

void loadMjpegFilesList()
{
    File mjpegDir = SD_MMC.open(MJPEG_FOLDER);

    if (!mjpegDir || !mjpegDir.isDirectory())
    {
        Serial.printf("Failed to open %s folder\n", MJPEG_FOLDER);
        while (true)
        {
            delay(1000);
        }
    }

    mjpegCount = 0;

    while (mjpegCount < MAX_FILES)
    {
        File file = mjpegDir.openNextFile();
        if (!file)
        {
            break;
        }

        if (!file.isDirectory())
        {
            String name = file.name();
            if (name.endsWith(".mjpeg"))
            {
                mjpegFileList[mjpegCount] = name;
                mjpegFileSizes[mjpegCount] = file.size();
                mjpegCount++;
            }
        }

        file.close();
    }

    mjpegDir.close();

    Serial.printf("%d mjpeg files read\n", mjpegCount);
    for (int i = 0; i < mjpegCount; i++)
    {
        Serial.printf("File %d: %s, Size: %lu bytes\n",
                      i,
                      mjpegFileList[i].c_str(),
                      mjpegFileSizes[i]);
    }
}
