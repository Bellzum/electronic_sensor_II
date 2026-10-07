# 3-Evening Plan (~1.5–2 h each, after work)

## Day 1 — Review sensors + design circuit (no soldering, low risk)
**Learn (30 min)**
- RFID RC522: reads a card's unique ID (UID) by radio at 13.56 MHz. Talks to Arduino over **SPI** (4 data wires: SCK, MOSI, MISO, SS). ⚠️ Runs on **3.3 V only**.
- 16x2 LCD with **I²C** backpack: only 2 data wires (SDA, SCL); address usually `0x27` (sometimes `0x3F`).
- SG90 servo: angle controlled by a **PWM** pulse (repeating on/off signal) on one pin.

**Design (60 min)**
- Draw the circuit in Tinkercad (Arduino Uno + servo + LCD I²C). Tinkercad has no RC522 — draw it as a labeled 8-pin header.
- Copy the pin map from `wiring.md` onto your drawing.
- Gather parts; check the list in `wiring.md`.

✅ Done when: circuit drawing saved, parts confirmed, `git push`.

## Day 2 — Build step by step, test each part alone
1. Install libraries in Arduino IDE: **MFRC522** (by GithubCommunity), **LiquidCrystal I2C** (by Frank de Brabander). Servo/SPI/Wire are built in.
2. Wire **RFID only** → upload `code/01_read_uid` → open Serial Monitor (9600) → tap cards → write UIDs into `experiments/test_log.csv`.
3. Add **LCD** → if blank, turn the blue contrast screw; if nothing, try address `0x3F`.
4. Add **servo** → watch it sweep 0°→87°→0°.

✅ Done when: each part works alone, UIDs recorded, `git push`.

## Day 3 — Full system + test + document
1. Paste your UID into `ALLOWED_UIDS` in `code/02_door_access/02_door_access.ino`, upload.
2. Run test table (10 taps each): allowed card, unknown card, fast repeated taps.
3. Log results in `experiments/test_log.csv`, take a photo/video of the build.
4. Reflection: what failed, what you learned, how it could join the robot dog dashboard.

✅ Done when: test log filled, photo added, `git push`.

## Safety notes
- Unplug USB before rewiring.
- RC522 VCC → **3.3V**, never 5V.
- Servo can draw spikes — if Arduino resets when servo moves, power servo from a separate 5V supply (share GND).
- Keep fingers out of the servo arm path.

## Stretch ideas (only if time)
- Green/red LED + buzzer for granted/denied.
- Log each scan over Serial as CSV → later read by Python/FastAPI.
