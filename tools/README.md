# Tools

В этой папке находятся вспомогательные скрипты для backup, проверки eFuse,
создания SHA-256 и быстрой прошивки ESP32-S3-LCD-1.47.

## Откуда запускать

Рекомендуемый вариант — открыть `cmd` или PowerShell **в корне репозитория**.
Тогда все команды из документации можно копировать без изменения путей.

Пример корня репозитория:

```text
lab-esp32-s3-lcd-1.47\
├── tools\
├── docs\
├── src\
└── platformio.ini
```

### Быстрая прошивка merged BIN

Из корня репозитория:

```bat
tools\flash-merged.bat COM7 path\to\KONTAKTS.merged.bin
```

Например, если BIN лежит в папке `firmware`:

```bat
tools\flash-merged.bat COM7 firmware\KONTAKTS.merged.bin
```

Сам [`flash-merged.bat`](flash-merged.bat) можно запускать и из папки `tools`,
но тогда путь к BIN нужно задавать относительно `tools` либо использовать полный путь.
Например:

```bat
flash-merged.bat COM7 ..\firmware\KONTAKTS.merged.bin
```

### Создание SHA-256 для BIN-файлов

[`make-firmware-hashes.bat`](make-firmware-hashes.bat) обрабатывает `.bin` файлы
**в текущей рабочей папке**, а не автоматически в папке самого BAT-файла.

Поэтому если BIN-файлы лежат в корне репозитория, запускайте из корня:

```bat
tools\make-firmware-hashes.bat
```

Если BIN-файлы лежат, например, в `firmware\`, сначала перейдите туда и вызовите
BAT относительным путём:

```bat
cd firmware
..\tools\make-firmware-hashes.bat
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

## Практический короткий цикл

Для обычной работы с уже собранной KONTAKTS firmware удобно держать release BIN
в отдельной папке, например `firmware\`, и работать из корня репозитория:

```bat
tools\flash-merged.bat COM7 firmware\KONTAKTS.merged.bin
```

Для обновления хешей:

```bat
cd firmware
..\tools\make-firmware-hashes.bat
```

Так `tools` остаётся папкой инструментов, а готовые BIN и `SHA256SUMS.txt` не
смешиваются со скриптами.
