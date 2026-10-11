# Ensy AHU-400 BH: CO2 sensing options

Notes from 2026-10-08 for later. Nothing is installed yet; the AHU has no CO2 sensor
(input register 222 reads 0).

## Option 1: CO2 sensor wired into the AHU

The LMC311 has a dedicated analog input for CO2 on **CN11** (from the InoVent wiring diagram):

| CN11 pin | Signal | Use |
|---|---|---|
| 1 | VAN1 | power for the humidity (RH) sensor |
| 2 | A1 | RH sensor signal |
| 3 | GND | |
| 4 | VAN2 | **power for the CO2 sensor** |
| 5 | A2 | **CO2 sensor signal** |
| 6 | GND | **CO2 sensor ground** |

Ceiling models also bring "CO2" out to the external quick-connect terminal strip. On wall
models it has to be wired inside the AHU.

**Before buying, check two things with a multimeter:**
1. VAN2 (CN11-4) to GND: the supply voltage, probably 12 V or 24 V.
2. A1 (CN11-2) to GND while RH is ~64 %: if it reads ~6.4 V, the inputs are 0–10 V.

**Sensor:** a generic 3-wire NDIR CO2 transmitter with a 0–10 V output (~€40–80) should
work. The unknown is the ppm range the controller expects for 0–10 V: 0–2000 ppm is
common, but Ensy doesn't document it. Ensy's own CO2 accessory avoids that guesswork.

**Once fitted:**
- The AHU's built-in CO2 control uses holding registers 1920–1922. They are already set:
  normal up to 600 ppm, high from 800 ppm, and step 3 on high.
- The reading appears in input register 222 (ppm).

- **Pros:** works without HA; the controller handles it natively.
- **Cons:** measures the *mixed extract air* of the whole house, so one stuffy bedroom
  gets diluted (1500 ppm in the room may read ~800 at the AHU). It also means opening the unit.

## Option 2: CO2 sensor on the T-CAN485 board

Add a **Sensirion SCD41** (~€20–30, I²C, NDIR) to the ESP32 on spare GPIOs. ESPHome has an
`scd4x` component. It shows up in HA directly; HA then controls the fan step.

- **Pros:** cheap, no AHU wiring or scaling questions.
- **Cons:** it sits at the AHU, so it has the same mixed-air limitation unless it's mounted in a room.
  If HA is down, nothing reacts to CO2.

## Option 3: CO2 sensors in the rooms, HA controls the fans (preferred)

Put a sensor in each room that matters (bedrooms, living room). An HA automation takes the
**highest** reading and writes the fan step (holding 1003):

| Highest CO2 | Fan step |
|---|---|
| < 800 ppm | 1 |
| 800–1000 ppm | 2 |
| > 1000 ppm | 3 |

Add hysteresis or a minimum hold time (e.g. 10 min) before stepping down, so the fans don't
flap between steps. Leave Boost (551) and manual changes from the wall panel alone, for
example by pausing the automation for a while after a manual change.

**Sensor choices (all NDIR; avoid "eCO2" / VOC-estimate sensors):**

| Option | ~Cost each | Notes |
|---|---|---|
| DIY ESPHome node: ESP32 + SCD41 or SenseAir S8 | €15–30 | Cheapest, fully local, also gives temperature and humidity; needs USB power |
| IKEA ALPSTUGA | ~€30 | Off the shelf, Matter over Thread; HA needs a Thread border router |
| Aranet4 | ~€150+ | Very accurate, battery, Bluetooth (HA via a BT proxy) |
| Airthings (View Plus, Wave) | ~€200+ | Accurate, battery, cloud or BLE |

Avoid MH-Z19 modules from marketplaces; fakes are common.

- **Pros:** measures what people actually breathe; per-room visibility in HA.
- **Cons:** depends on HA being up; costs more sensors.

## Related registers

| Register | Meaning |
|---|---|
| Input 222 | AirQual.CO2 (ppm); only with option 1 |
| Holding 1920 | CO2_VentHi: fan step on high CO2 (currently 3) |
| Holding 1921 | CO2_LimLo: normal-ventilation limit (currently 600 ppm) |
| Holding 1922 | CO2_LimHi: high-ventilation limit (currently 800 ppm) |
| Holding 1003 | Control.VentSet: fan step 0–3, written by HA in options 2 and 3 |
