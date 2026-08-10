# Waveshare ESP32-S3-LCD-1.47 — manufacturer Project Resources catalog

Source page:

https://www.waveshare.com/wiki/ESP32-S3-LCD-1.47

Waveshare has a **Project Resources** section containing third-party projects for this board or closely related ESP32-S3 hardware. These are not maintained by Waveshare; the manufacturer provides links to the original authors.

This file is our index. Each project will later receive a separate research card under `../../projects/`.

## Catalog

| # | Project / author | Original resources | Our status |
|---|---|---|---|
| 01 | Hakista TV (Pinoy Hacker) — USB Army Knife on Waveshare ESP32-S3 1.47 / ESP32-S3-GEEK | YouTube: https://www.youtube.com/watch?v=eMnJFsxMS94 ; related video: https://www.youtube.com/watch?v=roNRKHlf9f0 ; main project: https://github.com/i-am-shodan/USBArmyKnife ; auxiliary Espressif binary: https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/boot_app0.bin ; browser flasher used in the video description: https://esp.huhn.me/ | `REFERENCE_ONLY` — detailed card created |
| 02 | Jstudner10 — Jcorp Nomad: Mini WIFI Media Server | Guide: https://www.instructables.com/Jcorp-Nomad-Mini-WIFI-Media-Server/ ; GitHub: https://github.com/Jstudner/jcorp-nomad | `REFERENCE_ONLY` |
| 03 | moononournation — Loki Miss Minutes clock / GIF Animation Clock | YouTube: https://www.youtube.com/watch?v=e-Z9bq2xT2E ; Guide: https://www.instructables.com/GIF-Animation-Clock/ ; GitHub: https://github.com/moononournation/animation_clock | `REFERENCE_ONLY` |
| 04 | Valleytech Custom Solutions — Forget Expensive Hacking Tools – Try This Instead | YouTube: https://www.youtube.com/watch?v=_E7mgh94wxk | `REFERENCE_ONLY` |

## Classification rule

A video description can contain several links, but they do not necessarily represent several projects.

For example, the first Hakista TV resource contains:

```text
USB Army Knife
    ├── main application/project
    │   https://github.com/i-am-shodan/USBArmyKnife
    │
    ├── boot_app0.bin
    │   auxiliary Arduino-ESP32 / Espressif flashing binary
    │
    └── ESPWebTool
        browser-based flashing utility
```

For our catalog, this is **one project plus supporting resources**, not three independent projects.

## Research order

We will process the manufacturer list in order:

1. USB Army Knife
2. Jcorp Nomad mini Wi-Fi media server
3. Loki / GIF Animation Clock
4. Valleytech Custom Solutions video/project

For every item we will answer:

- What does it actually do?
- Why was it created?
- What hardware features does it demonstrate?
- Does it really support our Waveshare ESP32-S3-LCD-1.47?
- What is needed only for installation/flashing and what belongs to the application itself?
- What is worth reproducing on our hardware?
- What general engineering idea could become an independent KONTAKTS/Lab project?
