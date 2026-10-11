# Ensy ventilation in Home Assistant: user guide

This small board connects your Ensy AHU-400 ventilation unit to Home Assistant over
WiFi. You can then see temperatures and humidity, and control the fan speed, boost and
mode from your phone. The wall panel keeps working as before.

You only need to set it up once. Updates install by themselves.

---

## 1. What you need

- The board (LilyGO T-CAN485), already loaded with the firmware
- 4 thin wires, ideally a twisted pair for the two data wires
- A small flat screwdriver
- A phone with WiFi
- Your home WiFi name and password
- Home Assistant running at home

---

## 2. Wiring

> ⚠️ **Switch off the ventilation unit at its circuit breaker before you open it.**
> If you are not comfortable working inside the unit, ask whoever gave you the board
> to do this part. It takes about 10 minutes.

Inside the unit, find the controller board (LMC311). Next to its USB socket is a
6-pin connector marked **CN7**. Connect 4 of its pins to the board:

| CN7 pin | What it is | Connect to on the board |
|---|---|---|
| **1** | 12 V power | **VIN** (power in) |
| **2** | Data A | **A** (RS485 terminal) |
| **3** | Data B | **B** (RS485 terminal) |
| **6** | Ground | **GND** |

- **Leave pins 4 and 5 alone.** They go to the wall panel.
- Don't swap A and B. If you do, nothing breaks, but the LED stays **yellow**
  (see section 5).
- The board powers itself from pin 1, so it needs no USB cable or charger.
- Fix the board so it can't touch any metal or moving parts, for example in a small
  plastic box or with a cable tie. Keep it near the outer edge of the unit so the
  WiFi signal gets out.

Close the unit and switch the breaker back on.

---

## 3. Connect it to your WiFi

1. Within a minute of power-up, the LED on the board turns **red**: it doesn't know
   your WiFi yet.
2. On your phone, open WiFi settings and join the network **`Ensy AHU setup`**.
   Password: **`ensy-ahu-setup`**
3. A setup page opens by itself. If it doesn't, open a browser and go to
   **`http://192.168.4.1`**.
4. Pick **your home WiFi** from the list, type its password and press **Save**.
5. The `Ensy AHU setup` network disappears and the LED turns **blue**. The board is
   now on your WiFi.

> The board only works with 2.4 GHz WiFi. Most routers have it, but if your WiFi
> doesn't appear in the list, check that 2.4 GHz is switched on in your router.

---

## 4. Add it to Home Assistant

1. In Home Assistant, go to **Settings → Devices & services**.
2. Under **Discovered**, you'll see **Ensy AHU-400**. Click **Add** (or **Configure**).
3. Confirm. Home Assistant sets up a secure connection by itself; you don't need to
   type any password or key.
4. The LED turns **green**. You're done.

> **Do this within 24 hours of switching the unit on.** For security, the board stops
> accepting new connections after that. If you missed it, switch the ventilation unit
> off at the breaker for 10 seconds and on again, then repeat these steps.

---

## 5. What the LED means

| LED | Meaning | What to do |
|---|---|---|
| 🟢 **Green** | Everything works | Nothing |
| 🔵 **Blue** | On WiFi, but Home Assistant isn't connected | Normal for a moment after start. If it stays blue, check that Home Assistant is running, or add the device (section 4). |
| 🟡 **Yellow** | Home Assistant is fine, but the ventilation unit doesn't answer | Check the wires on CN7. A and B may be swapped. |
| 🔴 **Red** | No WiFi | First setup: see section 3. Later: the router may be off or its password changed. The `Ensy AHU setup` network comes back so you can enter the new password. |
| ⚫ **Off** | No power | Check pin 1 (12 V) and pin 6 (GND), and that the unit is switched on. |

---

## 6. What you get in Home Assistant

- **Ventilation**: the main control. Turn it off or on, pick the mode, set the
  temperature, and set the fan to Low / Medium / High.
- **Boost**: runs the fans at full speed for a while (2 hours by default), then
  returns to normal.
- **Cooker hood**: a shorter boost for cooking (20 minutes by default).
- **Temperatures**: outside air, supply air, room air and exhaust air.
- **Humidity**
- **Alarms**: for example **Filter alarm**, which means it's time to change the filters.
  **Reset alarms** clears them after you've fixed the cause.
- **Firmware**: shows the installed version. Updates install by themselves.

The wall panel keeps working. Whatever you change on either the panel or Home
Assistant shows up on the other.

---

## 7. Updates

You don't need to do anything. The board checks for new firmware a few times a day
and installs it. During an update the LED may go dark or red for about a minute,
which is normal. The ventilation keeps running during the update; only Home
Assistant loses contact for that minute.

---

## 8. If something goes wrong

| Problem | Try this |
|---|---|
| Home Assistant shows the device as "unavailable" | Look at the LED and see section 5. |
| You changed your router or WiFi password | Wait a few minutes until the LED is **red**, then repeat section 3. If `Ensy AHU setup` doesn't appear, switch the unit off and on at the breaker. |
| "Ensy AHU-400" never appears in Home Assistant | Make sure your phone or computer is on the same WiFi, wait a few minutes, then switch the unit off and on at the breaker. |
| Anything else | Contact whoever gave you the board. It can be updated remotely, so most problems can be fixed without a visit. |

You can't break the ventilation unit with this board. If you unplug the board, the unit
simply works as it did before, from the wall panel.
