# 🌍 Mini Smart World

A tabletop robotics world, built one zone at a time. Every zone is a small finished project that **plugs into the zones built before it**. At the end, the whole world runs together and a Freenove robot dog patrols it.

![Mini Smart World map](assets/world-map.svg)

## Zones

| # | Zone | Folder | Weeks | What you learn | Connects to |
|---|---|---|---|---|---|
| 1 | **Smart Gate** (RFID door) | [`1_smart_gate`](1_smart_gate/) | 1–3 | Pins, RFID, servo, buzzer, state machine, E-stop | The start |
| 2 | **Lights & Security** | [`2_lights_security`](2_lights_security/) | 4–5 | Light + motion sensors, PWM, alarms | Good card → welcome lights; forced gate → alarm |
| 3 | **Smart Farm** | [`3_smart_farm`](3_smart_farm/) | 6–8 | Temp/soil sensors, calibration, fan/pump | First zone on Wi‑Fi; reports to the Control Center |
| 4 | **Watchtower** | [`4_watchtower`](4_watchtower/) | 9–10 | Pan/tilt servos, camera, OpenCV | Turns to the gate on card scan or alarm, saves a photo |
| 5 | **The Road** (rover + dog) | [`5_the_road`](5_the_road/) | 11–14, 18–21 | Motors, IMU, obstacle stop, remote driving | Gate opens for the rover's and dog's RFID tags |
| 6 | **Control Center** | [`6_control_center`](6_control_center/) | 6–8, 15–17 | Raspberry Pi hub, MQTT, dashboard, logs, ROS 2, Gazebo | Every zone reports here; one world E-stop |

Shared code (message format, MQTT helpers) lives in [`common/`](common/).

## Build progress

- [ ] Zone 1 · Smart Gate
- [ ] Zone 2 · Lights & Security
- [ ] Zone 3 · Smart Farm + Control Center v1 (hub, dashboard, log)
- [ ] Zone 4 · Watchtower
- [ ] Zone 5 · Delivery Rover on the road
- [ ] Zone 6 · Control Center v2 (ROS 2 + Gazebo twin)
- [ ] ⭐ Robot dog patrol

## How the zones talk

Zones 3–5 run on an **ESP32** (a small Wi‑Fi microcontroller). The gate and lights share an **Arduino Uno** that talks to the hub over USB. The **Raspberry Pi** in the Control Center is the hub. Zones send messages with **MQTT**: a device *publishes* to a named *topic* and anyone *subscribed* to it receives the message (ROS 2 uses the same idea).

```text
world/gate/state        LOCKED | UNLOCKED | OPEN | ALARM | EMERGENCY
world/gate/card         {"uid":"A1B2C3D4","allowed":true}
world/lights/state      ON | OFF | AUTO
world/security/alarm    true | false
world/farm/sensors      {"temp":24.1,"humidity":55,"soil":38}
world/farm/pump         ON | OFF
world/tower/event       {"photo":"...jpg","reason":"card_scan"}
world/rover/state       IDLE | DRIVING | BLOCKED | ESTOP
world/dog/state         IDLE | STANDING | MOVING | ESTOP
world/estop             true | false     <- ONE emergency stop for the whole world
world/cmd/<zone>        commands from the dashboard
```

Until Zone 3, zones run alone. After that, the Uno reaches MQTT through `6_control_center/hub/serial_bridge.py`.

## Safety rules (every zone)

1. **World E-stop wins.** `world/estop = true` → every zone goes to its safe state.
2. **Silence means stop.** A zone that stops hearing from the hub goes safe on its own.
3. **Limits.** Max servo angle, motor speed and pump time.
4. **Log everything** with a timestamp.
5. **Manual first**, automatic later.

Hardware: ESP32 and Pi pins are **3.3 V**, never 5 V. Servos and motors get their own power supply (shared GND). Unplug before rewiring.

## Repo layout

```text
mini_smart_world/
├── README.md            ← you are here
├── assets/              ← world map, photos, wiring diagrams
├── common/              ← shared message format + helpers
├── 1_smart_gate/
├── 2_lights_security/
├── 3_smart_farm/
├── 4_watchtower/
├── 5_the_road/
└── 6_control_center/
```

Every zone follows the same pattern as the Smart Gate:

```text
<zone>/
├── README.md          ← what it does, versions (v1, v2, …) with checkboxes, checkpoint
├── docs/
│   ├── plan.md        ← evening-by-evening plan
│   ├── wiring.md      ← parts list + pin map + sensor cheat-sheet
│   ├── circuit_design.md ← build and test it in Wokwi / Tinkercad first
│   └── figures/       ← wiring diagram
├── code/01_.../       ← one Arduino sketch per version (folder name = sketch name)
├── pi/                ← Python for the Raspberry Pi (Watchtower)
├── hardware/          ← cardboard model: 1:1 cutting template (SVG) + build guide
└── experiments/       ← test_log.csv: fill in a row for every test
```

| Zone | Board | Code |
|---|---|---|
| 1 Smart Gate | Arduino Uno | RFID, LCD, servo |
| 2 Lights & Security | same Uno (free pins) | LDR, PIR, PWM lamps, alarm, gate + lights together |
| 3 Smart Farm | ESP32 | DHT22, soil calibration, MQTT, fan/pump with limits |
| 4 Watchtower | ESP32 + Pi camera | pan/tilt presets, event photos, OpenCV ball tracking |
| 5 The Road | ESP32 rover + Freenove dog | safe driving (timeout, obstacle, tilt), mock dog, read-only dog sensors |
| 6 Control Center | Raspberry Pi | MQTT hub, world log, heartbeat, dashboard, mock world, ROS 2 starter |

**Try it with no hardware:** [6_control_center](6_control_center/) has a mock world and the dashboard. Run them on your laptop.

**Wi‑Fi passwords:** ESP32 sketches read them from `secrets.h` (copy `secrets_example.h`). `secrets.h` is in `.gitignore`, so it never reaches GitHub.

Commit after every version: `git add . && git commit -m "gate v3: allowed card list" && git push`

## Related

- Freenove Dog Lab Companion (web dashboard + backend for the robot dog), joins as the final zone.
