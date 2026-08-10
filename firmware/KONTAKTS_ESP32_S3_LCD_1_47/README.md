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

## Что хранить, а что не хранить

**Не нужно переносить в репозиторий весь каталог `build/`, созданный Arduino
IDE.** Это воспроизводимые промежуточные файлы, среди которых есть большие
`.elf`, `.map`, `sdkconfig` и другие служебные результаты сборки.

Для готовой версии прошивки достаточно сохранить два BIN-файла:

```text
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Первый — только application image. Второй — объединённый 16 MiB образ для
однофайловой прошивки с адреса `0x0`.

Дополнительно для публикации рекомендуется создать:

```text
SHA256SUMS.txt
```

То есть минимальный release-набор — **2 BIN + SHA256SUMS.txt**.

> [!NOTE]
> В корневом `.gitignore` проекта BIN-файлы намеренно игнорируются. Это удобно:
> автоматически созданные binaries не засоряют историю Git. Для распространения
> готовых версий лучше прикладывать два BIN и `SHA256SUMS.txt` к **GitHub Release**,
> а не коммитить новый 16 MiB merged image в Git при каждой версии.

## Какие два файла брать из Arduino IDE

Из фактического каталога Arduino IDE:

```text
build/esp32.esp32.esp32s3/
```

нужны эти два свежих результата сборки:

```text
KONTAKTS_ESP32_S3_LCD_1_47.ino.bin
KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin
```

Для release-копий убираем `.ino` из имени и добавляем номер версии:

```text
KONTAKTS_ESP32_S3_LCD_1_47.ino.bin
    -> KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin

KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin
    -> KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Исходные имена Arduino IDE можно не менять внутри `build/`; переименовываются
только release-копии.

## Файлы, создаваемые Arduino IDE

Ниже перечислены файлы, реально наблюдавшиеся в сборке
`KONTAKTS_ESP32_S3_LCD_1_47 v0.1.0`.

| Файл | Назначение | Нужен в release? |
|---|---|---|
| `boot_app0.bin` | Служебный boot/OTA image Arduino-ESP32. В данной сборке `flash_args` записывает его по адресу `0xE000`. | Нет отдельно, уже включён в merged BIN |
| `build.options.json` | Параметры конкретной сборки Arduino IDE; полезен для диагностики воспроизводимости. | Обычно нет |
| `flash_args` | Точные параметры и адреса, которые Arduino/esptool используют для записи этой сборки. | Нет, но его содержание документировано ниже |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.bin` | Основное приложение KONTAKTS. В этой сборке записывается по адресу `0x10000`. | **Да** |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.bootloader.bin` | Bootloader ESP32-S3 для этой конфигурации сборки. | Нет отдельно, включён в merged BIN |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.elf` | ELF с кодом, символами и секциями. Нужен для отладки, symbol lookup и анализа crash/backtrace. | Нет |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.map` | Linker map: карта размещения функций, данных и секций в памяти. | Нет |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin` | Объединённый flash image: bootloader + partitions + boot_app0 + application в правильных offsets. В наблюдавшейся сборке имеет размер ровно 16 MiB. | **Да, основной файл для простой прошивки** |
| `KONTAKTS_ESP32_S3_LCD_1_47.ino.partitions.bin` | Бинарная таблица разделов ESP32-S3. В этой сборке записывается по адресу `0x8000`. | Нет отдельно, включена в merged BIN |
| `partitions.csv` | Текстовое описание partition table, из которого формируется `partitions.bin`. | Обычно нет |
| `sdkconfig` | Автоматически сформированная конфигурация ESP32/Arduino build environment. | Нет |

### Файлы с `_flashed` в имени

В рабочем каталоге также наблюдались:

```text
KONTAKTS_ESP32_S3_LCD_1_47.ino.bootloader_flashed.bin
KONTAKTS_ESP32_S3_LCD_1_47.ino.partitions_flashed.bin
KONTAKTS_ESP32_S3_LCD_1_47.ino_flashed.bin
```

Они имеют более ранние timestamps, чем текущая свежая сборка, и **не входят в
актуальный `flash_args` Arduino IDE**. Поэтому они не считаются обязательными
release-артефактами и не должны подменять свежие `.ino.bin`,
`.ino.partitions.bin` или `.ino.bootloader.bin` без отдельно зафиксированного
происхождения. Для обычного release их не переносим.

## Точные flash offsets этой сборки

Фактический `flash_args`, полученный из Arduino IDE, содержит:

```text
--flash-mode dio --flash-freq 80m --flash-size 16MB
0x0     KONTAKTS_ESP32_S3_LCD_1_47.ino.bootloader.bin
0x8000  KONTAKTS_ESP32_S3_LCD_1_47.ino.partitions.bin
0xe000  boot_app0.bin
0x10000 KONTAKTS_ESP32_S3_LCD_1_47.ino.bin
```

Таким образом, для **именно этой аппаратно проверяемой сборки** адрес
application image не предполагается, а известен точно:

```text
application offset = 0x10000
```

Также важно, что данный `flash_args` сообщает:

```text
flash mode = DIO
flash frequency = 80 MHz
flash size = 16 MB
```

Эти значения следует считать фактическими параметрами данной Arduino IDE
сборки при воспроизведении или диагностике.

## Два способа прошивки

### 1. Рекомендуемый простой способ: merged BIN

Для чистой полной установки используйте:

```text
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Запись начинается с `0x0`:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x0 KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Или из корня репозитория:

```bat
tools\flash-merged.bat COM7 path\to\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

> [!WARNING]
> Наблюдавшийся merged BIN имеет полный размер Flash — **16 MiB**. Его следует
> рассматривать как полный установочный image: запись такого файла может
> заменить/очистить содержимое других flash-областей, включая пользовательские
> данные в разделах, которые в image заполнены пустыми значениями. Для обновления
> только приложения используйте application BIN.

### 2. Только application BIN

Если bootloader и partition table уже соответствуют этой версии сборки, можно
записать только приложение по адресу, подтверждённому `flash_args`:

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 921600 write-flash 0x10000 KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
```

Этот способ быстрее и не переписывает bootloader/partition table. Его следует
использовать только когда layout Flash заведомо совместим.

## SHA-256

Для release обязательно создавайте SHA-256 для обоих BIN.

Из каталога, где лежат release BIN:

```bat
path\to\repo\tools\make-firmware-hashes.bat
```

Скрипт создаёт:

```text
SHA256SUMS.txt
```

Документация инструментов:

[`../../tools/README.md`](../../tools/README.md)

## Рекомендуемая структура работы

Исходник и документацию держим в Git:

```text
firmware/
└── KONTAKTS_ESP32_S3_LCD_1_47/
    ├── KONTAKTS_ESP32_S3_LCD_1_47.ino
    └── README.md
```

Arduino IDE может локально создавать внутри sketch-папки:

```text
build/
└── esp32.esp32.esp32s3/
    └── ...generated files...
```

Эту build-папку **не переносим и не публикуем как часть исходников**. При новой
сборке она создаётся заново.

Для выпуска версии из неё копируются только:

```text
KONTAKTS_ESP32_S3_LCD_1_47.ino.bin
KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin
```

после чего release-копиям присваиваются канонические имена с версией и для них
генерируется `SHA256SUMS.txt`.

## Что делает v0.1.0

Первая версия намеренно компактная и детерминированная:

- выводит информационный экран KONTAKTS на встроенный ST7789;
- показывает ESP32-S3, Flash, PSRAM, display и состояние microSD;
- монтирует microSD через аппаратно проверенный 4-bit SD_MMC;
- инициализирует Wi-Fi, фиксирует успех и сразу выключает Wi-Fi;
- инициализирует BLE, фиксирует успех и деинициализирует BLE;
- печатает однократный boot report через `printf()`;
- не запускает Wi-Fi/BLE scan и фоновые diagnostic tasks;
- после `setup()` оставляет статический экран и слабую зелёную индикацию RGB.

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

## Build environment

Для текущей линии используется Arduino-ESP32 3.x. Фактический `flash_args`
наблюдавшейся сборки фиксирует DIO / 80 MHz / 16 MB; точные IDE options следует
сохранять вместе с build provenance при каждом релизе.

Основной sketch:

[`KONTAKTS_ESP32_S3_LCD_1_47.ino`](KONTAKTS_ESP32_S3_LCD_1_47.ino)

Полная инструкция Arduino IDE:

[`../../docs/arduino-ide.md`](../../docs/arduino-ide.md)

## Factory recovery

Заводской backup и KONTAKTS merged image — разные объекты:

- factory backup — полный raw read 16 MiB с физической платы до замены firmware;
- KONTAKTS merged image — воспроизводимый установочный image, собранный из нашего
  исходного кода.

Документация:

- [Firmware backup](../../docs/firmware-backup.md)
- [Firmware restore](../../docs/firmware-restore.md)

## Verification status

На реальной плате уже проверены используемые firmware аппаратные пути:

```text
ST7789 display path     VERIFIED
16 MB Flash             VERIFIED
8 MB PSRAM              VERIFIED
SD_MMC 4-bit            VERIFIED
Wi-Fi initialization    VERIFIED
BLE initialization      VERIFIED
```

Объединённая **KONTAKTS_ESP32_S3_LCD_1_47 v0.1.0** также запущена и наблюдалась
на физической плате. Текущий статус линии — **HARDWARE-VERIFIED**.
