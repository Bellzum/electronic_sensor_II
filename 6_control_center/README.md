# Zone 6 · Control Center 🖥️

The world's brain: a Raspberry Pi hub that collects every zone's messages, keeps one log, serves the dashboard and owns the **world E-stop**.

## What it does
Every zone sends its news to the hub over MQTT. The hub saves it all in one CSV log, sends a heartbeat every second, and serves a dashboard you open on your phone: live zone cards, farm and tower buttons, hold-to-drive pads for the rover and dog, and one big E-stop.

**Start in mock mode:** `hub/fake_world.py` pretends to be every zone, so the dashboard works before any hardware exists.

## Folder layout
```
requirements.txt            Python packages for the hub + dashboard
hub/README.md               Pi setup + how to run everything
hub/mosquitto.conf          MQTT broker settings
hub/world_logger.py         one CSV log for the world + heartbeat
hub/serial_bridge.py        gate Uno (USB) -> MQTT
hub/fake_world.py           mock mode: every zone, simulated
dashboard/app.py            FastAPI server + API
dashboard/static/index.html the dashboard page
ros2/                       v2: ROS 2 plan + mqtt_bridge_node.py
simulation/                 v2: Gazebo digital twin plan
docs/                       plan + architecture diagram
hardware/                   cardboard Control Center building
experiments/test_log.csv    safety tests (world_log.csv is created here when the logger runs)
```

## Quick start (any laptop, no hardware)
```bash
cd 6_control_center
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
# Mosquitto must be running (see hub/README.md), then, each in its own terminal:
python hub/world_logger.py
python hub/fake_world.py
python ../5_the_road/dog/code/dog_mock.py
uvicorn dashboard.app:app --host 0.0.0.0 --port 8000     # open http://localhost:8000
```

## v1 · Hub + dashboard (weeks 6–8, with the Smart Farm)
| | Build | Code | Status |
|---|---|---|---|
| v1 | Pi set up: SSH, venv, Mosquitto | `hub/README.md` | ☐ |
| v2 | One log for the world + heartbeat | `hub/world_logger.py` | ☐ |
| v3 | Dashboard in mock mode | `hub/fake_world.py`, `dashboard/` | ☐ |
| v4 | Real zones: gate via serial bridge, farm via Wi‑Fi | `hub/serial_bridge.py` | ☐ |
| v5 | **World E-stop** tested on every zone | dashboard | ☐ |

## v2 · ROS 2 + Gazebo (weeks 15–17)
See [ros2/README.md](ros2/README.md) and [simulation/README.md](simulation/README.md).

## Checkpoint
- [ ] Dashboard works from your phone on the same Wi‑Fi
- [ ] E-stop stops every zone, and commands are refused while it's on
- [ ] Stopping `world_logger.py` makes the farm stop watering

Pi 2 note: fine for v1 (Ethernet or a USB Wi‑Fi dongle). For ROS 2 and vision, a Pi 4/5 or a laptop with Ubuntu is much easier.
