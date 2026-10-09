# 3-Week Plan (~1.5–2 h per evening)

This zone runs **together with the Control Center v1** ([../../6_control_center](../../6_control_center/)). Farm on the ESP32, hub on the Raspberry Pi.

## Week 6 — sensors + science
**Day 1** — Learn: analog on the ESP32 (0–4095), DHT22, calibration. Build it in Wokwi ([circuit_design.md](circuit_design.md)).
**Day 2** — Wire the DHT22 + soil sensor → `code/01_read_sensors`.
**Day 3** — **Calibration experiment** with `code/02_soil_calibration`: 20 dry + 20 wet readings → `experiments/soil_calibration.csv`. Write 3 sentences: average dry, average wet, how much the readings jump around.

✅ Done when: calibration CSV + conclusion pushed.

## Week 7 — the world goes online
**Day 4** — Raspberry Pi: install Mosquitto (see `6_control_center/hub/README.md`).
**Day 5** — `code/03_mqtt_publish`: farm data appears in `mosquitto_sub -t 'world/#' -v` on the Pi.
**Day 6** — Start `world_logger.py` and the gate serial bridge on the Pi: gate + lights + farm in ONE log.

✅ Done when: `world_log.csv` has lines from Zone 1, 2 and 3.

## Week 8 — control with limits
**Day 7** — Wire fan + pump through MOSFET modules (separate 5 V).
**Day 8** — `code/04_farm_control`: test every row in `experiments/test_log.csv`, especially the E-stop and the 20 min pump gap.
**Day 9** — Cardboard greenhouse ([../hardware/cardboard_demo.md](../hardware/cardboard_demo.md)) + dashboard shows the farm.

✅ Done when: dashboard E-stop stops the pump, test log filled, `git push`.
