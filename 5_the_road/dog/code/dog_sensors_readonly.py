"""dog_sensors_readonly.py - REAL dog sensor readings into the world. READ-ONLY: nothing moves.

Purpose:
    Runs on the robot dog's Raspberry Pi and publishes battery, distance and IMU
    to world/dog/sensors using Freenove's own sensor classes. It never imports the
    movement code (Control.py), so the dog can't move from this script.

Before running:
    1. Check the Robot Shield version (see ../docs/plan.md). v1.0-v1.9 needs the ADS7830.py fix.
    2. Stop Freenove's own server first (only one program should use the sensors).

Run on the dog's Pi:
    pip install paho-mqtt
    python dog_sensors_readonly.py --host <IP of the hub Pi> \
        --freenove ~/Freenove_Robot_Dog_Kit_for_Raspberry_Pi/Code/Server
"""
import argparse
import json
import sys
import time

import paho.mqtt.client as mqtt

LOW_BATTERY_V = 6.4      # Freenove's Server.py shuts down below this


def main():
    """Purpose: read sensors once a second and publish them. Args: none. Returns: None."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", required=True, help="hub Pi IP (MQTT broker)")
    parser.add_argument("--freenove", required=True, help="path to Freenove Code/Server folder")
    args = parser.parse_args()

    sys.path.insert(0, args.freenove)
    from ADS7830 import ADS7830          # battery
    from Ultrasonic import Ultrasonic    # distance
    from IMU import IMU                  # pitch, roll, yaw

    adc, sonic, imu = ADS7830(), Ultrasonic(), IMU()
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="dog-sensors")
    client.connect(args.host, 1883)
    client.loop_start()
    print("Publishing world/dog/sensors (read-only). Ctrl+C to stop.")
    try:
        while True:
            battery = round(adc.power(0), 2)
            distance = round(float(sonic.get_distance()), 1)
            pitch, roll, yaw = (round(v, 1) for v in imu.imuUpdate())
            data = {"battery": battery, "battery_low": battery < LOW_BATTERY_V,
                    "distance": distance, "pitch": pitch, "roll": roll, "yaw": yaw}
            client.publish("world/dog/sensors", json.dumps(data))
            print(data)
            time.sleep(1.0)
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()


if __name__ == "__main__":
    main()
