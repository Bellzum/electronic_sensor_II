"""ball_tracker.py - the Watchtower follows a red ball (proportional control).

Purpose:
    Finds the biggest red blob in the camera image and sends small pan/tilt
    corrections to world/cmd/tower so the ball moves toward the centre.

    error = ball position - image centre
    new angle = old angle - GAIN * error      (small steps, always inside the limits)

Run:  python ball_tracker.py --host localhost --show
Stop: Ctrl+C or press q in the window. Stops sending while world/estop is true.
"""
import argparse
import json
import time

import cv2
import numpy as np
import paho.mqtt.client as mqtt

GAIN = 0.03                     # degrees per pixel of error. Too big = wobbly
DEAD_ZONE_PX = 25               # close enough to the centre: don't move
PAN_LIMITS, TILT_LIMITS = (20, 160), (40, 120)
SEND_EVERY_S = 0.15
# red wraps around the hue circle in HSV, so we need two ranges
RED_LOW_1, RED_HIGH_1 = np.array([0, 120, 70]), np.array([10, 255, 255])
RED_LOW_2, RED_HIGH_2 = np.array([170, 120, 70]), np.array([180, 255, 255])

state = {"pan": 90, "tilt": 80, "estop": False}


def find_ball(frame):
    """Purpose: locate the biggest red blob.

    Args:
        frame: BGR image from the camera
    Returns:
        (x, y, radius) in pixels, or None if no ball
    """
    hsv = cv2.cvtColor(cv2.GaussianBlur(frame, (11, 11), 0), cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, RED_LOW_1, RED_HIGH_1) | cv2.inRange(hsv, RED_LOW_2, RED_HIGH_2)
    mask = cv2.dilate(cv2.erode(mask, None, iterations=2), None, iterations=2)
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if not contours:
        return None
    (x, y), r = cv2.minEnclosingCircle(max(contours, key=cv2.contourArea))
    return (int(x), int(y), int(r)) if r > 8 else None


def on_message(client, userdata, msg):
    """Purpose: track the real tower angle and the E-stop. Args: MQTT args. Returns: None."""
    text = msg.payload.decode(errors="ignore")
    if msg.topic == "world/estop":
        state["estop"] = text.strip() == "true"
    elif msg.topic == "world/tower/position":
        try:
            data = json.loads(text)
            state["pan"], state["tilt"] = data["pan"], data["tilt"]
        except (json.JSONDecodeError, KeyError):
            pass


def main():
    """Purpose: camera loop -> find ball -> send correction. Args: none. Returns: None."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="localhost")
    parser.add_argument("--camera", type=int, default=0)
    parser.add_argument("--show", action="store_true", help="show a window (needs a screen)")
    args = parser.parse_args()

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="ball-tracker")
    client.on_message = on_message
    client.connect(args.host, 1883)
    client.subscribe("world/estop")
    client.subscribe("world/tower/position")
    client.loop_start()

    camera = cv2.VideoCapture(args.camera)
    last_send, frames, t0 = 0.0, 0, time.time()
    try:
        while True:
            ok, frame = camera.read()
            if not ok:
                break
            frames += 1
            h, w = frame.shape[:2]
            ball = find_ball(frame)
            if ball and not state["estop"] and time.time() - last_send > SEND_EVERY_S:
                ex, ey = ball[0] - w // 2, ball[1] - h // 2
                pan, tilt = state["pan"], state["tilt"]
                if abs(ex) > DEAD_ZONE_PX:
                    pan = int(np.clip(pan - GAIN * ex, *PAN_LIMITS))
                if abs(ey) > DEAD_ZONE_PX:
                    tilt = int(np.clip(tilt + GAIN * ey, *TILT_LIMITS))
                client.publish("world/cmd/tower", f"{pan},{tilt}")
                state["pan"], state["tilt"] = pan, tilt
                last_send = time.time()
            if args.show:
                if ball:
                    cv2.circle(frame, ball[:2], ball[2], (0, 255, 255), 2)
                fps = frames / max(time.time() - t0, 1e-6)
                cv2.putText(frame, f"FPS {fps:.1f}  {'E-STOP' if state['estop'] else ''}", (10, 25),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
                cv2.imshow("ball tracker", frame)
                if cv2.waitKey(1) & 0xFF == ord("q"):
                    break
    except KeyboardInterrupt:
        pass
    finally:
        print(f"average FPS: {frames / max(time.time() - t0, 1e-6):.1f}")
        client.loop_stop()
        camera.release()
        cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
