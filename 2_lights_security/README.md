# Zone 2 · Lights & Security 💡

Street lights and a security system, connected to the gate. **Weeks 4–5.**

**New parts:** LDR light sensor + 10 kΩ, PIR motion sensor, extra LEDs, buzzer/siren.

**New words:** analog input / ADC (a range of values, 0–4095) · threshold · hysteresis (two thresholds so lights don't flicker at dusk).

| | Build | Status |
|---|---|---|
| v1 | Print LDR readings; measure light vs dark | ☐ |
| v2 | Street lights auto-on when dark, dimmed with PWM | ☐ |
| v3 | PIR: lights brighten on motion | ☐ |
| v4 | **Gate link:** good card → welcome lights 30 s | ☐ |
| v5 | **Security mode:** gate ALARM or motion while ARMED → siren + flashing red | ☐ |
| v6 | *(Zone 3)* Publish `world/lights/state`, `world/security/alarm`; obey `world/estop` | ☐ |

Before Zone 3, run this on the gate's ESP32 or a second ESP32 wired by serial.

## Checkpoint
- [ ] Hysteresis stops flicker
- [ ] Gate events change the lights
- [ ] E-stop also silences the siren
