# Zone 5 · The Road 🛣️

The road connects the Gate and the Farm. Two robots drive it: the **Delivery Rover** (weeks 11–14) and, at the end, the **Freenove robot dog** (weeks 18–21).

```text
5_the_road/
├── rover/   ← 2-wheel delivery rover (ESP32 firmware)
└── dog/     ← Freenove robot dog integration
```

## Part A · Delivery Rover

**New parts:** 2WD chassis, TB6612FNG (or L298N) motor driver, separate motor battery, MPU6050 IMU, HC-SR04 ultrasonic (ECHO through a 1 kΩ/2 kΩ divider), ESP32, RFID tag.

**New words:** motor driver · H-bridge · IMU · encoder · teleoperation · line following.

| | Build | Status |
|---|---|---|
| v1 | Forward/back/turn by Serial, **wheels off the ground** | ☐ |
| v2 | Speed limit + **1 s command timeout → stop** + E-stop button | ☐ |
| v3 | Refuse forward if obstacle < 20 cm | ☐ |
| v4 | IMU: stop if tipped | ☐ |
| v5 | Drive from the dashboard (`world/cmd/rover`) | ☐ |
| v6 | Obeys `world/estop`; logs commands with battery, distance, tilt | ☐ |
| v7 | **Gate handoff:** rover's tag opens the gate, rover drives through | ☐ |
| v8 | *(stretch)* Line following: "deliver to FARM" | ☐ |

## Part B · Robot dog patrol ⭐

**Before anything moves**
- [ ] Check the Robot Shield version. v2.0+: no change. v1.0–v1.9: in `Code/Server/ADS7830.py` change `data[4] / 255.0 * 5.0 * 2` → `data[4] / 255.0 * 5.0 * 3`
- [ ] Full battery; first tests on a stand, legs in the air
- [ ] Dog on the dashboard in **mock mode** first

| | Build | Status |
|---|---|---|
| v1 | Read the Freenove server code; map which file does what | ☐ |
| v2 | Dog as a zone in mock mode: `world/dog/state`, battery, distance | ☐ |
| v3 | Real readings: battery (check with multimeter), ultrasonic, IMU | ☐ |
| v4 | One safe command: **stand** (on stand) | ☐ |
| v5 | Short moves ≤ 1 s with timeout, obstacle stop, obeys world E-stop | ☐ |
| v6 | **Gate opens for the dog** (RFID tag on its collar) | ☐ |
| v7 | Patrol: dog walks to the farm, tower photographs the plant, all logged | ☐ |
| v8 | ROS 2: `freenove_driver_node` joins `world_safety_node` | ☐ |

## Checkpoint
- [ ] Wi‑Fi off → robot stops by itself within 1 s
- [ ] Gate handoff works 5 times in a row
- [ ] Every drive is in the world log
