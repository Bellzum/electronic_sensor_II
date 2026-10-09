# Robot dog patrol: 4-week plan (weeks 18–21)

## ⛔ Before anything moves
- [ ] **Robot Shield version checked** ([freenove_code_map.md](freenove_code_map.md)); `ADS7830.py` fixed if v1.0–v1.9
- [ ] Battery fully charged, voltage checked with a multimeter
- [ ] First moving tests with the dog **on a stand, legs in the air**
- [ ] The dog works on the dashboard in **mock mode** first (`code/dog_mock.py`)
- [ ] World E-stop tested with the mock dog
- [ ] Clear floor area, no people or pets near, hand near the E-stop

## Week 18 — understand + mock
**Day 1** — Read the Freenove server code; fill in [freenove_code_map.md](freenove_code_map.md).
**Day 2** — Run `code/dog_mock.py` on your laptop. The dog card appears on the dashboard. Test: E-stop, timeout, low battery, obstacle.
**Day 3** — Make the collar ([../hardware/cardboard_demo.md](../hardware/cardboard_demo.md)), add its tag UID to the gate.

## Week 19 — real sensors, no movement
**Day 4** — Shield check + battery vs multimeter.
**Day 5** — `code/dog_sensors_readonly.py` on the dog's Pi: real battery, distance and tilt in the world log.

## Week 20 — first safe moves
**Day 6** — One safe command: **stand**, dog on its stand.
**Day 7** — Short moves (≤ 1 s) with timeout, obstacle stop and world E-stop. Start from `dog_mock.py` and replace "would do" with `Control.py` calls *only after* all mock tests pass.

## Week 21 — patrol
**Day 8** — Gate opens for the collar tag.
**Day 9** — Patrol: dog walks to the farm, Watchtower photographs it, everything in the log.
**Day 10** — Video + write-up. 🎉
