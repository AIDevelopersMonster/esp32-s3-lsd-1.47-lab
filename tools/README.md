# Tools

В этой папке находятся вспомогательные скрипты для backup, проверки eFuse,
экспорта release BIN, создания SHA-256 и быстрой прошивки ESP32-S3-LCD-1.47.

## Откуда запускать

Рекомендуемый вариант — открыть `cmd` или PowerShell **в корне репозитория**.
Тогда команды ниже можно копировать без изменения путей.

```text
lab-esp32-s3-lcd-1.47\
├── firmware\
├── tools\
├── docs\
└── platformio.ini
```

Каноническое имя нашей прошивки:

```text
KONTAKTS_ESP32_S3_LCD_1_47
```

## Экспорт двух BIN из Arduino IDE build

После успешной сборки Arduino IDE создаёт файлы в:

```text
firmware\KONTAKTS_ESP32_S3_LCD_1_47\build\esp32.esp32.esp32s3\
```

Для принудительного копирования двух нужных BIN в release-папку выполните из
корня репозитория:

```bat
tools\export-kontakts-release.bat 0.1.0
```

Скрипт [`export-kontakts-release.bat`](export-kontakts-release.bat):

- берёт свежие `KONTAKTS_ESP32_S3_LCD_1_47.ino.bin` и
  `KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin`;
- создаёт `firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\`, если её нет;
- копирует файлы с `copy /Y`, то есть **принудительно перезаписывает** старые
  release-копии;
- добавляет номер версии в имя.

Результат для версии `0.1.0`:

```text
firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\
    KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
    KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Эквивалентные ручные команды из корня репозитория:

```bat
if not exist firmware\KONTAKTS_ESP32_S3_LCD_1_47\release mkdir firmware\KONTAKTS_ESP32_S3_LCD_1_47\release
copy /Y firmware\KONTAKTS_ESP32_S3_LCD_1_47\build\esp32.esp32.esp32s3\KONTAKTS_ESP32_S3_LCD_1_47.ino.bin firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
copy /Y firmware\KONTAKTS_ESP32_S3_LCD_1_47\build\esp32.esp32.esp32s3\KONTAKTS_ESP32_S3_LCD_1_47.ino.merged.bin firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

## SHA-256 release-файлов

После экспорта:

```bat
cd firmware\KONTAKTS_ESP32_S3_LCD_1_47\release
..\..\..\tools\make-firmware-hashes.bat
```

[`make-firmware-hashes.bat`](make-firmware-hashes.bat) обрабатывает `.bin` именно
в **текущей рабочей папке** и создаёт там `SHA256SUMS.txt`.

## Быстрая прошивка merged BIN

Из корня репозитория:

```bat
tools\flash-merged.bat COM7 firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

[`flash-merged.bat`](flash-merged.bat) записывает merged image с адреса `0x0`.

## Остальные скрипты

- [`backup-factory.ps1`](backup-factory.ps1) — полный read-only backup Flash;
- [`read-efuses.ps1`](read-efuses.ps1) — read-only снимок eFuse;
- [`export-kontakts-release.bat`](export-kontakts-release.bat) — принудительный
  экспорт двух release BIN из Arduino IDE build;
- [`flash-merged.bat`](flash-merged.bat) — быстрая запись готового merged BIN;
- [`make-firmware-hashes.bat`](make-firmware-hashes.bat) — SHA-256 для BIN в
  текущей рабочей папке.

Документация:

- [Firmware backup](../docs/firmware-backup.md)
- [Firmware restore](../docs/firmware-restore.md)
- [esptool](../docs/esptool.md)
- [Arduino IDE и BIN-файлы](../docs/arduino-ide.md)
- [KONTAKTS firmware](../firmware/KONTAKTS_ESP32_S3_LCD_1_47/README.md)
- [KONTAKTS release directory](../firmware/KONTAKTS_ESP32_S3_LCD_1_47/release/README.md)

## Короткий рабочий цикл

Из корня репозитория:

```bat
tools\export-kontakts-release.bat 0.1.0
cd firmware\KONTAKTS_ESP32_S3_LCD_1_47\release
..\..\..\tools\make-firmware-hashes.bat
```

После этого для прошивки merged image:

```bat
cd /d ..\..\..
tools\flash-merged.bat COM7 firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```
