"""world_logger.py - the hub's heart: one log for the whole world + a heartbeat.

Purpose:
    - Saves every message on world/# to experiments/world_log.csv (timestamp, topic, payload)
    - Publishes world/hub/heartbeat every second. Zones use it for the
      "silence means stop" rule (e.g. the farm won't water without it).

Run:  python hub/world_logger.py --host localhost
"""
import argparse
import csv
import time
from datetime import datetime
from pathlib import Path

import paho.mqtt.client as mqtt

LOG_FILE = Path(__file__).resolve().parent.parent / "experiments" / "world_log.csv"


def on_message(client, userdata, msg):
    """Purpose: write one message as a CSV row.

    Args:
        client, userdata, msg: MQTT callback arguments
    Returns:
        None
    """
    if msg.topic == "world/hub/heartbeat":
        return                                   # don't fill the log with heartbeats
    new_file = not LOG_FILE.exists()
    LOG_FILE.parent.mkdir(exist_ok=True)
    with LOG_FILE.open("a", newline="") as f:
        writer = csv.writer(f)
        if new_file:
            writer.writerow(["time", "topic", "payload"])
        writer.writerow([datetime.now().isoformat(timespec="seconds"), msg.topic,
                         msg.payload.decode(errors="ignore")])
    print(msg.topic, msg.payload.decode(errors="ignore"))


def main():
    """Purpose: subscribe to everything and send the heartbeat. Args: none. Returns: None."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="localhost")
    args = parser.parse_args()

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="world-logger")
    client.on_message = on_message
    client.connect(args.host, 1883)
    client.subscribe("world/#")                  # '#' = every topic under world/
    client.loop_start()
    print(f"Logging to {LOG_FILE}. Ctrl+C to stop.")
    try:
        while True:
            client.publish("world/hub/heartbeat", str(int(time.time())))
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()


if __name__ == "__main__":
    main()
