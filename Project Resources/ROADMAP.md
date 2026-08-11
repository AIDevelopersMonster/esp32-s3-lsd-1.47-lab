# ESP32-S3-LCD-1.47 — планы исследований и собственных проектов

Статус документа: `ACTIVE_ROADMAP`

Дата фиксации: `2026-08-11`

Этот файл — рабочая карта того, что ещё имеет смысл проверить на **Waveshare ESP32-S3-LCD-1.47**, и какие собственные проекты логично строить после изучения сторонних решений.

Цель — не собрать максимальное количество чужих прошивок, а найти хорошие инженерные паттерны для нашей платы и постепенно перейти к собственному полезному устройству.

## Что уже реально проверено

### KONTAKTS

Статус: `HARDWARE_VERIFIED`

Наша собственная диагностическая прошивка уже подтверждает базовую работу платы, дисплея, Flash, PSRAM, microSD, Wi-Fi/BLE-информации, BOOT-кнопки и RGB-индикатора.

### USBArmyKnife

Статус: исследовано / запускалось на нашей плате.

Интерес: использование ESP32-S3 как USB-инструмента, экрана состояния и автономного сервисного устройства.

Главный вывод: сама форма платы очень хорошо подходит для устройств класса `USB service stick`, но не обязательно для offensive/security-задач. Тот же аппаратный шаблон можно использовать для администрирования, настройки и диагностики собственного оборудования.

### Jcorp Nomad

Статус: `PARTIAL / REPRODUCIBILITY_FRAGILE`

Интерес: SD, Wi-Fi media server, browser UI, USB MSC и работа с файловой системой.

Главный вывод: функциональность интересная, но сочетание SD + USB MSC + сетевого доступа существенно повышает сложность и риск повреждения файловой системы. Использовать прежде всего как архитектурный пример и источник отрицательных уроков.

### Loki Miss Minutes GIF Animation Clock

Статус: `HARDWARE_VERIFIED`

Карточка: [`projects/MissMinutesClock/README.md`](projects/MissMinutesClock/README.md)

Наше видео: https://youtube.com/shorts/ZK_HQWrQ_ig

Интерес: Arduino_GFX, LittleFS, GIF-анимации, Wi-Fi/NTP, компактная state-driven UI без LVGL.

Главный вывод: для небольших экранных приложений прямой рендеринг + LittleFS может быть значительно проще и надёжнее тяжёлого UI-стека.

---

# Следующие сторонние проекты для проверки

Ниже не список обязательных прошивок, а очередь кандидатов. Для каждого сначала делаем research card, проверяем лицензию и архитектуру, затем решаем, стоит ли реально прошивать плату.

## Priority A — bmorcelli/Launcher

Original project:
https://github.com/bmorcelli/Launcher

Статус плана: `NEXT_CANDIDATE`

Почему особенно интересен: начиная с версии 2.6.9 автор явно добавил порт для **Waveshare ESP32-S3-LCD-1.47**.

Launcher умеет:

- устанавливать firmware binaries через OTA;
- ставить `.bin` с microSD;
- принимать firmware через WebUI;
- работать с файлами на microSD;
- показывать и изменять partition scheme;
- создавать, удалять, форматировать и изменять размеры разделов;
- сохранять и восстанавливать data partitions;
- работать с несколькими прошивками как с приложениями;
- иметь serial interface для конфигурации.

### Почему это важно для нашего lab

Если Launcher нормально работает именно на нашей плате, она перестаёт быть устройством с одной прошивкой и становится **экспериментальной платформой для быстрого переключения приложений**.

Желаемый результат:

```text
Launcher
  ├── KONTAKTS
  ├── Miss Minutes
  ├── OpsKey
  ├── display tests
  └── другие test firmware
```

### Что обязательно проверить

- корректность LCD orientation и offset;
- работу единственной BOOT-кнопки;
- microSD на нашей распиновке;
- OTA;
- сохранность partition/data при переходе между приложениями;
- возможность загрузки наших merged/application BIN;
- recovery path, если установленная прошивка не стартует.

Не использовать управление разделами без полного backup Flash и понимания текущей partition table.

---

## Priority A — Claude Code Usage Monitor как шаблон DevOps-display

Original project:
https://github.com/rootedlab-code/claude-code-usage-monitor

Статус плана: `REFERENCE_FIRST`

Проект специально ориентирован на Waveshare ESP32-S3-LCD-1.47 и использует:

- LVGL;
- Wi-Fi;
- локальный Python bridge;
- JSON endpoint;
- captive portal;
- Prometheus `/metrics`;
- BOOT button gestures;
- RGB status LED;
- persistent configuration;
- offline/reconnect logic.

Нас интересует не столько мониторинг Claude, сколько готовый паттерн:

```text
локальный/серверный источник данных
          ↓ Wi-Fi / JSON
ESP32-S3-LCD-1.47
          ↓
маленький always-on dashboard
```

Это почти прямой технический прототип для собственного `OpsKey / Service Stick`.

---

## Priority B — Claude Desktop Buddy / hardware companion

Reference implementation / related port:
https://github.com/eukatree/claude-desktop-buddy-esp32-s3-lcd-1.47

Статус плана: `REFERENCE_ONLY` до проверки актуальности desktop protocol.

Интересные идеи:

- BLE-связь с desktop application;
- аппаратный companion для событий компьютера;
- состояния `sleep / idle / busy / attention / celebrate`;
- GIF character packs;
- передача набора анимаций на устройство;
- использование маленького дисплея как физического агента/индикатора состояния.

Это хорошо сочетается с уже проверенным Miss Minutes-паттерном: одна и та же GIF-инфраструктура может отображать не время, а состояние локального сервиса, AI-agent, build system или оборудования.

---

## Priority B — ESPHome / Home Assistant status panel

Reference:
https://community.home-assistant.io/

Искать конфигурации по:

```text
ESP32-S3-LCD-1.47 ST7789 172x320 LVGL ESPHome
```

Статус плана: `REFERENCE_ONLY`

Возможное применение:

- компактный status panel;
- температура/влажность/энергия;
- состояние теплицы/дома/серверной;
- OTA;
- web setup/captive portal;
- отображение данных Home Assistant без отдельного браузера.

Поскольку экран не touch, навигацию можно делать BOOT-кнопкой или автоматически вращать страницы.

---

## Priority B — Cortex Link / USB↔BLE service bridge

Original project:
https://github.com/turfptax/cortex-link

Статус репозитория: архивирован; использовать как `REFERENCE_ONLY`.

Интересные функции:

- MicroPython;
- BLE GATT server;
- USB serial ↔ BLE bridge;
- ST7789 status display;
- microSD;
- RGB status LED;
- BOOT button;
- OTA/update pattern.

Главная ценность — пример того, как одна маленькая плата может быть одновременно транспортным мостом и локальной диагностической консолью.

---

## Priority C — USB Keyring / macro stick

Original project:
https://github.com/alfem/usb-keyring

Статус плана: `REFERENCE_ONLY`

Функции:

- USB HID keyboard;
- макросы с microSD;
- выбор макроса одной кнопкой;
- LCD feedback;
- RGB status.

Полезный общий принцип: **контекстная USB-клавиша с экраном**.

Для собственной разработки возможен безопасный вариант только для локальной автоматизации:

- шаблоны команд;
- часто используемые пути/URL;
- команды настройки нашего оборудования;
- текстовые заготовки;
- сервисные последовательности, которые пользователь явно запускает кнопкой.

Не хранить открытые пароли, API tokens и другие секреты на обычной SD без отдельной модели защиты.

---

## Priority C — Retro-Go / мини-консоль

Port/reference candidate:
https://github.com/kchimera/retro-go-espego

Статус плана: `LOW_PRIORITY_EXPERIMENT`

Интерес не в превращении проекта в игровую приставку как основной сценарий, а в проверке:

- производительности ESP32-S3;
- быстрого display pipeline;
- файлового браузера с SD;
- launcher architecture;
- сохранения состояния;
- Wi-Fi file manager.

Ограничение нашей платы — одна BOOT-кнопка, поэтому полноценное игровое управление потребует внешних кнопок/BLE-контроллера или других аппаратных изменений.

---

## Priority C — ESP-IDF/LVGL/Wi-Fi/BLE demo forks

Reference example:
https://github.com/mylesdebastion/waveshare-esp32-s3-lcd-1.47

Статус плана: `REFERENCE_ONLY`

Полезно для сравнения Arduino с ESP-IDF:

- LVGL;
- Wi-Fi auto reconnect;
- BLE scan;
- SDMMC;
- RGB LED;
- особенности API ESP-IDF 5.x.

Не является конечным приложением, но может быть хорошей базой для проверки low-level ESP-IDF toolchain на нашей плате.

---

## Priority C — ESP-STICK enclosure / корпус

Original project:
https://github.com/diedasman/ESP32-STICK

Статус плана: `MECHANICAL_REFERENCE`

Интерес:

- STEP/STL корпуса под конкретную Waveshare ESP32-S3-LCD-1.47;
- превращение dev board в законченное карманное устройство;
- полезно, если `OpsKey` выйдет за пределы лабораторного прототипа.

Перед использованием геометрии и файлов отдельно проверить лицензию/условия распространения конкретных mechanical assets.

---

# Собственный главный проект: OpsKey / Service Stick

Рабочее название: **OpsKey** / **Service Stick** / **KONTAKTS Ops**.

Статус: `CONCEPT_ACCEPTED / NOT_IMPLEMENTED`

Это не hacking tool и не ещё одни часы. Идея — сделать из формы USB-stick полноценный **карманный инженерный терминал диагностики и настройки собственного оборудования и инфраструктуры**.

## Базовый сценарий

Плата питается от USB или power bank, подключается к заданной Wi-Fi-сети и сразу показывает состояние инфраструктуры.

Пример страницы:

```text
OPS KEY
────────────────
WiFi       OK
192.168.1.37
RSSI      -48 dBm
Gateway    OK
DNS        OK
NTP        OK
NET       23 ms
────────────────
BOOT: next
```

Следующая страница:

```text
SERVICES
────────────────
Router      OK
Server      OK
MQTT        OK
API        ERR
Backup      OK
TLS        27d
```

Радиодиагностика:

```text
RADIO
────────────────
WiFi APs    17
Channel      6
RSSI      -48
BLE devs    12
```

## Планируемые функции OpsKey

### Phase 1 — автономная сетевая диагностика

- Wi-Fi scan;
- подключение к сохранённой сети;
- RSSI;
- IP / gateway / DNS;
- ping-like reachability checks;
- DNS lookup;
- NTP check;
- HTTP/HTTPS endpoint checks;
- измерение latency;
- BOOT для смены страниц;
- RGB: green/yellow/red.

### Phase 2 — captive portal configuration

Без перекомпиляции firmware:

- настройка Wi-Fi;
- список контролируемых hosts/services;
- interval проверок;
- пользовательские labels;
- timezone;
- reset configuration.

Секреты не публиковать и не хранить в репозитории.

### Phase 3 — profiles на microSD

Профиль определяет, что именно проверять:

```text
/home
/server
/greenhouse
/router
/lab-device
```

Формат — JSON/YAML-подобный config, без необходимости пересобирать firmware.

### Phase 4 — настройка оборудования

Для наших собственных устройств:

- BLE provisioning;
- Wi-Fi provisioning;
- чтение device info;
- проверка firmware version;
- выдача локального configuration profile;
- запуск безопасных сервисных команд;
- журнал результата на SD.

### Phase 5 — DevOps mode

Плата становится физическим dashboard:

- GitHub Actions status;
- сервер availability;
- API health;
- backup status;
- certificate expiration;
- MQTT availability;
- Prometheus-derived counters;
- локальные агенты/bridge processes.

Сильный шаблон для этого режима можно взять из `claude-code-usage-monitor`, заменив Claude-specific backend на наш универсальный service endpoint.

### Phase 6 — USB service console

Опционально:

- USB CDC console;
- вывод диагностического JSON/text на host;
- экспорт журнала;
- локальные, явно выбранные пользователем HID macros для рутинных операций.

HID-функции должны быть только пользовательски инициируемой автоматизацией собственного оборудования, а не скрытой инъекцией команд.

---

# Возможная архитектура собственного проекта

```text
                  ┌─────────────────────┐
                  │      OpsKey UI      │
                  │ ST7789 + BOOT + RGB │
                  └─────────┬───────────┘
                            │
                 ┌──────────▼──────────┐
                 │   State / Menu Core │
                 └──────┬────┬────┬────┘
                        │    │    │
             ┌──────────┘    │    └────────────┐
             ▼               ▼                 ▼
        Wi-Fi tools      BLE tools         USB CDC
             │               │                 │
             ▼               ▼                 ▼
        HTTP/DNS/NTP     provisioning      host console
             │
             ▼
       service profiles
             │
       LittleFS / microSD
```

## UI principle

Не строить тяжёлый интерфейс без причины.

Для первых версий достаточно:

```text
page state machine
+ direct Arduino_GFX rendering
+ small icons/fonts
+ optional GIF/status animation
```

LVGL вводить только если появится реальная необходимость в сложных виджетах, графиках или reusable UI components.

---

# Идея объединения через Launcher

Если `bmorcelli/Launcher` проходит аппаратную проверку, его можно использовать как инфраструктурный слой лаборатории:

```text
ESP32-S3-LCD-1.47
        │
        ▼
     Launcher
        │
 ┌──────┼─────────┬───────────┐
 ▼      ▼         ▼           ▼
KONTAKTS Miss     OpsKey      Tests
         Minutes
```

Это позволяет не пытаться собрать все функции в один монолитный firmware.

Разные задачи остаются отдельными приложениями, а сама плата становится маленькой экспериментальной платформой.

---

# Что пока не стоит делать

Низкий приоритет:

- ещё одни обычные часы;
- простой погодный экран без дополнительной инженерной идеи;
- Bitcoin/NerdMiner только ради демонстрации;
- очередной декоративный GIF player без нового поведения;
- монолитная прошивка, которая пытается одновременно включить все найденные проекты.

Такие проекты могут использоваться как короткие hardware tests, но не являются основной линией развития lab.

---

# Рабочая очередь

```text
[1] HARDWARE_VERIFIED  Miss Minutes
[2] NEXT               bmorcelli/Launcher
[3] RESEARCH           Claude Code Usage Monitor architecture
[4] DESIGN             OpsKey Phase 1
[5] IMPLEMENT          OpsKey Wi-Fi diagnostics + UI
[6] EXTEND             captive portal + profiles
[7] EXTEND             BLE/device provisioning
[8] EXTEND             DevOps/service monitoring
[9] OPTIONAL           enclosure / ESP-STICK mechanical study
```

Приоритет может меняться после каждого аппаратного теста. Новый проект добавляется в очередь только если он показывает новый полезный аппаратный или программный паттерн.

## Основной критерий

Вопрос для любого следующего проекта:

> **Что нового этот проект позволяет узнать о возможностях ESP32-S3-LCD-1.47 или использовать в нашем собственном устройстве?**

Если ответа нет, проект не заслуживает отдельного цикла портирования и аппаратной проверки.
