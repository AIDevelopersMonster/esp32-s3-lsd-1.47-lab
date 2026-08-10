# `src` — как устроен `main.cpp`

Эта папка содержит основной код прошивки для **Waveshare ESP32-S3-LCD-1.47**.

Главный файл:

- [`main.cpp`](./main.cpp) — запуск платы, работа с LCD, RGB LED, microSD, Wi-Fi и кнопкой BOOT.

Аппаратные GPIO и размеры дисплея вынесены отдельно:

- [`../include/board_config.h`](../include/board_config.h) — номера выводов, размер LCD и скорость Serial.

> Если нужно изменить GPIO платы, лучше менять `board_config.h`, а не разносить номера GPIO по `main.cpp`.

---

## Что делает прошивка

После включения ESP32-S3:

1. настраивается кнопка BOOT;
2. запускается Serial на скорости `115200`;
3. запускается встроенный RGB LED;
4. инициализируется дисплей ST7789;
5. проверяется microSD;
6. показывается страница `SYSTEM`;
7. в основном цикле отслеживается нажатие кнопки BOOT.

Каждое нажатие BOOT переключает страницу:

```text
SYSTEM -> WI-FI SCAN -> ONBOARD PINS -> SYSTEM -> ...
```

При переходе на страницу `WI-FI SCAN` выполняется новое сканирование Wi-Fi сетей.

---

# Структура `main.cpp`

## 1. Подключение библиотек

В начале файла подключаются Arduino и библиотеки периферии:

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <SD_MMC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_NeoPixel.h>

#include "board_config.h"
```

`board_config.h` содержит описание аппаратной части платы.

---

## 2. Объекты оборудования

```cpp
Adafruit_ST7789 display(&SPI, Board::TFT_CS, Board::TFT_DC, Board::TFT_RST);
Adafruit_NeoPixel statusLed(1, Board::RGB_LED, NEO_GRB + NEO_KHZ800);
```

- `display` — LCD ST7789;
- `statusLed` — встроенный адресный RGB LED.

Они создаются один раз и затем используются во всём файле.

---

## 3. Страницы интерфейса

Перечень страниц задаёт `enum class Page`:

```cpp
enum class Page : uint8_t {
  System = 0,
  WiFi = 1,
  Pinout = 2,
};
```

Текущая страница хранится здесь:

```cpp
Page currentPage = Page::System;
```

Функция

```cpp
void drawCurrentPage()
```

вызывает функцию отрисовки нужной страницы.

Сейчас существуют:

```cpp
drawSystemPage();
drawWiFiPage();
drawPinoutPage();
```

---

# Как добавить новую страницу

Например, хотим добавить страницу `Sensors`.

## Шаг 1. Добавить её в `Page`

Было:

```cpp
enum class Page : uint8_t {
  System = 0,
  WiFi = 1,
  Pinout = 2,
};
```

Станет:

```cpp
enum class Page : uint8_t {
  System = 0,
  WiFi = 1,
  Pinout = 2,
  Sensors = 3,
};
```

## Шаг 2. Создать функцию страницы

```cpp
void drawSensorsPage() {
  drawHeader(F("SENSORS"), ST77XX_CYAN);

  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(1);
  display.setCursor(8, 45);
  display.print(F("Sensor page"));

  drawFooter();
}
```

## Шаг 3. Добавить её в `drawCurrentPage()`

```cpp
case Page::Sensors:
  drawSensorsPage();
  break;
```

## Шаг 4. Изменить число страниц в `nextPage()`

Сейчас используется:

```cpp
const auto next = (static_cast<uint8_t>(currentPage) + 1U) % 3U;
```

После добавления четвёртой страницы должно быть:

```cpp
const auto next = (static_cast<uint8_t>(currentPage) + 1U) % 4U;
```

При большом количестве страниц лучше позднее заменить это число на отдельную константу, чтобы не менять его вручную.

---

# Как изменить страницу `SYSTEM`

Вся её отрисовка находится в:

```cpp
void drawSystemPage()
```

Строки вида

```cpp
printKeyValue(43, F("Chip"), String(ESP.getChipModel()));
```

означают:

```text
43          координата Y
"Chip"      подпись слева
value       значение справа
```

Например:

```cpp
printKeyValue(79, F("CPU"), String(ESP.getCpuFreqMHz()) + F(" MHz"));
```

показывает частоту CPU.

Чтобы добавить ещё одну строку:

```cpp
printKeyValue(250, F("My value"), F("123"));
```

Нужно следить, чтобы координата `Y` не пересекалась с другими строками и нижним footer.

---

# Как рисовать непосредственно на дисплее

Используется объект:

```cpp
display
```

Основные операции:

```cpp
display.fillScreen(ST77XX_BLACK);
display.setTextColor(ST77XX_WHITE);
display.setTextSize(1);
display.setCursor(8, 50);
display.print(F("Hello"));
```

Прямоугольник:

```cpp
display.fillRect(x, y, width, height, ST77XX_BLUE);
```

Линия:

```cpp
display.drawFastHLine(x, y, width, ST77XX_WHITE);
```

Для общей шапки уже есть:

```cpp
drawHeader(...);
```

а для нижней строки:

```cpp
drawFooter();
```

Их лучше использовать для новых страниц, чтобы интерфейс оставался единообразным.

---

# RGB LED

Для изменения цвета используется:

```cpp
setLed(red, green, blue);
```

Например:

```cpp
setLed(32, 0, 0);   // красный
setLed(0, 32, 0);   // зелёный
setLed(0, 0, 32);   // синий
setLed(0, 0, 0);    // выключить
```

В текущей прошивке LED используется как простой индикатор состояния.

---

# microSD

Инициализация карты находится в:

```cpp
bool initializeSdCard()
```

GPIO берутся из `Board::SD_*` в `board_config.h`.

Результат сохраняется в:

```cpp
bool sdMounted;
uint64_t sdSizeMb;
```

Поэтому другие части программы могут проверить:

```cpp
if (sdMounted) {
  // карта доступна
}
```

---

# Wi-Fi

Сканирование выполняет:

```cpp
void scanWiFi()
```

Результаты складываются в:

```cpp
NetworkInfo networks[8];
int networkCount;
```

Сейчас отображается максимум 8 сетей.

Это задаётся строкой:

```cpp
networkCount = min(found, 8);
```

и размером массива:

```cpp
NetworkInfo networks[8];
```

Если менять лимит, нужно изменить оба места одновременно.

Страница результатов:

```cpp
void drawWiFiPage()
```

---

# Кнопка BOOT

Кнопка обрабатывается функцией:

```cpp
void handleBootButton()
```

Она вызывается постоянно из `loop()`.

В коде уже реализован программный debounce примерно `35 ms`, поэтому одно физическое нажатие обычно считается одним событием.

При нажатии вызывается:

```cpp
nextPage();
```

Если вместо переключения страниц нужно выполнить другое действие, менять логику следует именно здесь или внутри `nextPage()`.

---

# `setup()` — что выполняется один раз

Arduino вызывает:

```cpp
void setup()
```

только один раз после старта или reset.

В этом проекте там выполняются:

```text
BOOT GPIO
  -> Serial
  -> RGB LED
  -> LCD
  -> стартовый экран
  -> microSD
  -> SYSTEM page
```

Инициализацию нового оборудования обычно следует добавлять именно в `setup()`.

---

# `loop()` — что выполняется постоянно

```cpp
void loop() {
  handleBootButton();
  delay(5);
}
```

`loop()` повторяется всё время, пока работает ESP32.

Сейчас он специально оставлен очень маленьким.

Для периодической задачи лучше не добавлять длинные `delay(1000)` и подобные блокировки. Предпочтительнее использовать `millis()`:

```cpp
static uint32_t lastUpdate = 0;

if (millis() - lastUpdate >= 1000) {
  lastUpdate = millis();

  // код, выполняемый раз в секунду
}
```

Так кнопка и интерфейс продолжат быстро реагировать.

---

# Где менять GPIO

Не рекомендуется писать в `main.cpp`:

```cpp
pinMode(38, OUTPUT);
```

Если этот GPIO относится к аппаратуре платы, используйте имя из:

```text
include/board_config.h
```

Например:

```cpp
Board::RGB_LED
Board::BOOT_BUTTON
Board::TFT_MOSI
Board::SD_CLK
```

Это делает код понятнее и позволяет изменить разводку платы в одном месте.

---

# Важное правило при изменении `main.cpp`

Удобный порядок работы:

1. сначала определить, к какому блоку относится новая функция;
2. добавить маленькую отдельную функцию;
3. вызвать её из `setup()`, `loop()` или нужной страницы;
4. собрать проект;
5. прошить плату;
6. проверить Serial Monitor и LCD;
7. только после этого добавлять следующую функцию.

Не стоит сразу превращать `loop()` в большой монолит.

По мере роста проекта крупные части `main.cpp` можно будет вынести в отдельные `.cpp/.h` модули, например:

```text
src/
  main.cpp
  display_ui.cpp
  wifi_tools.cpp
  sd_tools.cpp

include/
  board_config.h
  display_ui.h
  wifi_tools.h
  sd_tools.h
```

Пока текущий `main.cpp` остаётся достаточно небольшим, разделять его только ради разделения не требуется.

---

# Сборка проекта через PlatformIO

Проект использует PlatformIO. Конфигурация находится в корневом файле:

```text
platformio.ini
```

Окружение по умолчанию:

```text
waveshare_esp32s3_lcd_147
```

## 1. Перейти в корень проекта

Команды PlatformIO нужно выполнять не из `src`, а из папки, где лежит `platformio.ini`:

```powershell
cd C:\Users\CHUWI\Documents\GitHub\lab-esp32-s3-lcd-1.47
```

Используйте **один терминал для одной сборки**. Не запускайте несколько `pio run`
одновременно: они используют один и тот же `.pio\build`.

## 2. Проверить PlatformIO

Если `pio` доступен напрямую:

```powershell
pio --version
```

Если обычный PowerShell его не видит:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" --version
```

## 3. Собрать проект

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run
```

На первой сборке PlatformIO может устанавливать платформу, компилятор, Arduino
framework и библиотеки. Не запускайте второй процесс, пока первый не завершился.

Нормальное окончание:

```text
========================= [SUCCESS] =========================
```

Основной результат сборки создаётся в:

```text
.pio\build\waveshare_esp32s3_lcd_147\firmware.bin
```

## 4. Очистка после прерванной или конфликтующей сборки

Если сборка была прервана, либо несколько терминалов одновременно запускали
PlatformIO, сначала выполните:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run -t clean
```

После `SUCCESS` снова:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run
```

Этот порядок реально проверен в проекте после ошибки линковки с отсутствующими
объектными файлами Wi-Fi.

## 5. Найти порт

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" device list
```

Номер COM-порта может меняться. В проверенном сеансе использовался `COM16`.

## 6. Прошить плату

Пример для `COM16`:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run -t upload --upload-port COM16
```

Успешная прошивка заканчивается проверкой записанных данных и сообщениями типа:

```text
Hash of data verified.
Hard resetting via RTS pin...
========================= [SUCCESS] =========================
```

## 7. Открыть Serial Monitor

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" device monitor --port COM16
```

Скорость Serial:

```text
115200 baud
```

## Проверенная последовательность

```text
редактируем src/main.cpp
        ↓
pio run
        ↓
SUCCESS
        ↓
pio run -t upload --upload-port COMxx
        ↓
Hash of data verified / SUCCESS
        ↓
pio device monitor --port COMxx
```

[Видео: сборка и программирование ESP32-S3-LCD-1.47 через PlatformIO](https://youtube.com/shorts/qIY_et3dHcA)
— реальный успешный цикл сборки, записи Flash, проверки данных и reset платы.

---

## Главное

Для первого знакомства с проектом достаточно помнить четыре точки входа:

```cpp
setup();              // один раз после старта
loop();               // работает постоянно
drawCurrentPage();    // рисует выбранную страницу
handleBootButton();   // обрабатывает кнопку BOOT
```

Если требуется изменить аппаратные выводы — смотрите `include/board_config.h`.
Если требуется изменить поведение прошивки — начинайте с `src/main.cpp`.
