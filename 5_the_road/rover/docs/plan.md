# 4-Week Plan (~1.5–2 h per evening)

## Week 11 — motors
**Day 1** — Learn: motor driver, H-bridge, PWM speed. Build the chassis kit.
**Day 2** — Wire motors + driver. **Wheels in the air.** `code/01_motor_test`. Swap wires until every direction is right.
**Day 3** — `code/02_safe_drive`: test the 1 s timeout, the speed limit and the E-stop.

## Week 12 — sensors that say NO
**Day 4** — Wire HC-SR04 (with divider) + MPU6050 → simulate first in Wokwi.
**Day 5** — `code/03_obstacle_tilt`: test every row in `experiments/test_log.csv`.
**Day 6** — First drive on the floor: clear area, slow speed, hand near the E-stop.

## Week 13 — the rover joins the world
**Day 7** — `code/04_mqtt_drive`. Drive from the Pi: `mosquitto_pub -t world/cmd/rover -m forward` (note: it stops after 1 s).
**Day 8** — Drive from the dashboard (hold-to-drive buttons).
**Day 9** — Test: unplug the Pi's Wi‑Fi while driving → rover stops within 1 s.

## Week 14 — gate handoff
**Day 10** — Stick an RFID tag under the shell, add its UID to the gate's allowed list.
**Day 11** — Gate handoff: drive to the gate, reader sees the tag, gate opens, rover drives through. Repeat 5 times.
**Day 12** — Cardboard shell + road tiles ([../hardware/cardboard_demo.md](../hardware/cardboard_demo.md)), video, `git push`.

✅ Done when: Wi‑Fi off → stop, gate handoff works 5 times in a row, every drive in the world log.
