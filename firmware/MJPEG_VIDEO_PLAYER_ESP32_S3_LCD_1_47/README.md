# MJPEG Video Player — Waveshare ESP32-S3-LCD-1.47

Status: `HARDWARE_VERIFIED_PORT`

Canonical identifier:

```text
MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47
```

Это специализированный порт MJPEG-плеера именно для **Waveshare ESP32-S3-LCD-1.47**. Мы намеренно убрали универсальную C6/S3 архитектуру и оставили один понятный аппаратный профиль: ST7789 172x320, встроенный слот microSD через SD_MMC 4-bit, 8 MB OPI PSRAM и BOOT/GPIO0.

## Что получилось

На реальной плате проверено:

- автоматический поиск `.mjpeg` в `/mjpeg` на microSD;
- непрерывное последовательное воспроизведение и возврат к первому ролику;
- **BOOT = NEXT** — короткое нажатие прерывает текущий ролик и запускает следующий;
- BOOT обрабатывается polling-ом, без аппаратного interrupt/ISR;
- встроенный слот microSD работает в штатном для этой платы **SD_MMC 4-bit** режиме;
- MJPEG frame buffer перенесён в **PSRAM** и увеличен до **256 KiB**;
- Serial используется только для диагностики и не должен останавливать видео при закрытии Serial Monitor;
- на исправной microSD успешно воспроизводился полный тестовый набор из 20 upstream `.mjpeg` файлов.

Примеры измерений на физической плате:

```text
alien_eye.mjpeg: 120 frames, ~32.2 FPS, ~3.73 s
winston.mjpeg:   103 frames, ~21.7 FPS, ~4.75 s
```

## Почему этот порт отличается от исходного C6-проекта

Исходный проект сделан для ESP32-C6-LCD-1.47 и использует SPI-доступ к SD. У Waveshare ESP32-S3-LCD-1.47 встроенный слот подключён иначе, поэтому прямой перенос SPI-вызовов не работает.

Основные изменения:

1. `SD.h` / SPI SD заменены на `SD_MMC.h` и 4-bit SDMMC.
2. Используется распиновка Waveshare S3: CLK=14, CMD=15, D0=16, D1=18, D2=17, D3=21.
3. Аппаратная конфигурация ST7789 встроена непосредственно в sketch — C6-профили и multi-board abstraction здесь не нужны.
4. MJPEG buffer увеличен с исходной оценки около 22 KiB для 172x320 до 256 KiB и размещается в OPI PSRAM.
5. Убрана ISR-логика кнопки. BOOT опрашивается между кадрами; одно нажатие означает NEXT.
6. Сохранён автоматический demo-loop: без нажатий видео идут одно за другим.
7. Для Hardware CDC задан неблокирующий TX timeout, чтобы закрытие Serial Monitor не останавливало воспроизведение.

## Аппаратная распиновка

### ST7789

| Signal | GPIO |
|---|---:|
| MOSI | 45 |
| SCLK | 40 |
| CS | 42 |
| DC | 41 |
| RESET | 39 |
| Backlight | 48 |

### microSD / SD_MMC 4-bit

| Signal | GPIO |
|---|---:|
| CLK | 14 |
| CMD | 15 |
| D0 | 16 |
| D1 | 18 |
| D2 | 17 |
| D3 | 21 |

BOOT button: GPIO0, active LOW.

## Что установить в Arduino IDE

Board package:

```text
esp32 by Espressif Systems 3.3.11
```

Libraries:

- **GFX Library for Arduino** (`Arduino_GFX_Library`);
- **JPEGDEC** by bitbank2.

`SD_MMC` входит в Arduino-ESP32.

## Проверенные Arduino IDE settings

| Option | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | Enabled |
| CPU Frequency | 240MHz (WiFi) |
| Core Debug Level | None |
| USB DFU On Boot | Disabled |
| Erase All Flash Before Sketch Upload | Disabled |
| Events Run On | Core 1 |
| Flash Mode | QIO 80MHz |
| Flash Size | 16MB (128Mb) |
| JTAG Adapter | Disabled |
| Arduino Runs On | Core 1 |
| USB Firmware MSC On Boot | Disabled |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| PSRAM | OPI PSRAM |
| Upload Mode | UART0 / Hardware CDC |
| Upload Speed | 921600 |
| USB Mode | Hardware CDC and JTAG |
| Zigbee Mode | Disabled |

Serial Monitor: `115200`.

Полная общая инструкция проекта: [`../../docs/arduino-ide.md`](../../docs/arduino-ide.md).

## Подготовка microSD

Создайте на карте каталог:

```text
/mjpeg
```

и положите туда файлы:

```text
/mjpeg/demo1.mjpeg
/mjpeg/demo2.mjpeg
...
```

В текущей версии индексируется до 20 файлов (`MAX_FILES`).

### Готовые тестовые MJPEG

Мы не копируем сторонние demo-файлы в этот репозиторий. Их можно взять непосредственно из upstream:

https://github.com/thelastoutpostworkshop/ESP32-C6-LCD-1.47_video_player/tree/main/SD%20Content/mjpeg

### Конвертация собственного видео

Upstream Video Conversion Studio:

https://thelastoutpostworkshop.github.io/video_conversion/

Исходный tutorial:

https://youtu.be/JqQEG0eipic

## Важный практический урок: проверяйте microSD

В ходе портирования плохая microSD оказалась способна создать очень убедительную ложную картину программной ошибки: карта нормально монтировалась, каталог и размеры файлов читались, несколько роликов работали, а другие немедленно завершались с `0 frames`.

После замены карты весь набор заработал.

Поэтому если часть MJPEG неожиданно даёт `0 frames`, прежде чем переписывать decoder:

- проверьте файлы на ПК;
- попробуйте заведомо исправную карту;
- выполните полное чтение/проверку карты, а не только mount/list-directory.

`SD_MMC.begin()` подтверждает монтирование файловой системы, но сам по себе не доказывает надёжность чтения каждого сектора карты.

## Управление

Обычный режим не требует кнопки:

```text
video 0 -> video 1 -> ... -> last -> video 0 -> ...
```

Короткое нажатие BOOT во время ролика:

```text
current video -> NEXT -> following video -> AUTO continues
```

Кнопка не переключает отдельный manual mode: после NEXT автоматическая демонстрация продолжается.

## Сборка из исходника

Откройте:

```text
MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47.ino
```

Выберите настройки выше и нажмите **Verify** / **Upload**.

Для получения release-образов используйте:

**Sketch -> Export Compiled Binary**

Arduino IDE создаёт в локальном `build/esp32.esp32.esp32s3/` два нужных файла:

```text
MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47.ino.bin
MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47.ino.merged.bin
```

После свежего Export можно выполнить из корня репозитория:

```bat
tools\export-mjpeg-player-release.bat 0.1.0
```

Скрипт перенесёт application и merged image в `release/` и добавит номер версии.

## Два BIN: какой нужен

### 1. Merged / полный установочный образ

Рекомендуется человеку, который просто хочет запустить плеер без Arduino IDE.

Файл:

```text
MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Записывается с адреса `0x0`:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x0 firmware\MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47\release\MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Или универсальным BAT из репозитория:

```bat
tools\flash-merged.bat COM7 firmware\MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47\release\MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

> [!WARNING]
> Merged image содержит не только application. Он предназначен для полной установки совместимого flash layout. Если во Flash есть нужные пользователю данные других разделов, сначала сделайте backup.

### 2. Application-only / частичный образ

Файл:

```text
MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47_v0.1.0.bin
```

Для проверенной Arduino partition scheme application размещается по `0x10000`:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x10000 firmware\MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47\release\MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47_v0.1.0.bin
```

Application-only image используйте только если bootloader и partition table уже совместимы. Для неизвестной платы/старой разметки безопаснее использовать согласованный merged image после backup.

## SHA-256

После экспорта release:

```bat
cd firmware\MJPEG_VIDEO_PLAYER_ESP32_S3_LCD_1_47\release
..\..\..\tools\make-firmware-hashes.bat
```

В release должен лежать `SHA256SUMS.txt` вместе с двумя BIN.

## Где это может пригодиться

Для такого маленького дисплея MJPEG-плеер оказался интересен не как «кинотеатр», а как автономный визуальный модуль. Возможные применения:

- мини-информационные и рекламные циклы;
- анимированные логотипы, заставки и boot/demo screens;
- стендовые демонстраторы устройств без подключения к ПК;
- короткие инструкции/подсказки рядом с оборудованием;
- индикация состояний сложнее обычной иконки;
- музейные/макетные/модельные мини-дисплеи;
- cosplay/props/панели приборов;
- анимированные элементы роботов и устройств;
- локальный media loop на выставочном образце;
- учебный пример потокового чтения SD + JPEG decode + LCD + PSRAM.

Особенно полезна архитектурная связка:

```text
microSD -> SD_MMC 4-bit -> compressed JPEG frame -> PSRAM -> JPEGDEC -> ST7789
```

Она показывает, что даже маленькая ESP32-S3-плата может быть самостоятельным визуальным терминалом без сети и внешнего компьютера.

## Ограничения текущей версии

- максимум 20 индексируемых файлов;
- только MJPEG/JPEG-frame stream, без звуковой дорожки;
- текущий upstream `MjpegClass` собирает полный compressed JPEG frame в RAM перед decode;
- 256 KiB PSRAM buffer намного надёжнее исходной маленькой оценки, но это не универсальная гарантия для произвольного огромного JPEG frame;
- порядок воспроизведения соответствует порядку, в котором файловая система возвращает записи каталога;
- эта ветка намеренно предназначена только для Waveshare ESP32-S3-LCD-1.47.

## Provenance / attribution

Исходная идея и значительная часть MJPEG decoder/player logic происходят из проекта:

https://github.com/thelastoutpostworkshop/ESP32-C6-LCD-1.47_video_player

Автор upstream: **thelastoutpostworkshop**. License: **MIT**. Использованный при портировании commit:

```text
6085a7e339de19356d399e8e2b48a0af152ce7a5
```

Полный upstream MIT notice сохранён в [`THIRD_PARTY_LICENSES.md`](THIRD_PARTY_LICENSES.md).

Наши изменения — аппаратная адаптация под Waveshare ESP32-S3-LCD-1.47, SD_MMC 4-bit, PSRAM buffer, Hardware CDC behavior и polling BOOT/NEXT — публикуются в рамках MIT-лицензии этого репозитория.
