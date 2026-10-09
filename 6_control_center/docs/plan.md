# Control Center plan

## v1 · Hub + dashboard (weeks 6–8, with the Smart Farm)
**Day 1 — Mock first (no hardware, any laptop)**
1. Install Mosquitto on your laptop or Pi (see [../hub/README.md](../hub/README.md)).
2. Run `hub/world_logger.py`, `hub/fake_world.py`, `5_the_road/dog/code/dog_mock.py` and the dashboard.
3. Open `http://localhost:8000`. Every zone card moves. Press the E-stop: everything goes to ESTOP.

✅ Done when: the whole world works in mock mode.

**Day 2 — Pi setup:** Raspberry Pi OS, SSH, Ethernet or USB Wi‑Fi (Pi 2), Mosquitto, Python venv.
**Day 3 — Real zones:** stop `fake_world.py`. Start `hub/serial_bridge.py` for the gate Uno. The farm ESP32 publishes over Wi‑Fi.
**Day 4 — Safety tests:** run every row of `experiments/test_log.csv`.

## v2 · ROS 2 + Gazebo (weeks 15–17)
See [../ros2/README.md](../ros2/README.md) and [../simulation/README.md](../simulation/README.md).
