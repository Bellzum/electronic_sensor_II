# Zone 6 · Control Center 🖥️

The world's brain: a Raspberry Pi hub that collects every zone's messages, saves one log, serves the dashboard and owns the **world E-stop**.

```text
6_control_center/
├── hub/          ← MQTT broker setup + world_logger.py
├── dashboard/    ← FastAPI backend + web page (world map, zone cards, log, E-stop)
├── ros2/         ← ROS 2 nodes (v2)
└── simulation/   ← Gazebo twin of the road (v2)
```

## v1 · Hub + dashboard (weeks 6–8, with the Smart Farm)

| | Build | Status |
|---|---|---|
| v1 | Pi set up: SSH, Python venv, Mosquitto MQTT broker | ☐ |
| v2 | `hub/world_logger.py`: subscribe to `world/#`, save to `world_log.csv` | ☐ |
| v3 | Gate + Lights + Farm all publish to the hub | ☐ |
| v4 | Dashboard: live zone cards, log table, sensor chart | ☐ |
| v5 | **World E-stop button** → publishes `world/estop` | ☐ |

## v2 · ROS 2 + Gazebo (weeks 15–17)

| | Build | Status |
|---|---|---|
| v1 | ROS 2 tutorials: talker/listener, `ros2 topic echo`, `rqt_graph` | ☐ |
| v2 | `mqtt_bridge_node`: `world/*` ↔ ROS topics | ☐ |
| v3 | `world_safety_node`: owns `/estop`, offers `/reset_estop` | ☐ |
| v4 | Rover in Gazebo on a copy of the road | ☐ |
| v5 | Dashboard via rosbridge | ☐ |
| v6 | `ros2 bag record` a full day in the world and replay it | ☐ |

```text
[ESP32 zones] --MQTT--> [mqtt_bridge_node] --> /gate/state /farm/sensors /rover/state ...
[dashboard] --rosbridge--> /cmd/* --> [world_safety_node] --> /safe_cmd/* --> zones
                                          |
                               [world_logger_node]  [glasses_status_node (future)]
```

Pi 2 note: fine for v1 (add a USB Wi‑Fi dongle or Ethernet). For ROS 2 and vision, a Pi 4/5 or a laptop with Ubuntu is much easier.
