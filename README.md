# Ensy AHU-400 → Home Assistant

ESPHome firmware for a LilyGO T-CAN485 that bridges an Ensy AHU-400 (LMC311 controller,
Modbus RTU on CN7) to Home Assistant over WiFi. One firmware for every board; nothing
in it is device- or owner-specific.

**Setting one up at home? Read the [user guide](docs/user-guide.md)**: wiring, WiFi, what the LED colours mean.

## How it works

- **Releases:** every push to `main` that changes the firmware is built by CI and
  published as a GitHub release (`manifest.json`, `firmware.ota.bin`, `firmware.factory.bin`).
- **Updates:** boards check the latest release every 6 h and install it automatically.
  Boards running a local build (`version: dev`) only show the update; they never auto-install.
- **No credentials in the binary:**
  - WiFi is set on site through the board's AP **`Ensy AHU setup`**, password
    `ensy-ahu-setup`. The AP is only up while the board has no working WiFi.
  - The API key is provisioned by the owner's Home Assistant when the device is adopted.
    This has to happen within 24 h of power-up; otherwise power-cycle the AHU.
    The same key also authorises OTA uploads.
  - Push OTA works only on a board that once got a local build with `ota_password`
    set. Any other board gets a random password at every boot.

## New board

1. Flash `firmware.factory.bin` from the latest release over USB-C, using
   [web.esphome.io](https://web.esphome.io) in Chrome or Edge.
2. Wire it: CN2 pin 1 (12 V) → VIN; CN7 pin 2 → A, pin 3 → B, pin 6 → GND.
3. On a phone, join WiFi `Ensy AHU setup` (password `ensy-ahu-setup`), pick the home
   WiFi in the page that opens, and enter its password.
4. Home Assistant finds "Ensy AHU-400" under Settings → Devices → Discovered. Click
   Configure; HA sets the encryption key by itself.

Status LED: red = no WiFi, blue = no Home Assistant, yellow = AHU not answering, green = OK.

## Local builds

`ensy-ahu.yaml` is the entry point. A local ESPHome dashboard includes it from a stub
that sets `components_path` and `ota_password`:

```yaml
substitutions:
  components_path: ensy-ahu-firmware/components
  ota_password: !secret ensy_ota_password
packages:
  ensy: !include ensy-ahu-firmware/ensy-ahu.yaml
```

## Files

| Path | What |
|---|---|
| `ensy-ahu.yaml` | Device config: identity, WiFi, API, OTA, updates |
| `ensy-ahu-core.yaml` | AHU logic: Modbus, entities, climate, status LED |
| `ensy-ahu-debug.yaml` | Read-only sensors for undocumented registers, plus a change tracker |
| `components/ensy_climate` | The grouped "Ventilation" climate entity |
| `docs/user-guide.md` | Plain-language setup and troubleshooting for owners |
| `docs/` | Register map, scan results, CO2 options |
