# 2-Week Plan (~1.5–2 h per evening)

## Week 4
**Day 1 — Learn + design (no wiring)**
- Analog vs digital: the Uno reads A0 as a number 0–1023.
- Voltage divider: LDR + 10 kΩ turns light into a voltage.
- PWM: `analogWrite` dims an LED by switching very fast.
- Build the circuit in Wokwi ([circuit_design.md](circuit_design.md)).

✅ Done when: simulation runs `02_street_lights`, `git push`.

**Day 2 — Light sensor**
1. Wire the LDR → upload `code/01_ldr_read` → Serial Monitor (9600).
2. Record bright / normal / covered values in `experiments/test_log.csv`.
3. Choose `DARK_ON` and `DARK_OFF` (about 100 apart).

**Day 3 — Street lights + motion**
1. Add the two lamp LEDs → `code/02_street_lights`.
2. Add the PIR → `code/03_motion_lights`. Tune the two PIR screws.

✅ Done when: lights work in a dark room, `git push`.

## Week 5
**Day 4 — Security state machine**
1. Draw DISARMED → ARMED → ALARM on paper first.
2. Add button, buzzer, red LED → `code/04_security_system`.
3. Test: arm, walk past after 10 s, check the siren stops after 20 s.

**Day 5 — Join the gate (first zone-to-zone link!)**
1. Plug Zone 2 parts into the gate's Uno (free pins only).
2. Copy your card UID into `code/05_gate_and_lights` → upload.
3. Test table: good card → welcome lights; 3 bad cards → ALARM.

**Day 6 — Cardboard + document**
1. Build the street and guard house ([../hardware/cardboard_demo.md](../hardware/cardboard_demo.md)).
2. Fill the test log, add a photo, write what you learned.

✅ Done when: gate + lights work together on the cardboard world, `git push`.
