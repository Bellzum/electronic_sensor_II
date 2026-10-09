# 2-Week Plan (~1.5–2 h per evening)

## Week 9 — the tower moves
**Day 1** — Learn: servos and PWM, OpenCV basics, FPS. Simulate in Wokwi ([circuit_design.md](circuit_design.md)).
**Day 2** — Build the pan/tilt bracket, wire it, run `code/01_servo_limits`. Find your safe limits and write them down.
**Day 3** — Aim at the gate, farm and road by hand; write the angles into `PRESETS` in `code/02_pan_tilt_mqtt`. Upload it and send `GATE` from the Pi:
`mosquitto_pub -t world/cmd/tower -m GATE`

✅ Done when: presets work from the Pi, `git push`.

## Week 10 — the tower sees
**Day 4** — Camera on the Pi: `pi/tower_camera.py`. Tap a card at the gate → tower turns → photo in `pi/photos/`.
**Day 5** — `pi/ball_tracker.py --show`. Measure your FPS and write it in `experiments/test_log.csv`. Tune `GAIN`: start small, double it until it wobbles, then halve it.
**Day 6** — Cardboard tower ([../hardware/cardboard_demo.md](../hardware/cardboard_demo.md)) + photo gallery on the dashboard.

✅ Done when: card scan → photo in the log, E-stop freezes the tower, `git push`.
