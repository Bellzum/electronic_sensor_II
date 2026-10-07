# Zone 1 · Smart Gate 🚪

The entrance to the world: a cardboard gate with a servo lock that opens for the right RFID card. **Weeks 1–3.**

**Parts:** ESP32, RC522 RFID reader + cards/tags, SG90 servo, reed switch + magnet, active buzzer, red + green LEDs, 220 Ω resistors, 2 push buttons (emergency + reset).

**New words:** GPIO (a pin you switch or read) · RFID (card with a unique ID read by radio) · SPI (fast 4-wire chip connection) · servo (motor that goes to an angle) · PWM (fast on/off switching) · state machine · latch.

## Versions

| | Build | Status |
|---|---|---|
| v1 | LEDs + button (blink, button → LED) | ☐ |
| v2 | RFID reads card ID and prints it | ☐ |
| v3 | Allowed-card list → green + short beep / red + long beep | ☐ |
| v4 | Servo unlocks 5 s, then relocks by itself | ☐ |
| v5 | Reed switch: knows if the door is open; ALARM if opened while locked | ☐ |
| v6 | Emergency button: latched EMERGENCY, hold reset 2 s | ☐ |
| v7 | *(Zone 3)* Publish `world/gate/state` + `world/gate/card` over MQTT; obey `world/estop` | ☐ |

## State machine

```text
LOCKED   --good card-->      UNLOCKED (start 5 s timer)
LOCKED   --door opens-->     ALARM (forced!)
UNLOCKED --5 s, still shut-> LOCKED
UNLOCKED --door opens-->     OPEN
OPEN     --door closes-->    LOCKED
ANY      --emergency btn-->  EMERGENCY (latched until reset)
```

## Wiring (RC522 → ESP32, 3.3 V module)

| RC522 | ESP32 |
|---|---|
| SDA (SS) | GPIO 5 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| RST | GPIO 22 |
| 3.3V | 3.3V (**not 5 V**) |
| GND | GND |

Servo signal → GPIO 13, servo power from a separate 5 V supply with shared GND.

## Checkpoint

- [ ] I drew the state diagram before coding
- [ ] Emergency beats every state
- [ ] Servo has its own 5 V supply
- [ ] I use `millis()`, not long `delay()`

**Links to the world:** Zone 2 lights react to gate events · Zone 4 camera turns here on a card scan · Zone 5 rover and dog carry their own RFID tags.
