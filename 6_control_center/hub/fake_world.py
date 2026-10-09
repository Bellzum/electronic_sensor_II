"""fake_world.py - MOCK MODE: pretend to be every zone so you can build the dashboard first.

Purpose:
    Publishes realistic fake messages for the gate, lights, farm, watchtower and rover,
    and reacts to dashboard commands and the world E-stop, like the real zones will.
    Nothing physical moves. Run the real hardware zones instead of this when they're ready.
    (The robot dog has its own mock: 5_the_road/dog/code/dog_mock.py)

Run:  python hub/fake_world.py --host localhost
"""
import argparse
import json
import random
import time

import paho.mqtt.client as mqtt

world = {"estop": False, "fan": False, "pump_until": 0.0, "tower": "HOME",
         "rover_cmd": "stop", "rover_until": 0.0, "security": "DISARMED"}
CARDS = [("A1 B2 C3 D4", True), ("DE AD BE EF", False)]


def on_message(client, userdata, msg):
    """Purpose: react to commands like the real zones. Args: MQTT callback args. Returns: None."""
    text = msg.payload.decode(errors="ignore").strip()
    if msg.topic == "world/estop":
        world["estop"] = text == "true"
        if world["estop"]:
            world.update(pump_until=0.0, rover_cmd="stop", fan=False)
    elif world["estop"]:
        return                                            # E-stop: ignore every command
    elif msg.topic == "world/cmd/farm":
        if text == "pump":
            world["pump_until"] = time.time() + 3        # 3 s limit, like the real farm
        elif text in ("fan_on", "fan_off"):
            world["fan"] = text == "fan_on"
    elif msg.topic == "world/cmd/tower":
        world["tower"] = text if text.isalpha() else "MANUAL"
    elif msg.topic == "world/cmd/rover":
        world["rover_cmd"] = text
        world["rover_until"] = time.time() + 1           # 1 s timeout, like the real rover
    elif msg.topic == "world/cmd/security":
        world["security"] = text


def main():
    """Purpose: publish fake zone data forever. Args: none. Returns: None."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="localhost")
    args = parser.parse_args()

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="fake-world")
    client.on_message = on_message
    client.connect(args.host, 1883)
    for t in ("world/estop", "world/cmd/#"):
        client.subscribe(t)
    client.loop_start()
    print("Fake world running (MOCK MODE). Ctrl+C to stop.")
    temp, soil, distance, tick = 24.0, 55, 80.0, 0
    try:
        while True:
            tick += 1
            now = time.time()
            temp = round(min(34, max(18, temp + random.uniform(-0.3, 0.35))), 1)
            soil = max(5, min(95, soil - random.choice([0, 0, 1]) + (8 if now < world["pump_until"] else 0)))
            fan = world["fan"] and not world["estop"]
            pump = now < world["pump_until"]
            if tick % 10 == 0:
                client.publish("world/farm/sensors", json.dumps({"temp": temp, "humidity": random.randint(45, 65), "soil": soil}))
                client.publish("world/farm/state", json.dumps({"fan": fan, "pump": pump, "mode": "AUTO", "estop": world["estop"]}))
                client.publish("world/lights/state", "ON" if (tick // 300) % 2 else "OFF")
            if tick % 150 == 0:                                   # a card every ~75 s
                uid, ok = random.choice(CARDS)
                client.publish("world/gate/card", json.dumps({"uid": uid, "allowed": ok}))
                client.publish("world/gate/state", "UNLOCKED" if ok and not world["estop"] else "LOCKED")
                client.publish("world/tower/position", json.dumps({"pan": 40, "tilt": 70, "preset": "GATE", "estop": world["estop"]}))
            elif tick % 150 == 8:
                client.publish("world/gate/state", "LOCKED")
            if tick % 4 == 0:
                driving = world["rover_cmd"] != "stop" and now < world["rover_until"] and not world["estop"]
                distance = round(max(8, min(150, distance + (-6 if driving and world["rover_cmd"] == "forward" else random.uniform(-2, 3)))), 1)
                blocked = driving and world["rover_cmd"] == "forward" and distance < 20
                state = "ESTOP" if world["estop"] else "BLOCKED" if blocked else "DRIVING" if driving else "IDLE"
                client.publish("world/rover/state", json.dumps({"state": state, "cmd": world["rover_cmd"] if driving else "s",
                                                                "distance": distance, "tilt": round(random.uniform(0, 3), 1),
                                                                "estop": world["estop"]}))
                client.publish("world/security/state", world["security"])
                client.publish("world/tower/position", json.dumps({"pan": 90, "tilt": 80, "preset": world["tower"], "estop": world["estop"]}))
            time.sleep(0.5)
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()


if __name__ == "__main__":
    main()
