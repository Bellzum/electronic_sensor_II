# Zone 1 · Smart Gate 🚪

The entrance to the world: a cardboard gate with a servo lock that opens for the right RFID card. **Weeks 1–3.**

**Built on:** Arduino Uno (your current build in `code/`). Zone 2 adds its parts to the free pins of the same Uno, and the Pi's `serial_bridge.py` brings it into the Wi‑Fi world.

**Parts:** Arduino Uno, RC522 RFID reader + cards/tags, SG90 servo, 16x2 LCD (I²C). Later versions add a reed switch + magnet, active buzzer, red + green LEDs, 220 Ω resistors, 2 push buttons (emergency + reset).

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
| v7 | *(Zone 3)* Into the world: `6_control_center/hub/serial_bridge.py` turns the Uno's Serial CSV into `world/gate/state` + `world/gate/card` | ☐ |

## State machine

```text
LOCKED   --good card-->      UNLOCKED (start 5 s timer)
LOCKED   --door opens-->     ALARM (forced!)
UNLOCKED --5 s, still shut-> LOCKED
UNLOCKED --door opens-->     OPEN
OPEN     --door closes-->    LOCKED
ANY      --emergency btn-->  EMERGENCY (latched until reset)
```

## Wiring
See [docs/wiring.md](docs/wiring.md) (Arduino Uno pin map) and [docs/figures/wiring_diagram.png](docs/figures/wiring_diagram.png).

## Checkpoint

- [ ] I drew the state diagram before coding
- [ ] Emergency beats every state
- [ ] Servo has its own 5 V supply
- [ ] I use `millis()`, not long `delay()`

**Links to the world:** Zone 2 lights react to gate events · Zone 4 camera turns here on a card scan · Zone 5 rover and dog carry their own RFID tags.
