# Control Center v2 · ROS 2 (weeks 15–17)

Don't start this until Zones 1–5 work with MQTT. ROS 2 doesn't replace the world. It **wraps** it.

## Words first
- **ROS 2:** a standard framework for robot software.
- **Node:** one program with one job. Each zone becomes a node.
- **Topic:** a named channel, like an MQTT topic. Nodes publish and subscribe.
- **Service:** request → reply (e.g. `/reset_estop`).
- **Launch file:** starts many nodes at once.
- **rosbridge:** lets a web page talk to ROS over WebSocket.

## The plan
```text
[ESP32 zones] --MQTT--> [mqtt_bridge_node] --> /gate/state /farm/sensors /rover/state ...
[dashboard] --rosbridge--> /cmd/rover --> [world_safety_node] --> /safe_cmd/rover --> mqtt_bridge --> rover
                                                |
                                     [world_logger_node]   [glasses_status_node (future)]
```

| Node | Subscribes | Publishes | Job |
|---|---|---|---|
| `mqtt_bridge_node` | MQTT `world/#`, ROS `/safe_cmd/*`, `/estop` | ROS `/gate/state`, `/farm/sensors`, … ; MQTT `world/cmd/*` | Translate between the two worlds |
| `world_safety_node` | `/cmd/*`, `/rover/state`, `/estop` | `/safe_cmd/*`, `/safety_state` | Only safe commands get through. Owns `/reset_estop` |
| `world_logger_node` | everything | — | `ros2 bag record` does most of this |
| `freenove_driver_node` | `/safe_cmd/dog` | `/dog/state`, `/dog/sensors` | The dog joins (Zone 5 part B, v8) |

## Setup
ROS 2 needs Ubuntu: a Pi 4/5 with Ubuntu, a laptop with Ubuntu, or Docker on your Mac. Use the **current ROS 2 LTS** release and follow its official install guide.

```bash
mkdir -p ~/msw_ws/src && cd ~/msw_ws/src
ros2 pkg create --build-type ament_python mini_smart_world
cp ~/mini_smart_world/6_control_center/ros2/mqtt_bridge_node.py mini_smart_world/mini_smart_world/
# add the entry point in setup.py:  'mqtt_bridge = mini_smart_world.mqtt_bridge_node:main'
cd ~/msw_ws && colcon build && source install/setup.bash
ros2 run mini_smart_world mqtt_bridge
ros2 topic list          # /farm/sensors, /rover/state, ...
ros2 topic echo /farm/sensors
```

## Versions
| | Build | Status |
|---|---|---|
| v1 | ROS 2 tutorials: talker/listener, `ros2 topic echo`, `rqt_graph` | ☐ |
| v2 | `mqtt_bridge_node`: world topics appear in ROS | ☐ |
| v3 | `world_safety_node` with `/reset_estop` service | ☐ |
| v4 | Rover in Gazebo ([../simulation](../simulation/)) | ☐ |
| v5 | Dashboard via rosbridge | ☐ |
| v6 | `ros2 bag record` a full day in the world, replay it | ☐ |
