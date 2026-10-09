# The Road · Part B · Robot Dog Patrol ⭐

The Freenove robot dog becomes the world's guardian: it walks the road, enters through the gate with its RFID collar, and reports to the Control Center. **Weeks 18–21.**

## Folder layout
```
docs/plan.md                    safety checklist + 4-week plan
docs/freenove_code_map.md       which Freenove file does what
code/dog_mock.py                pretend dog for the dashboard (nothing moves)
code/dog_sensors_readonly.py    real battery / distance / IMU → world (nothing moves)
hardware/                       RFID collar (1:1 template)
experiments/test_log.csv        test results
```

## Versions
| | Build | Code | Status |
|---|---|---|---|
| v1 | Read the Freenove server code; fill in the code map | `docs/freenove_code_map.md` | ☐ |
| v2 | Dog as a zone in **mock mode** | `code/dog_mock.py` | ☐ |
| v3 | Real readings: battery (vs multimeter), ultrasonic, IMU | `code/dog_sensors_readonly.py` | ☐ |
| v4 | One safe command: **stand** (on stand) | — | ☐ |
| v5 | Short moves ≤ 1 s with timeout, obstacle stop, world E-stop | — | ☐ |
| v6 | **Gate opens for the dog** (collar tag) | gate allowed list | ☐ |
| v7 | Patrol: dog → farm, Watchtower photo, all logged | — | ☐ |
| v8 | ROS 2: `freenove_driver_node` joins `world_safety_node` | *(Control Center v2)* | ☐ |

**Topics:** `world/cmd/dog` · `world/dog/state` · `world/dog/sensors` · `world/estop`

The larger Freenove Dog Lab Companion dashboard project lives in the `robot_science` repo; this folder is how the dog joins Mini Smart World.
