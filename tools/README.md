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
корня репозитория.

PowerShell:

```powershell
.\tools\export-kontakts-release.bat 0.1.0
```

CMD:

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

## SHA-256 release-файлов

После экспорта:

```powershell
cd firmware\KONTAKTS_ESP32_S3_LCD_1_47\release
..\..\..\tools\make-firmware-hashes.bat
```

[`make-firmware-hashes.bat`](make-firmware-hashes.bat) обрабатывает `.bin` именно
в **текущей рабочей папке** и создаёт там `SHA256SUMS.txt`.

## Быстрая прошивка merged BIN

### Самый простой способ — по номеру версии

Скрипт [`flash-merged.bat`](flash-merged.bat) теперь сам находит release BIN по
номеру версии и не зависит от текущей рабочей папки.

Из **корня репозитория** в PowerShell:

```powershell
.\tools\flash-merged.bat COM16 0.1.0
```

Из **папки firmware\KONTAKTS_ESP32_S3_LCD_1_47** в PowerShell:

```powershell
..\..\tools\flash-merged.bat COM16 0.1.0
```

Из **release-папки** в PowerShell:

```powershell
..\..\..\tools\flash-merged.bat COM16 0.1.0
```

В CMD начальный `.\` не обязателен.

### Прошивка с явным путём к merged BIN

Из корня репозитория в PowerShell:

```powershell
.\tools\flash-merged.bat COM16 .\firmware\KONTAKTS_ESP32_S3_LCD_1_47\release\KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
```

Merged image записывается с адреса `0x0`.

> [!NOTE]
> В PowerShell относительный исполняемый файл из текущего каталога обычно
> запускается через `./` или `.\`. Команда `tools\flash-merged.bat ...` корректна
> из корня в CMD, но в PowerShell используйте `.\tools\flash-merged.bat ...`.
> Если вы уже находитесь внутри `firmware\KONTAKTS_ESP32_S3_LCD_1_47`, путь
> `tools\...` неверен, потому что папка `tools` находится на два уровня выше.

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

Из корня репозитория в PowerShell:

```powershell
.\tools\export-kontakts-release.bat 0.1.0
cd firmware\KONTAKTS_ESP32_S3_LCD_1_47\release
..\..\..\tools\make-firmware-hashes.bat
cd ..\..\..
.\tools\flash-merged.bat COM16 0.1.0
```
