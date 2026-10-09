# Hub setup (Raspberry Pi)

The hub is the world's message centre. Every zone sends its messages here.

## 1. Install the MQTT broker (Mosquitto)
```bash
sudo apt update
sudo apt install -y mosquitto mosquitto-clients
sudo cp mosquitto.conf /etc/mosquitto/conf.d/mini_smart_world.conf
sudo systemctl restart mosquitto
hostname -I          # <- this IP goes into every ESP32 secrets.h as MQTT_HOST
```

Test it in two terminals:
```bash
mosquitto_sub -h localhost -t 'world/#' -v      # terminal 1: listen to everything
mosquitto_pub -h localhost -t world/test -m hi  # terminal 2: you should see "world/test hi" in terminal 1
```

## 2. Python environment
```bash
cd ~/mini_smart_world/6_control_center
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
```

## 3. Run the hub programs (one terminal each, or use tmux)
| Program | What it does |
|---|---|
| `python hub/world_logger.py` | Saves **every** world message to `experiments/world_log.csv` and sends `world/hub/heartbeat` every second |
| `python hub/serial_bridge.py --port /dev/ttyACM0` | Turns the gate Uno's Serial CSV (Zone 1 + 2) into MQTT messages |
| `python hub/fake_world.py` | **Mock mode:** pretends to be every zone, so you can build the dashboard with no hardware |
| `uvicorn dashboard.app:app --host 0.0.0.0 --port 8000` | The dashboard: open `http://<pi-ip>:8000` on your phone |

The world's safety depends on `world_logger.py`: if it stops, the heartbeat stops, and the farm stops watering.

## Pi 2 note
The Pi 2 has no built-in Wi‑Fi: use Ethernet or a USB Wi‑Fi dongle. Everything in this folder runs fine on a Pi 2. ROS 2 (v2) needs a Pi 4/5 or a laptop.
