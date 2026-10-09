# Zone 4 · Watchtower 📷

A pan/tilt camera tower that watches the world and reacts to the other zones. **Weeks 9–10.**

## What it does
The tower turns to preset positions (GATE, FARM, ROAD). When someone taps a card at the gate or the alarm goes off, it turns to the gate and the Pi saves a photo. It can also follow a red ball by itself. The world E-stop freezes it.

## Folder layout
```
docs/plan.md               2-week plan + checklists
docs/wiring.md             pin map + servo power rules
docs/circuit_design.md     Wokwi + testing the camera on a laptop
docs/figures/              wiring diagram
code/01_servo_limits/      move servos slowly, never past the limits
code/02_pan_tilt_mqtt/     presets + listens to gate, alarm and E-stop
pi/tower_camera.py         photo on every gate event -> world/tower/event
pi/ball_tracker.py         OpenCV red-ball tracking (proportional control)
hardware/                  cardboard tower (1:1 template)
experiments/test_log.csv   test results
```

**New words:** OpenCV · frame / FPS · HSV colour · proportional control · dead zone.

## Versions
| | Build | Code | Status |
|---|---|---|---|
| v1 | Pan/tilt from Serial, with angle limits and slow moves | `01_servo_limits` | ☐ |
| v2 | Preset positions GATE / FARM / ROAD over MQTT | `02_pan_tilt_mqtt` | ☐ |
| v3 | **Event links:** card scan / alarm → aim at GATE | `02_pan_tilt_mqtt` | ☐ |
| v4 | Photo on every event, published to `world/tower/event` | `pi/tower_camera.py` | ☐ |
| v5 | OpenCV ball tracking | `pi/ball_tracker.py` | ☐ |
| v6 | Photo gallery on the dashboard | *(Control Center)* | ☐ |

## Checkpoint
- [ ] I know my camera FPS
- [ ] Servo limits protect the bracket
- [ ] E-stop freezes the tower

## Link to the robot dog
The dog's head camera works the same way. Ball tracking is the first step toward "follow me".
A Pi 4/5 makes vision much smoother than a Pi 2.
