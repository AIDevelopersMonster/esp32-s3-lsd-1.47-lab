# ESP32-S3 eFuse security audit example

This document is a **real, anonymized audit** of one Waveshare ESP32-S3-LCD-1.47 board captured with `espefuse v5.3.1`.

Source snapshot files were generated with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\tools\read-efuses.ps1 -Port COM16
```

The per-device raw files remain under `backup/efuses/` and are excluded from Git.

> [!WARNING]
> eFuses are one-time-programmable. This document is for inspection only. Do not burn security, key, JTAG, USB, calibration, or write-protection eFuses unless you fully understand the irreversible consequences.

## Overall result

The audited chip is in an **open development state**, not a locked production-security state.

### Security summary

| Area | Observed eFuse state | Interpretation |
|---|---|---|
| Secure Boot | `SECURE_BOOT_EN = False` | Secure Boot is not enabled |
| Flash Encryption | `SPI_BOOT_CRYPT_CNT = 0b000` / `Disable` | Flash encryption is not enabled |
| Anti-rollback | `SECURE_VERSION = 0` | No anti-rollback secure version has been burned |
| Secure-download mode | `ENABLE_SECURITY_DOWNLOAD = False` | Secure UART download mode is not enabled |
| Download mode | `DIS_DOWNLOAD_MODE = False` | ROM download mode is not disabled |
| Force-download disable | `DIS_FORCE_DOWNLOAD = False` | Force-download functionality has not been disabled |
| JTAG soft disable | `SOFT_DIS_JTAG = 0` | JTAG is not soft-disabled |
| JTAG hard disable | `DIS_PAD_JTAG = False` | Pad JTAG is not permanently disabled |
| USB-JTAG disable | `DIS_USB_JTAG = False` | USB switch-to-JTAG is not disabled |
| USB Serial/JTAG | `DIS_USB_SERIAL_JTAG = False` | USB Serial/JTAG block is enabled |
| USB OTG | `DIS_USB_OTG = False` | USB OTG is enabled |
| USB download restrictions | both relevant disable bits are `False` | USB download paths are not disabled by eFuse |
| Key blocks | `BLOCK_KEY0..5 = all zero` | No security keys are provisioned in these key blocks |
| Key purposes | all `USER` | No key block is assigned to Secure Boot / Flash Encryption purposes |
| Read protection | `RD_DIS = 0` | No BLOCK4-10 read-disable bits are set |
| Write protection | `WR_DIS = 0` | No individual eFuse write-disable bits are set |

## Secure Boot

Observed:

```text
SECURE_BOOT_EN                = False
SECURE_BOOT_AGGRESSIVE_REVOKE = False
SECURE_BOOT_KEY_REVOKE0       = False
SECURE_BOOT_KEY_REVOKE1       = False
SECURE_BOOT_KEY_REVOKE2       = False
```

No Secure Boot enable bit is burned, and no Secure Boot key has been revoked.

All six key-purpose fields are still:

```text
KEY_PURPOSE_0 = USER
KEY_PURPOSE_1 = USER
KEY_PURPOSE_2 = USER
KEY_PURPOSE_3 = USER
KEY_PURPOSE_4 = USER
KEY_PURPOSE_5 = USER
```

The corresponding key blocks are all zero and readable/writeable in the snapshot.

**Conclusion:** Secure Boot has not been provisioned on this board.

## Flash Encryption

Observed:

```text
SPI_BOOT_CRYPT_CNT = 0b000 = Disable
```

No key block is assigned an XTS AES flash-encryption purpose, and all key blocks are still zero.

**Conclusion:** Flash Encryption is not enabled.

This means a full raw Flash backup made with `esptool read-flash` is expected to contain plaintext firmware/data rather than an encrypted-at-rest image.

## Anti-rollback

Observed:

```text
SECURE_VERSION = 0
```

**Conclusion:** no anti-rollback secure version has been programmed through this eFuse.

## JTAG state

Observed:

```text
SOFT_DIS_JTAG = 0
DIS_PAD_JTAG  = False
DIS_USB_JTAG  = False
STRAP_JTAG_SEL = False
```

**Conclusion:** neither the soft JTAG disable nor the permanent pad-JTAG disable is active. USB-to-JTAG switching is also not disabled by eFuse.

This is consistent with a development-oriented chip state.

## ROM download-mode state

Observed:

```text
DIS_DOWNLOAD_MODE           = False
ENABLE_SECURITY_DOWNLOAD    = False
DIS_FORCE_DOWNLOAD          = False
DIS_DOWNLOAD_ICACHE         = False
DIS_DOWNLOAD_DCACHE         = False
DIS_DOWNLOAD_MANUAL_ENCRYPT = False
```

**Conclusion:** ordinary ROM download mode has not been disabled, and secure download mode has not been enabled.

This is the expected configuration for a board intended to remain easy to flash and recover during development.

## USB state

Observed:

```text
DIS_USB_OTG                      = False
DIS_USB_SERIAL_JTAG              = False
DIS_USB_JTAG                     = False
DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE = False
DIS_USB_OTG_DOWNLOAD_MODE         = False
DIS_USB_SERIAL_JTAG_ROM_PRINT     = Enable
USB_EXCHG_PINS                    = False
USB_EXT_PHY_ENABLE                = False
```

**Conclusion:** the ESP32-S3 USB functions are not disabled by eFuse. USB Serial/JTAG and ROM printing remain available, and the USB download paths are not blocked by these disable bits.

## Key blocks and access protection

All key blocks were observed as 256 bits of zero:

```text
BLOCK_KEY0 = 00 ... 00
BLOCK_KEY1 = 00 ... 00
BLOCK_KEY2 = 00 ... 00
BLOCK_KEY3 = 00 ... 00
BLOCK_KEY4 = 00 ... 00
BLOCK_KEY5 = 00 ... 00
```

And:

```text
RD_DIS = 0
WR_DIS = 0
```

The snapshot reports the fields as `R/W`.

Important: **`R/W` only describes current eFuse access-protection state. It does not mean these fields are safe to modify.** eFuses are physically one-time programmable.

## Flash-related eFuse detail

A useful distinction appears in this sample:

```text
FLASH_CAP    = None
FLASH_VENDOR = None
FLASH_TEMP   = None
FLASH_TYPE   = 4 data lines
```

Yet `esptool flash-id` independently detected a real external SPI Flash device with a size of **16 MB**.

So, on this board, the actual Flash capacity is obtained from the attached Flash device / JEDEC identification rather than from the optional capacity/vendor eFuse fields. The `FLASH_TYPE` eFuse does confirm the quad / 4-data-line configuration.

## PSRAM and supply configuration

Observed:

```text
PSRAM_CAP       = 8M
PSRAM_TEMP      = 85C
PSRAM_VENDOR    = AP_3v3
VDD_SPI_XPD     = True
VDD_SPI_TIEH    = VDD_SPI connects to VDD3P3_RTC_IO
VDD_SPI_FORCE   = True
PIN_POWER_SELECTION = VDD_SPI
```

**Conclusion:** the embedded PSRAM is identified as 8 MB with the AP 3.3 V encoding, and the VDD_SPI supply configuration is explicitly forced by eFuse to the 3.3 V path.

## Silicon / calibration snapshot

The same eFuse dump confirms:

- wafer minor version `2`;
- calibration block version `ADC calib V1`, minor `3`;
- factory ADC1/ADC2 calibration values;
- temperature-sensor calibration;
- digital and RTC LDO / bias calibration values.

These are normal factory-programmed calibration and identity fields and should be treated as read-only reference data in this project.

## Privacy

The raw eFuse snapshot contains per-device identifiers, including:

```text
MAC
OPTIONAL_UNIQUE_ID
```

They are intentionally omitted from this public audit.

## Practical interpretation for this project

At the time of this snapshot, the board remains easy to develop with and recover:

- Secure Boot is off;
- Flash Encryption is off;
- JTAG is not disabled;
- USB Serial/JTAG remains enabled;
- ROM download mode remains available;
- no security keys are provisioned;
- no eFuse read/write protection bits are set.

This is a good state for experimentation, backup/restore tests, Arduino, PlatformIO and ESP-IDF development.

It is **not** a hardened production-security configuration.

## Official references

- [Espressif espefuse summary documentation](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/espefuse/summary-cmd.html)
- [ESP32-S3 eFuse Manager](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/system/efuse.html)
- [ESP32-S3 Security Overview](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/security/security.html)
- [ESP32-S3 Flash Encryption](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/security/flash-encryption.html)
