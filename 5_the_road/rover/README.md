# The Road · Part A · Delivery Rover 🚗

A small 2-wheel robot that drives the road between the Gate and the Farm. Your first robot that moves around. **Weeks 11–14.**

## What it does
You drive it from the dashboard. It refuses to drive into obstacles, stops if it tips over, and stops by itself if the commands stop coming. At the gate, its RFID tag opens the gate.

## Folder layout
```
docs/plan.md              4-week plan
docs/wiring.md            pin map + motor power rules
docs/circuit_design.md    Wokwi test with LEDs for motors
docs/figures/             wiring diagram
code/01_motor_test/       each wheel, both directions (wheels in the air!)
code/02_safe_drive/       timeout + speed limit + E-stop from Serial
code/03_obstacle_tilt/    + ultrasonic refusal + IMU tilt E-stop
code/04_mqtt_drive/       drive from the world (world/cmd/rover), report world/rover/state
hardware/                 cardboard shell + road tiles (1:1 template)
experiments/test_log.csv  test results
```

**New words:** motor driver · H-bridge · IMU · teleoperation · "no news = stop" (command timeout).

## Versions
| | Build | Code | Status |
|---|---|---|---|
| v1 | Each motor forward/back, **wheels off the ground** | `01_motor_test` | ☐ |
| v2 | Speed limit + 1 s timeout + E-stop button | `02_safe_drive` | ☐ |
| v3 | Refuse forward if obstacle < 20 cm | `03_obstacle_tilt` | ☐ |
| v4 | IMU: stop if tipped | `03_obstacle_tilt` | ☐ |
| v5 | Drive from the dashboard | `04_mqtt_drive` | ☐ |
| v6 | Obeys world E-stop; every drive logged | `04_mqtt_drive` | ☐ |
| v7 | **Gate handoff:** rover's tag opens the gate | gate allowed list | ☐ |
| v8 | *(stretch)* Line following "deliver to FARM" | — | ☐ |

## Checkpoint
- [ ] Wi‑Fi off → rover stops by itself within 1 s
- [ ] Gate handoff works 5 times in a row
- [ ] Every drive is in the world log
