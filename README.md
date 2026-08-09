# ESP32-S3-LCD-1.47 Lab

![Плата с обратной стороны](docs/images/board-back.jpg)

Учебный и прикладной GitHub-проект для платы **Waveshare ESP32-S3-LCD-1.47**
с USB-A, экраном ST7789 172 × 320, слотом microSD, 16 МБ Flash и 8 МБ PSRAM.

Репозиторий предназначен для аккуратного исследования платы: сохранения
заводской прошивки, аппаратной идентификации, проверки eFuse, дисплея, microSD,
Wi-Fi, USB и последующей разработки собственных приложений на Arduino,
PlatformIO и ESP-IDF.

## Текущий статус

Плата уже проверена физически через `esptool v5.3.1` и `espefuse v5.3.1`.
Подтверждено на реальном экземпляре:

- **ESP32-S3 QFN56, revision v0.2**;
- Dual Core + LP Core, до 240 МГц;
- **8 MB embedded PSRAM** (`AP_3v3`);
- **16 MB SPI Flash**;
- Quad Flash, 3.3 V;
- кварц 40 МГц;
- USB-Serial/JTAG;
- заводские ADC/temperature/LDO calibration eFuse;
- открытая development-конфигурация security eFuse.

Уникальные идентификаторы конкретного экземпляра — MAC, optional unique ID и
аналогичные per-device значения — в публичной документации намеренно не
публикуются и заменяются на `<REDACTED>`.

## Что уже подготовлено

Основная прошивка `src/main.cpp` превращает плату в небольшой диагностический
терминал:

- показывает модель микроконтроллера, частоту, Flash, PSRAM и свободную память;
- проверяет наличие microSD и показывает её объём;
- сканирует Wi-Fi и выводит сети с уровнем сигнала;
- показывает таблицу выводов встроенных устройств;
- переключает страницы кнопкой **BOOT**;
- использует встроенный RGB-светодиод как индикатор состояния;
- собирается в PlatformIO и проверяется через GitHub Actions.

Перед первой собственной прошивкой рекомендуется сохранить полный заводской
образ Flash и снимок eFuse.

## Быстрый старт в PlatformIO

1. Установите VS Code и расширение PlatformIO.
2. Откройте папку проекта.
3. Подключите плату через USB-удлинитель или USB-хаб.
4. Сначала сохраните заводскую прошивку по инструкции
   [docs/firmware-backup.md](docs/firmware-backup.md).
5. Выполните:

```bash
pio run
pio run -t upload
pio device monitor
```

В `platformio.ini` закреплена среда Arduino-ESP32 3.x. Официальная документация
Waveshare требует пакет ESP32 версии не ниже 3.0.2.

## esptool, backup и eFuse

Для низкоуровневой диагностики платы добавлены отдельные материалы:

- [docs/esptool.md](docs/esptool.md) — команды `flash-id`, `read-flash`,
  проверка размера backup, SHA-256 и основные операции `esptool`;
- [docs/firmware-backup.md](docs/firmware-backup.md) — процедура сохранения
  заводской прошивки;
- [tools/backup-factory.ps1](tools/backup-factory.ps1) — полный read-only backup
  Flash;
- [tools/read-efuses.ps1](tools/read-efuses.ps1) — read-only снимок eFuse в
  текстовом и JSON-форматах;
- [docs/examples/efuse-active-example.md](docs/examples/efuse-active-example.md)
  — обезличенный пример активных eFuse;
- [docs/examples/efuse-security-audit-example.md](docs/examples/efuse-security-audit-example.md)
  — обезличенный security-аудит реального экземпляра платы.

### Проверенная security-конфигурация

На исследованном экземпляре:

| Область | Состояние |
|---|---|
| Secure Boot | выключен |
| Flash Encryption | выключено |
| Anti-rollback `SECURE_VERSION` | `0` |
| ROM download mode | доступен |
| Secure download mode | не включён |
| JTAG soft disable | не установлен |
| JTAG hard disable | не установлен |
| USB Serial/JTAG | доступен |
| USB OTG | не отключён eFuse |
| Security key blocks 0…5 | не provisioned, нулевые |
| Read protection `RD_DIS` | `0` |
| Write protection `WR_DIS` | `0` |

Это удобная **development-конфигурация**: плата остаётся пригодной для обычной
прошивки, восстановления и отладки. Она не является hardened production
security configuration.

> [!WARNING]
> eFuse — однократно программируемая память. Скрипт `read-efuses.ps1` выполняет
> только чтение. Команды записи/burn намеренно не автоматизированы в этом
> проекте: изменение security, key, JTAG, USB, calibration или protection eFuse
> может быть необратимым.

### Что маскируется перед публикацией

В публичные примеры не следует переносить без необходимости:

- MAC-адрес конкретного устройства;
- `OPTIONAL_UNIQUE_ID`;
- custom MAC / пользовательские уникальные идентификаторы;
- ключи Secure Boot / Flash Encryption и любые другие secret key blocks;
- Wi-Fi credentials, API tokens, пароли и иные секреты, если они встретятся в
  дампах или конфигурационных файлах.

Raw-снимки хранятся локально в `backup/`, который исключён через `.gitignore`.
Публичные примеры содержат только необходимые технические параметры и
обезличенные значения.

## Arduino IDE

Инструкция и рекомендуемые параметры находятся в
[docs/arduino-ide.md](docs/arduino-ide.md).

Для первичной проверки можно открыть:

```text
examples/01_display_test/01_display_test.ino
```

## Распиновка встроенных устройств

### Дисплей ST7789

| Сигнал | GPIO |
|---|---:|
| MOSI | 45 |
| SCLK | 40 |
| CS | 42 |
| DC | 41 |
| RESET | 39 |
| Подсветка | 48 |

### microSD в режиме SD_MMC

| Сигнал | GPIO |
|---|---:|
| CMD | 15 |
| CLK | 14 |
| D0 | 16 |
| D1 | 18 |
| D2 | 17 |
| D3 | 21 |

Встроенный адресный RGB-светодиод подключён к **GPIO38**, кнопка BOOT — к
**GPIO0**. Полная памятка: [docs/pinout.md](docs/pinout.md).

## Структура

```text
.
├── src/main.cpp
├── include/board_config.h
├── examples/
│   ├── 01_display_test/
│   ├── 02_sd_card_test/
│   └── 03_wifi_scanner/
├── docs/
│   ├── pinout.md
│   ├── arduino-ide.md
│   ├── firmware-backup.md
│   ├── esptool.md
│   ├── examples/
│   │   ├── efuse-active-example.md
│   │   └── efuse-security-audit-example.md
│   └── roadmap.md
├── tools/
│   ├── backup-factory.ps1
│   └── read-efuses.ps1
├── platformio.ini
└── .github/workflows/build.yml
```

## Важные замечания

- Это модель **ESP32-S3-LCD-1.47 с USB-A**.
- Не путать с **ESP32-S3-LCD-1.47B**: это другая аппаратная версия.
- Не путать с сенсорной `ESP32-S3-Touch-LCD-1.47`: её распиновка отличается.
- Сохраняйте заводской Flash backup до первого стирания или полной перепрошивки.
- Сохраняйте исходный eFuse snapshot как baseline перед security-экспериментами.
- При проблемах с загрузкой удерживайте BOOT, кратко нажмите RESET, отпустите
  RESET, затем BOOT.

## План развития

Следующие этапы: LVGL, просмотр изображений с microSD, USB-накопитель,
UART/I2C-терминал, MQTT-панель и компактные измерительные приложения.
Подробно: [docs/roadmap.md](docs/roadmap.md).

## Источники

- [Официальная Wiki Waveshare](https://www.waveshare.com/wiki/ESP32-S3-LCD-1.47)
- [Официальный комплект примеров](https://files.waveshare.com/wiki/ESP32-S3-LCD-1.47/ESP32-S3-LCD-1.47-Demo.zip)
- [Arduino-ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- [Espressif esptool/espefuse](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/)
- [ESP32-S3 eFuse Manager](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/system/efuse.html)

## Лицензия

Оригинальный код проекта распространяется по лицензии MIT. Сторонние
библиотеки и официальные материалы Waveshare имеют собственные лицензии.
