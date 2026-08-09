# esptool: команды для ESP32-S3-LCD-1.47

Эта памятка относится к плате **Waveshare ESP32-S3-LCD-1.47** и рассчитана прежде всего на PowerShell в Windows.

`esptool` — официальный инструмент Espressif для связи с ROM bootloader / flasher stub микроконтроллеров ESP32. Он умеет определять микросхему, читать параметры Flash, сохранять содержимое Flash, стирать и записывать Flash.

> [!IMPORTANT]
> Команды чтения (`flash-id`, `read-flash`) безопасны для содержимого Flash. Команды `erase-flash` и `write-flash` изменяют данные платы — используйте их только после сохранения заводской прошивки.

## Установка и проверка

Установка:

```powershell
py -m pip install esptool
```

Проверка версии:

```powershell
py -m esptool version
```

В момент первой аппаратной проверки этой платы использовался:

```text
esptool v5.3.1
```

## Определение платы и Flash

Для нашей платы на `COM16` была выполнена команда:

```powershell
py -m esptool --chip esp32s3 --port COM16 flash-id
```

Фактический ответ платы:

```text
esptool v5.3.1
Connected to ESP32-S3 on COM16:
Chip type:          ESP32-S3 (QFN56) (revision v0.2)
Features:           Wi-Fi, BT 5 (LE), Dual Core + LP Core, 240MHz, Embedded PSRAM 8MB (AP_3v3)
Crystal frequency:  40MHz
USB mode:           USB-Serial/JTAG
MAC:                [redacted]

Stub flasher running.

# Flash Memory Information:

Manufacturer: ef
Device: 4018
Detected flash size: 16MB
Flash type set in eFuse: quad (4 data lines)
Flash voltage set by eFuse: 3.3V

Hard resetting via RTS pin...
```

MAC-адрес в публичной документации намеренно не публикуется: он уникален для конкретного экземпляра платы и для работы проекта не нужен.

### Что мы подтвердили аппаратно

Для конкретного экземпляра платы подтверждено:

| Параметр | Фактическое значение |
|---|---|
| MCU | ESP32-S3, QFN56 |
| Revision | v0.2 |
| CPU | Dual Core + LP Core, до 240 MHz |
| PSRAM | 8 MB Embedded PSRAM |
| Crystal | 40 MHz |
| USB mode | USB-Serial/JTAG |
| Flash manufacturer ID | `0xEF` |
| Flash device ID | `0x4018` |
| Flash size | **16 MB** |
| Flash bus | Quad, 4 data lines |
| Flash voltage | 3.3 V |

`Manufacturer: ef` — JEDEC manufacturer ID `0xEF`, используемый Winbond. Идентификатор `0x4018` соответствует 128-Mbit SPI NOR Flash семейству; 128 Mbit = 16 MB. Это согласуется с маркировкой микросхемы **W25Q128JVSIQ**, видимой на плате.

## Почему `read-flash 0 ALL` не требует заранее знать размер

Команда резервного копирования использует:

```text
read-flash 0 ALL
```

Здесь:

- `0` — начальный адрес `0x00000000`;
- `ALL` — попросить `esptool` определить полный размер Flash и прочитать её целиком.

Размер определяет **не Windows и не PowerShell**. Это делает `esptool`, общаясь с ESP32-S3 и SPI Flash через ROM bootloader / flasher stub.

В нашем случае `flash-id` уже подтвердил:

```text
Detected flash size: 16MB
```

Поэтому полный backup должен иметь размер:

```text
16 * 1024 * 1024 = 16777216 bytes
```

## Полный backup заводской Flash

Рекомендуемый способ — проектный скрипт:

```powershell
.\tools\backup-factory.ps1 -Port COM16
```

Он внутри вызывает `esptool` примерно так:

```powershell
py -m esptool --chip esp32s3 --port COM16 --baud 460800 read-flash 0 ALL backup\esp32-s3-lcd-1.47-factory.bin
```

Эта команда только читает Flash и создаёт бинарный файл на компьютере.

### Медленный, но более устойчивый режим

Если USB-соединение нестабильно:

```powershell
.\tools\backup-factory.ps1 -Port COM16 -Baud 115200
```

или напрямую:

```powershell
py -m esptool --chip esp32s3 --port COM16 --baud 115200 read-flash 0 ALL backup\esp32-s3-lcd-1.47-factory.bin
```

## Проверка размера backup-файла

PowerShell:

```powershell
(Get-Item .\backup\esp32-s3-lcd-1.47-factory.bin).Length
```

Для нашей 16 MB Flash ожидается:

```text
16777216
```

Более наглядно:

```powershell
$size = (Get-Item .\backup\esp32-s3-lcd-1.47-factory.bin).Length
"{0:N2} MiB" -f ($size / 1MB)
```

Ожидаемый результат:

```text
16.00 MiB
```

## SHA-256 резервной копии

Наш `backup-factory.ps1` автоматически считает SHA-256. Отдельно это можно сделать так:

```powershell
Get-FileHash .\backup\esp32-s3-lcd-1.47-factory.bin -Algorithm SHA256
```

Сохраните hash вместе с `.bin`. При последующей проверке совпадающий SHA-256 показывает, что файл не изменился побайтно.

## Полезные команды esptool

### Справка

```powershell
py -m esptool --help
```

Справка по конкретной команде:

```powershell
py -m esptool flash-id --help
py -m esptool read-flash --help
py -m esptool write-flash --help
```

### Информация о Flash

```powershell
py -m esptool --chip esp32s3 --port COM16 flash-id
```

Это хорошая первая команда при подключении новой платы: она одновременно проверяет связь и сообщает ключевые параметры Flash.

### Прочитать конкретный диапазон Flash

Общий вид:

```powershell
py -m esptool --chip esp32s3 --port COM16 read-flash <ADDRESS> <SIZE> <FILE>
```

Например, прочитать первые 64 KiB:

```powershell
py -m esptool --chip esp32s3 --port COM16 read-flash 0x0 0x10000 first-64k.bin
```

### Прочитать Flash целиком

```powershell
py -m esptool --chip esp32s3 --port COM16 read-flash 0 ALL full-flash.bin
```

### Информация о бинарном образе

Для файла прошивки ESP32 можно использовать:

```powershell
py -m esptool image-info firmware.bin
```

Это не то же самое, что анализ полного 16 MB raw backup: `image-info` предназначен прежде всего для ESP image-файлов, имеющих корректный заголовок образа.

## Команды, которые изменяют Flash

> [!WARNING]
> Следующие команды потенциально уничтожают заводскую прошивку. Перед их использованием сохраните полный backup и SHA-256.

### Полное стирание Flash

```powershell
py -m esptool --chip esp32s3 --port COM16 erase-flash
```

После этой команды пользовательские данные и заводская прошивка во Flash будут удалены.

### Запись бинарного файла

Общий вид:

```powershell
py -m esptool --chip esp32s3 --port COM16 write-flash <ADDRESS> <FILE>
```

Не используйте произвольный адрес без понимания таблицы разделов и структуры образа.

Для восстановления полного raw backup отдельный проектный скрипт восстановления будет безопаснее прямого ручного вызова `write-flash`.

## Если плата не определяется

Сначала убедитесь, что выбран правильный COM-порт. Затем можно вручную войти в download mode:

1. удерживать **BOOT**;
2. кратко нажать **RST**;
3. отпустить **RST**;
4. отпустить **BOOT**;
5. повторить команду `esptool`.

На нашей плате также доступен встроенный **USB-Serial/JTAG**, что подтверждено фактическим выводом `flash-id`.

## PowerShell: перенос длинной команды на несколько строк

В PowerShell используется обратный апостроф — backtick:

```powershell
py -m esptool `
    --chip esp32s3 `
    --port COM16 `
    --baud 460800 `
    read-flash 0 ALL `
    backup\esp32-s3-lcd-1.47-factory.bin
```

После символа `` ` `` не должно быть пробела.

## Безопасный порядок первой работы с новой платой

1. Выполнить `flash-id`.
2. Зафиксировать тип MCU, PSRAM и размер Flash.
3. Сделать полный `read-flash 0 ALL`.
4. Проверить размер полученного файла.
5. Сохранить SHA-256.
6. Скопировать backup ещё в одно место.
7. Только после этого загружать тестовую прошивку.

## Ссылки

- [Espressif esptool documentation](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/)
- [Basic commands](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/esptool/basic-commands.html)
- [Waveshare ESP32-S3-LCD-1.47 Wiki](https://www.waveshare.com/wiki/ESP32-S3-LCD-1.47)

---

Аппаратные значения в разделе «Что мы подтвердили аппаратно» получены непосредственно с конкретного экземпляра Waveshare ESP32-S3-LCD-1.47 командой `esptool v5.3.1 flash-id`.