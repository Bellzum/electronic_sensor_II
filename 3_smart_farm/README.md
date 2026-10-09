# Zone 3 · Smart Farm 🌱

A tiny greenhouse with a plant, sensors, a fan and a mini pump. **Weeks 6–8.** This is when the world goes online: the Raspberry Pi hub ([`../6_control_center`](../6_control_center/)) starts at the same time.

## What it does
The farm measures temperature, humidity and soil moisture every 5 seconds and sends them over Wi‑Fi. It turns the fan on when it's hot and waters the plant for 3 seconds when the soil is dry, never more often than every 20 minutes. The world E-stop stops everything.

## Folder layout
```
docs/plan.md                3-week plan + checklists
docs/wiring.md              pin map + water safety
docs/circuit_design.md      build and test it in Wokwi (even MQTT)
docs/figures/               wiring diagram
code/01_read_sensors/       DHT22 + soil sensor
code/02_soil_calibration/   the calibration experiment
code/03_mqtt_publish/       send data to the world (Wi-Fi + MQTT)
code/04_farm_control/       fan + pump with limits, heartbeat and E-stop
hardware/                   cardboard greenhouse (1:1 template)
experiments/                test log + soil calibration data
```

**New words:** calibration · noise / filtering · MQTT (Wi‑Fi messages on named topics) · heartbeat (an "I'm alive" message) · MOSFET (an electronic switch).

## Versions
| | Build | Code | Status |
|---|---|---|---|
| v1 | Read temp, humidity, soil every 2 s | `01_read_sensors` | ☐ |
| v2 | **Calibration experiment:** dry vs wet, 20 readings each | `02_soil_calibration` | ☐ |
| v3 | Publish `world/farm/sensors` over Wi‑Fi | `03_mqtt_publish` | ☐ |
| v4 | Fan with hysteresis; pump 3 s max, 20 min gap; heartbeat; E-stop | `04_farm_control` | ☐ |
| v5 | **Gate link:** only an allowed RFID card can switch the farm to manual mode | *(Control Center)* | ☐ |

## Checkpoint
- [ ] Calibration CSV + written conclusion
- [ ] The pump can never run forever
- [ ] Farm data appears on the Control Center dashboard
- [ ] `secrets.h` is NOT on GitHub (check the repo page)

## Link to the robot dog
Calibration is how you'll check the dog's battery and distance readings. The heartbeat rule becomes the dog's "no message = stop" timeout.
