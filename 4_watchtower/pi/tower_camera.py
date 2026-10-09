"""tower_camera.py - take a photo when something happens in the world.

Purpose:
    Listens for events (card scan at the gate, security alarm). When the Watchtower
    has turned to the GATE, it saves a photo and publishes world/tower/event.

Run on the Raspberry Pi (or a laptop) with the USB webcam plugged in:
    python3 -m venv .venv && source .venv/bin/activate
    pip install -r requirements.txt
    python tower_camera.py --host localhost
"""
import argparse
import json
import time
from datetime import datetime
from pathlib import Path

import cv2
import paho.mqtt.client as mqtt

PHOTO_DIR = Path("photos")
WAIT_FOR_TOWER_S = 4.0     # take the photo anyway if the tower doesn't report in time

state = {"pending_reason": None, "pending_since": 0.0, "estop": False}


def save_photo(camera, reason):
    """Purpose: grab one frame and save it as a JPG.

    Args:
        camera: an opened cv2.VideoCapture
        reason: short text, e.g. "card_scan"
    Returns:
        file name (str) or None if the camera failed
    """
    for _ in range(5):          # throw away old buffered frames so the photo is fresh
        camera.read()
    ok, frame = camera.read()
    if not ok:
        print("camera read failed")
        return None
    PHOTO_DIR.mkdir(exist_ok=True)
    name = f"{datetime.now():%Y-%m-%d_%H%M%S}_{reason}.jpg"
    cv2.putText(frame, f"{datetime.now():%H:%M:%S} {reason}", (10, 30),
                cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 255), 2)
    cv2.imwrite(str(PHOTO_DIR / name), frame)
    return name


def on_message(client, userdata, msg):
    """Purpose: remember what happened. Args: MQTT callback args. Returns: None."""
    text = msg.payload.decode(errors="ignore").strip()
    if msg.topic == "world/estop":
        state["estop"] = text == "true"
    elif msg.topic == "world/gate/card":
        state["pending_reason"], state["pending_since"] = "card_scan", time.time()
    elif msg.topic == "world/security/alarm" and text == "true":
        state["pending_reason"], state["pending_since"] = "alarm", time.time()
    elif msg.topic == "world/tower/position" and state["pending_reason"]:
        try:
            if json.loads(text).get("preset") == "GATE":
                state["pending_since"] = 0.0      # tower arrived: take the photo now
        except json.JSONDecodeError:
            pass


def main():
    """Purpose: connect, wait for events, take photos. Args: none. Returns: None."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="localhost", help="MQTT broker (the Pi)")
    parser.add_argument("--camera", type=int, default=0, help="camera index")
    args = parser.parse_args()

    camera = cv2.VideoCapture(args.camera)
    if not camera.isOpened():
        raise SystemExit("No camera found. Try --camera 1")

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="tower-camera")
    client.on_message = on_message
    client.connect(args.host, 1883)
    for topic in ("world/estop", "world/gate/card", "world/security/alarm", "world/tower/position"):
        client.subscribe(topic)
    client.loop_start()
    print("Waiting for events... Ctrl+C to stop")

    try:
        while True:
            reason = state["pending_reason"]
            arrived = state["pending_since"] == 0.0
            waited_long = time.time() - state["pending_since"] > WAIT_FOR_TOWER_S
            if reason and (arrived or waited_long):
                name = save_photo(camera, reason)
                if name:
                    event = {"photo": name, "reason": reason, "estop": state["estop"]}
                    client.publish("world/tower/event", json.dumps(event))
                    print("photo:", event)
                state["pending_reason"] = None
            time.sleep(0.1)
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()
        camera.release()


if __name__ == "__main__":
    main()
