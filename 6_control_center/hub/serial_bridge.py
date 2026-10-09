"""serial_bridge.py - bring the gate's Arduino Uno (Zone 1 + 2) into the Wi-Fi world.

Purpose:
    The Uno has no Wi-Fi. It prints CSV lines on USB ("ms,event,value"), e.g.
        12345,card,AA BB CC DD GRANTED
        12350,gate,UNLOCKED
        20000,security,ALARM
    This script reads them on the Pi and publishes MQTT messages:
        world/gate/card       {"uid":"AA BB CC DD","allowed":true}
        world/gate/state      LOCKED | UNLOCKED
        world/security/state  DISARMED | ARMED | ALARM
        world/security/alarm  true | false

Run:  python hub/serial_bridge.py --port /dev/ttyACM0      (find the port with: ls /dev/tty*)
"""
import argparse
import json

import paho.mqtt.client as mqtt
import serial


def to_messages(line):
    """Purpose: turn one CSV line from the Uno into MQTT (topic, payload) pairs.

    Args:
        line: text like "12345,card,AA BB CC DD GRANTED"
    Returns:
        list of (topic, payload) tuples (empty if the line isn't an event)
    """
    parts = line.strip().split(",", 2)
    if len(parts) != 3 or not parts[0].isdigit():
        return []
    _, event, value = parts
    if value in ("GRANTED", "DENIED"):        # 1_smart_gate/code/02_door_access prints "ms,uid,result"
        return [("world/gate/card", json.dumps({"uid": event, "allowed": value == "GRANTED"}))]
    if event == "card":
        uid, _, result = value.rpartition(" ")
        return [("world/gate/card", json.dumps({"uid": uid, "allowed": result == "GRANTED"}))]
    if event == "gate":
        return [("world/gate/state", value)]
    if event == "security":
        return [("world/security/state", value), ("world/security/alarm", "true" if value == "ALARM" else "false")]
    if event == "state":                       # from 04_security_system
        return [("world/security/state", value), ("world/security/alarm", "true" if value == "ALARM" else "false")]
    return [(f"world/gate/{event}", value)]


def main():
    """Purpose: read the Uno forever and publish its events. Args: none. Returns: None."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", default="/dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=9600)
    parser.add_argument("--host", default="localhost")
    args = parser.parse_args()

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="gate-serial-bridge")
    client.connect(args.host, 1883)
    client.loop_start()
    uno = serial.Serial(args.port, args.baud, timeout=1)
    print(f"Reading {args.port}. Ctrl+C to stop.")
    try:
        while True:
            line = uno.readline().decode(errors="ignore")
            for topic, payload in to_messages(line):
                client.publish(topic, payload)
                print(topic, payload)
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()
        uno.close()


if __name__ == "__main__":
    main()
