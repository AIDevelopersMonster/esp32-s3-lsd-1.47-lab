# KONTAKTS_ESP32_S3_LCD_1_47 firmware

Собственная прошивка проекта для **Waveshare ESP32-S3-LCD-1.47 (USB-A variant)**.
Это независимо написанная прошивка KONTAKTS, а не заводская firmware Waveshare
и не переименованная копия vendor demo.

Текущая версия:

```text
0.1.0
```

Канонический идентификатор прошивки:

```text
KONTAKTS_ESP32_S3_LCD_1_47
```

Для исходного Arduino sketch, каталогов и release-файлов используем именно эту
underscore-форму. Человекочитаемое название платы остаётся
`ESP32-S3-LCD-1.47`.

## Видео

[Waveshare ESP32-S3-LCD-1.47 — прошивка KONTAKTS v0.1.0 через merged BIN](https://www.youtube.com/shorts/Dy9VAgI30Wk)

В ролике показана реальная прошивка release-образа KONTAKTS на физическую плату.

## Что хранить в Git

Arduino IDE создаёт локальный каталог:

```text
firmware/KONTAKTS_ESP32_S3_LCD_1_47/build/
```

В нём находятся воспроизводимые build-артефакты: `.elf`, `.map`, `sdkconfig`,
служебные BIN и другие промежуточные файлы. **Весь `build/` в Git не переносим.**
Корневой `.gitignore` теперь явно исключает все `**/build/`.

Для готового release сохраняем только два BIN:

```text
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

и контрольный файл:

```text
SHA256SUMS.txt
```

Для них используется отдельная папка:

```text
firmware/KONTAKTS_ESP32_S3_LCD_1_47/release/
```

В `.gitignore` сделано исключение, разрешающее хранить BIN именно из этой
release-папки, несмотря на общее правило `*.bin`.

## Какие два файла брать из Arduino IDE

После свежей сборки нужны:

```text
build/esp32.esp32.esp32s3/KONTAKTS_ESP32_S3_LCD_1_47.ino.bin
build/esp32.esp32.esp32s3/KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin
```

При экспорте release-копиям присваиваются имена с версией:

```text
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

## Автоматический принудительный экспорт BIN

Из корня репозитория:

```bat
tools\export-kontakts-release.bat 0.1.0
```

Скрипт [`../../tools/export-kontakts-release.bat`](../../tools/export-kontakts-release.bat):

- проверяет наличие двух свежих Arduino BIN;
- создаёт `release/`, если её ещё нет;
- копирует оба файла через `copy /Y`;
- **принудительно перезаписывает** старые release-файлы этой же версии;
- автоматически добавляет `v0.1.0` в имена.

Ручной эквивалент из корня репозитория:

```bat
if not exist firmware\KONTAKTS_ESP32_S3_LCD_1_47\release mkdir firmware\KONTAKTS_ESP32_S3_LCD_1_47\release
copy /Y firmware\KONTAKTS_ESP32_S3_LCD_1_47\build\esp32.esp32.esp32s3\KONTAKTS_ESP32_S3_LCD_1_47.ino.bin firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
copy /Y firmware\KONTAKTS_ESP32_S3_LCD_1_47\build\esp32.esp32.esp32s3\KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

## Файлы, создаваемые Arduino IDE

| Файл | Назначение | Нужен в release? |
|---|---|---|
| `boot_app0.bin` | Служебный boot/OTA image Arduino-ESP32. | Нет отдельно |
| `build.options.json` | Параметры конкретной сборки Arduino IDE. | Обычно нет |
| `flash_args` | Точные аргументы esptool/Arduino для этой сборки. | Нет, но полезен для диагностики |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.bin` | Основное application image. | **Да** |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.bootloader.bin` | Bootloader ESP32-S3. | Нет отдельно |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.elf` | ELF для отладки и анализа crash/backtrace. | Нет |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.map` | Linker map с размещением кода и данных. | Нет |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin` | Объединённый flash image. | **Да** |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.partitions.bin` | Бинарная partition table. | Нет отдельно |
| `partitions.csv` | Текстовая partition table. | Обычно нет |
| `sdkconfig` | Автоматически сформированная конфигурация сборки. | Нет |

### Файлы `_flashed.bin`

В рабочем каталоге также наблюдались:

```text
KONTAKTS_ESP32_S3_LCD_1_47.ino.bootloader_flashed.bin
KONTAKTS_ESP32_S3_LCD_1_47.ino.partitions_flashed.bin
KONTAKTS_ESP32_S3_LCD_1_47.ino_flashed.bin
```

Они имели более ранние timestamps, чем свежая сборка, и не считаются
обязательными release-артефактами. Их в release не переносим.

## Flash offsets

Переданный `flash_args` показывает следующие offsets:

```text
0x0     bootloader
0x8000  partitions
0xE000  boot_app0
0x10000 application
```

Поэтому для этой схемы application BIN записывается по:

```text
0x10000
```

### Важно: QIO/DIO нужно перепроверить

В настройках Arduino IDE для аппаратно проверенной сборки был выбран **QIO**.
При этом ранее переданный файл `flash_args` содержал строку с `--flash-mode dio`.
Это противоречие не следует трактовать как подтверждение DIO: файл мог относиться
к другой/предыдущей конфигурации либо Arduino core мог сформировать отдельный
upload-параметр.

До чистой пересборки и повторной проверки `flash_args` в документации считаем
целевой IDE-настройкой:

```text
Flash Mode: QIO 80 MHz
Flash Size: 16 MB
PSRAM: OPI PSRAM
```

А строку `dio` из старого `flash_args` считаем **неразрешённым build-provenance
расхождением**, а не новым подтверждённым параметром платы.

## Прошивка merged BIN

Из корня репозитория:

```bat
.\tools\flash-merged.bat COM7 0.1.0
```

Или напрямую:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x0 firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Merged image записывается с `0x0`.

> [!WARNING]
> Наблюдавшийся merged BIN имел размер ровно 16 MiB. Его следует считать полным
> установочным образом: он может заменить содержимое других flash-разделов.

## Прошивка только application BIN

Если bootloader и partition table уже совместимы:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x10000 firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
```

## SHA-256

После экспорта:

```bat
cd firmware\KONTAKTS_ESP32_S3_LCD_1_47\release
..\..\..\tools\make-firmware-hashes.bat
```

Будет создан:

```text
SHA256SUMS.txt
```

Подробно:

- [`../../tools/README.md`](../../tools/README.md)
- [`release/README.md`](release/README.md)
- [`../../docs/arduino-ide.md`](../../docs/arduino-ide.md)

## Что делает v0.1.0

Первая версия намеренно компактная и детерминированная:

- выводит информационный экран KONTAKTS на встроенный ST7789;
- показывает ESP32-S3, Flash, PSRAM, display и состояние microSD;
- монтирует microSD через аппаратно проверенный 4-bit SD_MMC;
- инициализирует Wi-Fi, фиксирует успех и сразу выключает Wi-Fi;
- инициализирует BLE, фиксирует успех и деинициализирует BLE;
- печатает однократный boot report через `printf()`;
- не запускает Wi-Fi/BLE scan и фоновые diagnostic tasks;
- после `setup()` оставляет статический экран и слабую зелёную RGB-индикацию.

## Hardware target

```text
Board       Waveshare ESP32-S3-LCD-1.47 (USB-A variant)
MCU         ESP32-S3
Flash       16 MB
PSRAM       8 MB OPI
Display     ST7789 IPS, 172 x 320
RGB LED     GPIO38
```

LCD:

```text
MOSI        GPIO45
SCLK        GPIO40
CS          GPIO42
DC          GPIO41
RESET       GPIO39
Backlight   GPIO48
```

microSD:

```text
CMD         GPIO15
CLK         GPIO14
D0          GPIO16
D1          GPIO18
D2          GPIO17
D3          GPIO21
```

## Verification status

На реальной плате проверены:

```text
ST7789 display path     VERIFIED
16 MB Flash             VERIFIED
8 MB PSRAM              VERIFIED
SD_MMC 4-bit            VERIFIED
Wi-Fi initialization    VERIFIED
BLE initialization      VERIFIED
```

Объединённая **KONTAKTS_ESP32_S3_LCD_1_47 v0.1.0** также запущена на физической
плате. Текущий статус линии — **HARDWARE-VERIFIED**.
