# ESP32-S3 eFuse active-field example

This file is an **anonymized real example** captured from a Waveshare ESP32-S3-LCD-1.47 board with `espefuse v5.3.1`.

The original snapshot was produced by:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\tools\read-efuses.ps1 -Port COM16
```

The raw per-device files remain under `backup/efuses/` and are intentionally excluded from Git.

> [!IMPORTANT]
> This is an **active-fields-only** view. Fields that were zero and not protected are omitted by the helper script. It is therefore useful as a compact hardware/calibration snapshot, but it is not a substitute for the complete `espefuse summary` when auditing all security eFuses.

## Privacy redaction

Two per-device identifiers are deliberately removed from this public example:

```text
MAC                 <REDACTED>
OPTIONAL_UNIQUE_ID  <REDACTED>
```

## Active fields captured

| eFuse | Raw value | Decoded value | Notes |
|---|---:|---:|---|
| ADC1_CAL_VOL_ATTEN0 | `0x79` | `484` | Factory ADC1 calibration |
| ADC1_CAL_VOL_ATTEN1 | `0x76` | `472` | Factory ADC1 calibration |
| ADC1_CAL_VOL_ATTEN2 | `0x6f` | `444` | Factory ADC1 calibration |
| ADC1_CAL_VOL_ATTEN3 | `0x79` | `484` | Factory ADC1 calibration |
| ADC1_INIT_CODE_ATTEN0 | `0x8b` | `-44` | Factory ADC1 initial-code calibration |
| ADC1_INIT_CODE_ATTEN1 | `0x2a` | `-40` | Factory ADC1 initial-code calibration |
| ADC1_INIT_CODE_ATTEN2 | `0x1e` | `120` | Factory ADC1 initial-code calibration |
| ADC1_INIT_CODE_ATTEN3 | `0x26` | `-24` | Factory ADC1 initial-code calibration |
| ADC2_CAL_VOL_ATTEN0 | `0x88` | `-32` | Factory ADC2 calibration |
| ADC2_CAL_VOL_ATTEN1 | `0x42` | `-8` | Factory ADC2 calibration |
| ADC2_CAL_VOL_ATTEN2 | `0x45` | `-20` | Factory ADC2 calibration |
| ADC2_CAL_VOL_ATTEN3 | `0x20` | `0` | Factory ADC2 calibration |
| ADC2_INIT_CODE_ATTEN0 | `0x94` | `-80` | Factory ADC2 initial-code calibration |
| ADC2_INIT_CODE_ATTEN1 | `0x2d` | `-52` | Factory ADC2 initial-code calibration |
| ADC2_INIT_CODE_ATTEN2 | `0x12` | `72` | Factory ADC2 initial-code calibration |
| ADC2_INIT_CODE_ATTEN3 | `0x22` | `-8` | Factory ADC2 initial-code calibration |
| BLK_VERSION_MAJOR | `0x1` | `ADC calib V1` | Calibration block format |
| BLK_VERSION_MINOR | `0x3` | `3` | Calibration block version detail |
| DIG_DBIAS_HVT | `0x18` | `-32` | Factory digital-bias calibration |
| K_DIG_LDO | `0x42` | `-8` | Factory digital LDO calibration |
| K_RTC_LDO | `0x40` | `0` | Factory RTC LDO calibration |
| OCODE | `0x52` | `82` | Factory analog calibration code |
| PIN_POWER_SELECTION | `0x1` | `VDD_SPI` | Power-selection configuration |
| PSRAM_CAP | `0x1` | `8M` | Embedded PSRAM capacity encoding |
| PSRAM_CAPACITY | `0x1` | `1` | PSRAM capacity-related factory field |
| PSRAM_TEMP | `0x2` | `85C` | PSRAM temperature grade |
| PSRAM_VENDOR | `0x1` | `AP_3v3` | Embedded PSRAM vendor/voltage encoding |
| TEMP_CALIB | `0x11c` | `-2.8` | Internal temperature-sensor calibration |
| V_DIG_DBIAS20 | `0x84` | `-16` | Factory digital-bias calibration |
| V_RTC_DBIAS20 | `0x02` | `8` | Factory RTC-bias calibration |
| VDD_SPI_FORCE | `0x1` | `true` | eFuse forces VDD_SPI configuration |
| VDD_SPI_TIEH | `0x1` | `VDD_SPI connects to VDD3P3_RTC_IO` | Selects the 3.3 V VDD_SPI path |
| VDD_SPI_XPD | `0x1` | `true` | VDD_SPI regulator/power configuration |
| WAFER_VERSION_MINOR | `0x2` | `2` | Silicon wafer/revision information |
| WAFER_VERSION_MINOR_LO | `0x2` | `2` | Lower portion of wafer revision field |

## What this tells us

The captured active fields are dominated by **factory calibration, identity/revision, embedded PSRAM, and VDD_SPI configuration data**.

In particular, the board reports:

- embedded PSRAM configured as **8 MB**;
- AP 3.3 V PSRAM encoding;
- VDD_SPI explicitly configured for the 3.3 V path;
- ADC1/ADC2 calibration data present;
- temperature, LDO, and bias calibration data present;
- silicon minor revision field equal to `2`.

The helper script reported every shown entry as readable and writable at the eFuse access-control level. This must **not** be interpreted as permission to modify calibration or identity eFuses. eFuses are one-time programmable and these factory fields should be treated as read-only reference data in this project.

## Security-audit limitation

No security eFuse appears in this active-only capture. Under the helper script's filtering rule, that means none of the security fields exposed by the JSON snapshot were selected as non-zero, protected, or error-bearing.

However, a definitive audit of Secure Boot, Flash Encryption, JTAG-disable, download-mode restrictions, key-purpose fields, and read/write protection should be made from the **complete summary or JSON file**, where zero-valued fields are visible explicitly.

Do not infer a production security configuration from this compact example alone.

## Related project files

- `tools/read-efuses.ps1` — read-only snapshot helper
- `docs/esptool.md` — esptool/espefuse command notes
- `backup/efuses/` — local raw per-device snapshots (Git-ignored)
