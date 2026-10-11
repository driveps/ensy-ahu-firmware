# Ensy AHU-400 BH (LMC311): full register scan

Scanned 2026-10-08 ~20:25 (+0300) by temporarily flashing `ensy-ahu-scan.yaml`, a read-only
firmware that polls every register in the LMC311 Modbus manual (Ver 2.0, 11.2021) as raw
U_WORD, one request per register. Unit state at scan time: On, Auto, fan step 1,
setpoint 19 °C, control state "Heating".

To re-scan, OTA-install `ensy-ahu-scan.yaml`, read the states, then reinstall `ensy-ahu.yaml`.

**Raw** is the 16-bit value as read. **Value** applies the manual's scale
(÷100 where the manual says scale 100). Temperatures are signed (S_WORD).

**Status** column:
- ✅ real: live data that makes sense on this unit
- ⚙️ setting: a configured value; a 0 can still be valid
- ⚪ not fitted: reads 0 because this unit doesn't have the hardware or function
- ⚠️ odd: needs a closer look (see notes)
- ⛔ dangerous: never write

## Input registers (FC04, read-only)

| Addr | Name | Raw | Value | Status | Notes |
|---|---|---|---|---|---|
| 0 | Bus.Version | 1 | 1 | ✅ | protocol version |
| 1 | App.VersionMajor | 8242 | " 2" | ✅ | ASCII 0x2032 |
| 2 | App.VersionMinor | 8240 | " 0" | ✅ | ASCII 0x2030 |
| 3 | App.VersionRelease | 12848 | "20" | ✅ | ASCII 0x3230, so firmware **2.0.20** |
| 100 | Input.UserFunc | 0 | 0 | ✅ | user function input |
| 101 | Input.AirFilter | 0 | 0 | ✅ | filter alarm (0 = OK) |
| 102 | Input.DoorOpen | 0 | 0 | ✅ | door contact |
| 103 | Input.Smoke | 0 | 0 | ✅ | smoke alarm |
| 104 | Input.MotorThermo | 0 | 0 | ✅ | motor thermal fuse |
| 105 | Input.Frost_Overht | 0 | 0 | ✅ | heater frost / overheat |
| 106 | Input.AirFlow | 0 | 0 | ✅ | airflow monitor |
| 200 | T0 controller board | 2363 | 23.63 °C | ✅ | |
| 201 | T1 fresh air intake | 1830 | 18.30 °C | ✅ | outside air |
| 202 | T2 supply before heater | 2186 | 21.86 °C | ✅ | after recuperator |
| 203 | T3 room exhaust (extract) | 2123 | 21.23 °C | ✅ | |
| 204 | T4 outlet (to outside) | 0 | 0 | ⚪ | no sensor fitted |
| 205 | T5 condenser | 0 | 0 | ⚪ | heat pump only |
| 206 | T6 evaporator | 0 | 0 | ⚪ | heat pump only |
| 207 | T7 supply after heater | 2186 | 21.86 °C | ✅ | equals T2 (heater off) |
| 208 | T8 outdoor | 1830 | 18.30 °C | ✅ | mirrors T1 (duplicate) |
| 209 | T9 heater surface | 2188 | 21.88 °C | ✅ | |
| 210 | T10 external room | 0 | 0 | ⚪ | |
| 211 | T11 hot water top | 0 | 0 | ⚪ | DHW only |
| 212 | T12 hot water bottom | 0 | 0 | ⚪ | DHW only |
| 213 | T13 return | 0 | 0 | ⚪ | hydronic only |
| 214 | T14 supply | 0 | 0 | ⚪ | hydronic only |
| 215 | T15 user-panel room temp | 2319 | 23.19 °C | ✅ | wall panel's sensor |
| 221 | AirQual.RH | 6394 | 63.94 % | ✅ | |
| 222 | AirQual.CO2 | 0 | 0 ppm | ⚪ | no CO2 sensor |
| 223 | T after rotor | 2152 | 21.52 °C | ✅ | |
| 230 | ActSupplySpeed | 1 | "1 %" | ⚠️ | manual says %, but tracks fan step (see h201 = 30 %) |
| 231 | ActExtractSpeed | 1 | "1 %" | ⚠️ | same as 230 |
| 400 | Alarm.Status | 0 | 0 | ✅ | bit 0x80 = alarms active, 0x03 = count |
| 401–409 | Alarm list 1–3 (ID/date/time) | 0 | — | ✅ | empty, no alarms; date and time are in DOS format |
| 1000 | Control.RunAct | 1 | On | ✅ | actual on/off |
| 1001 | Control.ModeAct | 3 | Auto | ✅ | 0 Off / 1 Heat / 2 Cool / 3 Auto / 4 Service |
| 1002 | Control.StateDisplay | 7 | Heating | ✅ | 0–8, 13–16 (see manual) |
| 1003 | Control.SecInState | 2250 | 2250 s | ✅ | time in current state |
| 1200 | AirTemp.IsSummer | 1 | Summer | ✅ | |
| 1201 | AirTemp.TempInletSet | 1900 | 19.00 °C | ✅ | T7 supply setpoint |
| 1202 | AirTemp.TempControl | 2183 | 21.83 °C | ✅ | controlled temperature, actual |
| 1203 | AirTemp.TempRoom | 2121 | 21.21 °C | ✅ | room temp used by the controller |
| 1204 | AirTemp.EffPct | 10000 | 100 % | ⚠️ | likely capped: (T2−T1)/(T3−T1) = 121 % right now |
| 1205 | AirTemp.CapSet | 19 | 0.19 % | ⚠️ | scale unclear: 0.19 % or 19 % |
| 1206 | AirTemp.CapAct | 19 | 0.19 % | ⚠️ | same as 1205 |
| 1800 | HeatExtSet | 1800 | 18.00 °C | ✅ | external heater setpoint |
| 2200 | DPT.InSession | 0 | 0 | ⚪ | no pressure (DPT) module |
| 2201 | DPT.AirPress1 | 0 | 0 Pa | ⚪ | |
| 2202 | DPT.AirPress2 | — / 30 | ? | ⚪ | no reply when read alone; reads 30 when read in one block with 2200-2201 (likely junk) |

## Holding registers (FC03 read, FC16 write only)

| Addr | Name | Raw | Value | Status | Notes |
|---|---|---|---|---|---|
| 0 | Bus.Address | 30 | 30 | ⛔ | node address; changing it cuts comms |
| 100 | Output.AirFlap | 1 | open | ✅ | outputs: read-only use recommended |
| 101 | Output.SmokeFlap | 1 | open | ✅ | |
| 102 | Output.BypassOpen | 0 | 0 | ✅ | |
| 103 | Output.BypassClose | 0 | 0 | ✅ | |
| 105 | Output.AirHeatAllow | 0 | 0 | ✅ | air heating not selected |
| 106 | Output.AirHeat | 0 | 0 | ✅ | heater relay off |
| 130 | Output.Rotor | 0 | off | ✅ | rotor exchanger output (plate unit?) |
| 131 | Output.Supply | 1 | on | ✅ | supply fan running |
| 132 | Output.Exhaust | 1 | on | ✅ | exhaust fan running |
| 200 | Output.ExhaustSpeed | 3000 | 30.00 % | ✅ | **real fan %** |
| 201 | Output.InletSpeed | 3000 | 30.00 % | ✅ | **real fan %** |
| 202 | Output.AirHeatCap | 0 | 0 % | ✅ | heater capacity |
| 300–305 | Time (s/min/h/d/m/y) | — | 2026-10-08 17:29:22 | ⚠️ | 3 h behind local time (UTC?) |
| 400 | Alarm.Reset | 0 | — | ⚙️ | command: 255 = clear all, 101–199 = clear a code |
| 500 | Program.Select | 0 | None | ⚙️ | week program 0 none / 1–3 / 4 erase |
| 550 | PartyModeCountdown | 0 | 0 | ⚙️ | |
| 551 | PartyMode | 2 | Stop | ⚙️ | 1 start / 2 stop |
| 600 | User1 FuncAct | 0 | inactive | ✅ | |
| 601 | User1 FuncSet | 2 | Inlet | ⚙️ | 0 None / 1 Extend / 2 Inlet / 3 Exhaust / 4 Ext heater offset / 5 Ventilate / 6 Cooker hood |
| 602 | User1 TimeSet | 500 | 500 min | ⚙️ | |
| 603 | User1 VentSet | 2 | step 2 | ⚙️ | |
| 604 | User1 TempSet | 23 | 23 °C | ⚙️ | no scale (whole °C) |
| 605 | User1 OffsSet | 0 | 0 °C | ⚙️ | |
| 610 | User2 FuncAct | 0 | inactive | ✅ | |
| 611 | User2 FuncSet | 6 | Cooker hood | ⚙️ | |
| 612 | User2 TimeSet | 0 | 0 min | ⚙️ | |
| 613 | User2 VentSet | 3 | step 3 | ⚙️ | |
| 614 | User2 TempSet | 23 | 23 °C | ⚙️ | |
| 615 | User2 OffsSet | 0 | 0 °C | ⚙️ | |
| 1001 | Control.RunSet | 1 | On | ⚙️ | **in use** (Power switch) |
| 1002 | Control.ModeSet | 3 | Auto | ⚙️ | **in use** (Mode select) |
| 1003 | Control.VentSet | 1 | step 1 | ⚙️ | **in use** (Fan step) 0–3 |
| 1004 | Control.TempSet | 1900 | 19.00 °C | ⚙️ | **in use** (setpoint) |
| 1005 | Control.ServiceMode | 0 | Off | ⚙️ | 0 Off / 1 Defrost / 2 Flaps / 3 Inlet / 4 Exhaust |
| 1006 | Control.ServicePct | 5000 | 50.00 % | ⚙️ | service-mode capacity |
| 1007 | Control.Preset | 0 | Ready | ⛔ | 1 = factory reset; never expose |
| 1201 | TempMinSum | 1400 | 14.00 °C | ⚙️ | inlet min, summer |
| 1202 | TempMinWin | 1500 | 15.00 °C | ⚙️ | inlet min, winter |
| 1203 | TempMaxSum | 3100 | 31.00 °C | ⚙️ | inlet max, summer |
| 1204 | TempMaxWin | 3200 | 32.00 °C | ⚙️ | inlet max, winter |
| 1205 | TempSummer | 1200 | 12.00 °C | ⚙️ | summer/winter switch-over |
| 1910 | RH_VentLo | 1 | step 1 | ⚙️ | humidity low step (winter) |
| 1911 | RH_VentHi | 3 | step 3 | ⚙️ | humidity high step |
| 1912 | RH_LimLo | 0 | 0 % | ⚙️ | 0 suggests humidity control is off |
| 1913 | RH_TimeOut | 0 | 0 min | ⚙️ | |
| 1920 | CO2_VentHi | 3 | step 3 | ⚙️ | no CO2 sensor, so these are unused |
| 1921 | CO2_LimLo | 600 | 600 ppm | ⚙️ | |
| 1922 | CO2_LimHi | 800 | 800 ppm | ⚙️ | |

## Findings

- **Fan speed:** use holding **200/201** (÷100 = 30 %). Input 230/231 report the fan step, not %.
- **Exhaust temp (204)** isn't fitted. Efficiency (1204) is then probably computed from
  T1/T2/T3 and capped at 100 %, so it's unreliable for now.
- **Duplicates:** 208 = 201, and 207 = 202 while the heater is off.
- **New useful values:**
  - room temp from the wall panel (215)
  - after-rotor temp (223)
  - controller board temp (200)
  - actual state "Heating" (1002) and time in state (1003)
  - summer/winter (1200)
  - flap, fan and heater outputs (100–202)
- **AHU clock** reads 17:29 while local time was ~20:25: it's 3 h behind, either set to UTC or simply wrong.
  It can be set via 300–305.
- **Dangerous:** holding 0 (address) and 1007 (factory preset). Writing the outputs
  (100–202) would override the controller directly, so keep them read-only.

## Full sweep 0–9999 (2026-10-11)

`ensy-ahu-fullscan.yaml` (read-only, raw Modbus on the UART, one register per request) probed
every address 0–9999 with FC04 and FC03. Every request got a clean answer: 160 registers
returned a value, and all the other 19 840 returned **exception 2 (illegal data address)**.
There were no timeouts and no other exception codes. So this is the complete map of the
0–9999 range; holding ≥10000 is the documented input mirror and wasn't swept.
Unit state: On, Auto, fan step 2, setpoint 19 °C, Heating.

Every register in the manual answered, except **input 2202, which is illegal**. The earlier
"30" for 2202 was block-read junk, as suspected.

**Registers not in the manual** (meaning unknown; raw values):

| Type | Addr | Raw | Guess |
|---|---|---|---|
| input | 107, 108, 109, 110, 113 | 0 | more digital inputs, after 100–106 |
| input | 216, 218, 219, 220 | 0 | more temperature/analog channels, unfitted |
| input | 232 | 0 | **air-quality override** (2026-10-11): 2 while humidity forces fan step 3, else 0. Not the panel's Air quality mode setting |
| holding | 104 | 0 | output (gap in 100–106) |
| holding | 107–125, 127, 128 | 0 except 127 = 1 | more outputs/relays |
| holding | 133 | 0 | **rotor drive %** ÷100 (2026-10-11): 10000 whenever the Rotor output (130) is on. Not the panel's Rotor max % setting |
| holding | 140, 141, 142 | 0, 3, 15 | ? |
| holding | 203, 204 | 0 | analog outputs after 200–202 (fans, heater) |
| holding | 1000 | 2 | ? (precedes RunSet 1001) |
| holding | 1100–1104 | 1, 0, 0, 100, 100 | ? 1103/1104 = 100 look like % limits |
| holding | 1200 | 0 | ? (precedes the temperature limits 1201–1205) |
| holding | 1700, 1701 | 0, 0 | ? |
| holding | 1800 | 65436 (= −100 → −1.00 °C) | probably an external-heater offset; input 1800 = setpoint 18 °C |
| holding | 2001 | 2 | ? |
| holding | 2200 | 0 | holding twin of input 2200 (DPT session) |

Since 2026-10-11, `ensy-ahu.yaml` reads all of them as read-only diagnostic sensors named
"Debug I…/H…" (raw signed values). The text sensor **Last register change** shows the latest
change, e.g. `H1100: 1 → 0`. To identify a register, change one panel setting at a time and
watch that sensor; the full history is in HA's logbook. Never write these registers.
The "Air pressure 2" (input 2202) entity was removed because that register doesn't exist.

Settings that changed since the 2026-10-08 scan: 601 User1 function 2 → 3 (Inlet → Exhaust),
612 User2 time 0 → 10 min, 1912 RH_LimLo 0 → 3000 (30 %), 550 boost countdown = 120.

## Wide sweep 0–65535 and protocol probes (2026-10-11 evening)

`ensy-ahu-fullscan.yaml` was extended. It's still read-only and still one register per request.

- **FC04 and FC03, 0–65535:** the same 160 registers as the 0–9999 sweep. Every other address
  returns exception 2. There were no timeouts and no other exception codes.
  **Holding 10000+ isn't an input mirror on this firmware**, despite what the manual says: it's illegal too.
- **FC01/FC02** (coils, discrete inputs), **FC07** (exception status), **FC11** (slave ID) and **FC2B/0E**
  (device identification): all return exception 1, illegal function.
- **FC14** (file record) and **FC18** (FIFO): no reply.
- **Other slave IDs 1–247:** nothing answers. The LMC311 is alone on COM1.

So COM1 (CN7 pins 2/3) exposes exactly these 160 registers. In HA history, changing Air quality
mode and Rotor max % on the panel changed none of them, so those settings aren't readable on COM1.
To be confirmed with the fullscan diff monitor, which watches all 160. The only remaining source is the panel
bus, COM2 (CN7 pins 4/5): sniff it listen-only while changing the settings.
