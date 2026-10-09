# Wiring (Arduino Uno, shared with the Smart Gate)

Zone 2 uses the **free pins** of the gate's Arduino Uno, so in v5 one board runs both zones.

![Wiring diagram](figures/wiring_diagram.svg)

## Parts
LDR (light-dependent resistor) · 10 kΩ resistor · PIR motion sensor HC-SR501 · 2 white/yellow LEDs (street lamps) · 1 red LED · 3 × 220 Ω resistors · active buzzer · push button · jumper wires

## Pin map
| Part | Pin on part | Uno |
|---|---|---|
| LDR | one leg | 5V |
| LDR + 10 kΩ | middle point | **A0** |
| 10 kΩ | other leg | GND |
| PIR | VCC / OUT / GND | 5V / **D2** / GND |
| ARM button | one leg / other leg | **D4** / GND (uses INPUT_PULLUP, no resistor needed) |
| Street LED 1 | long leg via 220 Ω | **D5** (PWM ~) |
| Street LED 2 | long leg via 220 Ω | **D6** (PWM ~) |
| Active buzzer | + / − | **D7** / GND |
| Red alarm LED | long leg via 220 Ω | **D8** |

Already used by the gate (don't touch): D3 servo · D9 RST · D10 SS · D11 MOSI · D12 MISO · D13 SCK · A4/A5 LCD.

## Sensor review cheat-sheet
| Part | Signal type | Simple meaning |
|---|---|---|
| LDR + resistor | Analog (0–1023) | A **voltage divider**: more light = higher voltage on A0 |
| PIR HC-SR501 | Digital (HIGH/LOW) | Sees body heat moving. HIGH for a few seconds after motion |
| Street LEDs | PWM output | `analogWrite(pin, 0–255)` fakes dimming by fast on/off |
| Active buzzer | Digital output | HIGH = beep (it makes its own tone) |

## PIR tuning (the two orange screws)
- **Sx (sensitivity):** turn to the middle first.
- **Tx (time):** turn fully anticlockwise for the shortest HIGH time (~3 s).
- Jumper: **H** (repeat trigger) if your board has one.
- It needs about **60 s to warm up** after power on. The code ignores it until then.
