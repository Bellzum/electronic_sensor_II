"""dog_mock.py - a pretend robot dog for the world (MOCK MODE, nothing moves).

Purpose:
    Lets you build the dashboard, safety rules and logs for the dog BEFORE the
    real dog is connected. It behaves like the real dog will:

    listens:   world/cmd/dog   stand | sit | forward | backward | left | right | stop | relax
               world/estop     true | false
    publishes: world/dog/state {"state":"MOVING","cmd":"forward","battery":7.4,"distance":55.0,
                                "pitch":0.0,"roll":0.0,"estop":false,"mode":"MOCK"}

    Same rules as the rover: 1 s movement timeout, obstacle refusal under 20 cm,
    battery too low (< 6.4 V, Freenove's own limit) refuses moves, E-stop wins.

Run anywhere with Python:  pip install paho-mqtt  then  python dog_mock.py --host localhost
"""
import argparse
import json
import random
import time

import paho.mqtt.client as mqtt

MOVES = {"forward", "backward", "left", "right"}
POSES = {"stand", "sit", "relax"}
TIMEOUT_S = 1.0
STOP_CM = 20.0
LOW_BATTERY_V = 6.4

dog = {"state": "IDLE", "cmd": "stop", "battery": 8.2, "distance": 80.0,
       "pitch": 0.0, "roll": 0.0, "estop": False, "mode": "MOCK"}
last_cmd_time = 0.0


def refuse_reason(cmd):
    """Purpose: check the safety rules for one command.

    Args:
        cmd: command text
    Returns:
        a reason string if the command must be refused, else None
    """
    if dog["estop"]:
        return "E-stop active"
    if cmd in MOVES and dog["battery"] < LOW_BATTERY_V:
        return "battery too low"
    if cmd == "forward" and dog["distance"] < STOP_CM:
        return "obstacle"
    return None


def on_message(client, userdata, msg):
    """Purpose: react to commands and the E-stop. Args: MQTT callback args. Returns: None."""
    global last_cmd_time
    text = msg.payload.decode(errors="ignore").strip()
    if msg.topic == "world/estop":
        dog["estop"] = text == "true"
        if dog["estop"]:
            dog.update(state="ESTOP", cmd="stop")
        return
    reason = refuse_reason(text)
    if reason:
        print(f"refused '{text}': {reason}")
        dog.update(state="ESTOP" if dog["estop"] else "BLOCKED", cmd="stop")
        return
    dog["cmd"] = text
    dog["state"] = "MOVING" if text in MOVES else ("STANDING" if text == "stand" else "IDLE")
    last_cmd_time = time.time()
    print("would do:", text)


def main():
    """Purpose: simulate the dog and publish its state twice a second. Args: none. Returns: None."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="localhost")
    args = parser.parse_args()

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="dog-mock")
    client.on_message = on_message
    client.connect(args.host, 1883)
    client.subscribe("world/cmd/dog")
    client.subscribe("world/estop")
    client.loop_start()
    print("Mock dog running. Ctrl+C to stop.")
    try:
        while True:
            if dog["state"] == "MOVING" and time.time() - last_cmd_time > TIMEOUT_S:
                dog.update(state="STANDING", cmd="stop")          # no news = stop
            if dog["state"] == "MOVING":
                dog["battery"] = round(dog["battery"] - 0.002, 3)   # moving drains the battery
            dog["distance"] = round(max(5.0, min(150.0, dog["distance"] + random.uniform(-4, 4))), 1)
            dog["pitch"] = round(random.uniform(-2, 2), 1)
            dog["roll"] = round(random.uniform(-2, 2), 1)
            if not dog["estop"] and dog["state"] == "ESTOP":
                dog["state"] = "IDLE"
            client.publish("world/dog/state", json.dumps(dog))
            time.sleep(0.5)
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()


if __name__ == "__main__":
    main()
