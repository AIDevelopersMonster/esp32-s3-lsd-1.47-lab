# KONTAKTS release files

В эту папку вручную или скриптом копируются только готовые release-файлы прошивки.
Каталог `build/`, который создаёт Arduino IDE, сюда не переносится.

Для версии `v0.1.0` ожидаются:

```text
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.bin
KONTAKTS_ESP32_S3_LCD_1_47_v0.1.0.merged.bin
SHA256SUMS.txt
```

- `.bin` — только application image;
- `.merged.bin` — полный объединённый flash image для записи с `0x0`;
- `SHA256SUMS.txt` — контрольные SHA-256.

Скопировать два BIN из свежего Arduino IDE build можно из корня репозитория командой:

```bat
tools\export-kontakts-release.bat 0.1.0
```

Скрипт берёт исходные файлы из:

```text
firmware\KONTAKTS_ESP32_S3_LCD_1_47\build\esp32.esp32.esp32s3\
```

и принудительно перезаписывает соответствующие release-копии в этой папке.
