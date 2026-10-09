# Zone 2 · Lights & Security 💡

Street lights and a security system that react to the Smart Gate. **Weeks 4–5.**

Runs on the **same Arduino Uno as the gate**, using its free pins. In Zone 3 it joins the Wi‑Fi world.

## What it does
It gets dark → the street lights glow. Someone walks by → they go bright. A good card at the gate → welcome lights. Three wrong cards, or motion while the system is armed → siren and flashing red light.

## Folder layout
```
docs/plan.md               2-week plan + checklists
docs/wiring.md             pin map + sensor cheat-sheet
docs/circuit_design.md     build and test it in Wokwi first
docs/figures/              wiring diagram
code/01_ldr_read/          read the light sensor
code/02_street_lights/     auto lights with hysteresis + PWM dimming
code/03_motion_lights/     PIR makes the lights bright
code/04_security_system/   DISARMED / ARMED / ALARM state machine
code/05_gate_and_lights/   Zone 1 + Zone 2 together on one Uno
hardware/                  cardboard street + guard house (1:1 template)
experiments/test_log.csv   test results
```

**New words:** analog input (a range of values, 0–1023 on the Uno) · voltage divider · threshold · hysteresis (two thresholds so lights don't flicker at dusk) · PWM · latch.

## Versions
| | Build | Code | Status |
|---|---|---|---|
| v1 | Read the light sensor; measure bright vs dark | `01_ldr_read` | ☐ |
| v2 | Street lights auto-on when dark, dimmed with PWM | `02_street_lights` | ☐ |
| v3 | PIR: lights brighten on motion | `03_motion_lights` | ☐ |
| v4 | Security: DISARMED / ARMED / ALARM, siren time limit | `04_security_system` | ☐ |
| v5 | **Gate link:** good card → welcome lights; 3 bad cards → ALARM | `05_gate_and_lights` | ☐ |
| v6 | *(Zone 3)* Publish `world/lights/state`, `world/security/alarm`; obey `world/estop` | — | ☐ |

## Checkpoint
- [ ] Hysteresis stops flicker
- [ ] Gate events change the lights
- [ ] The siren can never beep forever
- [ ] Every event prints a CSV line on Serial

## Link to the robot dog
The dog's obstacle warning uses the same threshold + hysteresis idea, and its E-stop is the same latched state as ALARM.
