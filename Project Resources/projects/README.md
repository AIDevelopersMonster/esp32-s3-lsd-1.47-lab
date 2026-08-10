# Project Resources — project index

Здесь находятся публичные исследовательские карточки сторонних проектов.

Для каждого проекта создаётся отдельная папка:

```text
projects/<short-project-name>/README.md
```

Карточка создаётся по шаблону [`../_TEMPLATE.md`](../_TEMPLATE.md).

В этой папке не размещаются сторонние исходники по умолчанию. Локальные копии для сборки и адаптации хранятся в `../_local/`, которая исключена из Git.

## Manufacturer-listed projects

Каталог проектов, опубликованных производителем Waveshare для ESP32-S3-LCD-1.47:

[`../manufacturer/Waveshare-ESP32-S3-LCD-1.47/README.md`](../manufacturer/Waveshare-ESP32-S3-LCD-1.47/README.md)

## Index

### 01 — USB Army Knife

- Card: [`USBArmyKnife/README.md`](USBArmyKnife/README.md)
- Upstream: https://github.com/i-am-shodan/USBArmyKnife
- Status: `REFERENCE_ONLY`
- Relevance: **HIGH** — upstream contains a dedicated PlatformIO target `Waveshare-ESP32-S3-LCD-1_47`.
- Supporting resources used by the referenced video:
  - Espressif `boot_app0.bin`: https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/boot_app0.bin
  - Browser flasher: https://esp.huhn.me/

The `boot_app0.bin` file and browser flasher are classified as supporting installation resources, not separate application projects.
