# Tools

В этой папке находятся вспомогательные скрипты для backup, проверки eFuse,
создания SHA-256 и быстрой прошивки ESP32-S3-LCD-1.47.

## Откуда запускать

Рекомендуемый вариант — открыть `cmd` или PowerShell **в корне репозитория**.
Тогда все команды из документации можно копировать без изменения путей.

Пример корня репозитория:

```text
lab-esp32-s3-lcd-1.47\
├── firmware\
├── tools\
├── docs\
├── src\
└── platformio.ini
```

Каноническое имя нашей прошивки и release-файлов:

```text
KONTAKTS_ESP32_S3_LCD_1_47
```

Для release используются:

```text
KONTAKTS_ESP32_S3_LCD_1_47.bin
KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
SHA256SUMS.txt
```

### Быстрая прошивка merged BIN

Из корня репозитория:

```bat
tools\flash-merged.bat COM7 firmware\KONTAKTS_ESP32_S3_LCD_1_47\KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

Сам [`flash-merged.bat`](flash-merged.bat) можно запускать и из папки `tools`,
но тогда путь к BIN нужно задавать относительно `tools` либо использовать полный путь.
Например:

```bat
flash-merged.bat COM7 ..\firmware\KONTAKTS_ESP32_S3_LCD_1_47\KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

Merged-образ всегда записывается с адреса `0x0`.

### Создание SHA-256 для BIN-файлов

[`make-firmware-hashes.bat`](make-firmware-hashes.bat) обрабатывает `.bin` файлы
**в текущей рабочей папке**, а не автоматически в папке самого BAT-файла.

Если release BIN лежат в каталоге прошивки, из корня выполните:

```bat
cd firmware\KONTAKTS_ESP32_S3_LCD_1_47
..\..\tools\make-firmware-hashes.bat
```

В результате рядом с BIN-файлами будет создан:

```text
SHA256SUMS.txt
```

Не рекомендуется запускать `make-firmware-hashes.bat` из `tools`, если сами BIN
не лежат в `tools`: в этом случае скрипт будет искать `.bin` именно в папке
`tools`.

## Остальные скрипты

- [`backup-factory.ps1`](backup-factory.ps1) — полный read-only backup Flash;
- [`read-efuses.ps1`](read-efuses.ps1) — read-only снимок eFuse;
- [`flash-merged.bat`](flash-merged.bat) — быстрая запись готового `.merged.bin`;
- [`make-firmware-hashes.bat`](make-firmware-hashes.bat) — создание SHA-256 для
  BIN-файлов текущей рабочей папки.

Перед первым использованием backup/restore и eFuse-инструментов прочитайте
основные инструкции:

- [Firmware backup](../docs/firmware-backup.md)
- [Firmware restore](../docs/firmware-restore.md)
- [esptool](../docs/esptool.md)
- [Arduino IDE и BIN-файлы](../docs/arduino-ide.md)
- [KONTAKTS firmware](../firmware/KONTAKTS_ESP32_S3_LCD_1_47/README.md)

## Практический короткий цикл

Для обычной работы с уже собранной KONTAKTS firmware удобно хранить release BIN
рядом с исходным sketch в `firmware\KONTAKTS_ESP32_S3_LCD_1_47\`.

Из корня репозитория прошивка:

```bat
tools\flash-merged.bat COM7 firmware\KONTAKTS_ESP32_S3_LCD_1_47\KONTAKTS_ESP32_S3_LCD_1_47.merged.bin
```

Обновление хешей:

```bat
cd firmware\KONTAKTS_ESP32_S3_LCD_1_47
..\..\tools\make-firmware-hashes.bat
```

Так имена sketch, release BIN и документации совпадают, а `tools` остаётся
отдельной папкой инструментов.
