# Back up the factory firmware first

The board is supplied with a useful factory diagnostic firmware. Save it before
uploading your own program.

## Windows PowerShell

Install esptool:

```powershell
py -m pip install --upgrade esptool
```

Run the included script, replacing `COM7` with the actual port:

```powershell
.\tools\backup-factory.ps1 -Port COM7
```

The script reads the complete detected Flash and saves it to:

```text
backup\esp32-s3-lcd-1.47-factory.bin
```

The `backup` directory is excluded from Git so that a device-specific binary is
not published accidentally.

## Manual command

```powershell
py -m esptool --chip esp32s3 --port COM7 --baud 460800 read-flash 0 ALL factory.bin
```

Keep the resulting binary and its SHA-256 checksum in two separate locations.
