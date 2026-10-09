"""app.py - the Mini Smart World dashboard (FastAPI).

Purpose:
    - Listens to every world/# message and keeps the latest value of each topic
    - Serves the web page (static/index.html) and a small JSON API for it
    - Sends commands and the world E-stop

API:
    GET  /api/state            latest value per topic + last 60 log lines
    POST /api/cmd   {"zone": "rover", "msg": "forward"}   -> world/cmd/rover
    POST /api/estop {"on": true}                           -> world/estop (retained)

Run from the 6_control_center folder:
    uvicorn dashboard.app:app --host 0.0.0.0 --port 8000
    then open http://<pi-ip>:8000 on any phone or laptop on the same Wi-Fi.
Set MQTT_HOST if the broker isn't on this computer:  MQTT_HOST=192.168.1.50 uvicorn ...
"""
import os
import time
from collections import deque
from pathlib import Path

import paho.mqtt.client as mqtt
from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel

HERE = Path(__file__).resolve().parent
REPO = HERE.parent.parent
PHOTOS = REPO / "4_watchtower" / "pi" / "photos"
ALLOWED_ZONES = {"farm", "tower", "rover", "dog", "security"}

latest = {}                       # topic -> {"value": str, "t": unix time}
log = deque(maxlen=300)           # (time, topic, payload)


def on_message(client, userdata, msg):
    """Purpose: remember the newest message per topic. Args: MQTT callback args. Returns: None."""
    text = msg.payload.decode(errors="ignore")
    latest[msg.topic] = {"value": text, "t": time.time()}
    if msg.topic != "world/hub/heartbeat":
        log.appendleft((time.strftime("%H:%M:%S"), msg.topic, text))


mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="dashboard")
mqtt_client.on_message = on_message

app = FastAPI(title="Mini Smart World")
app.mount("/assets", StaticFiles(directory=REPO / "assets"), name="assets")
PHOTOS.mkdir(parents=True, exist_ok=True)
app.mount("/photos", StaticFiles(directory=PHOTOS), name="photos")


@app.on_event("startup")
def start_mqtt():
    """Purpose: connect to the broker when the server starts. Args: none. Returns: None."""
    mqtt_client.connect(os.environ.get("MQTT_HOST", "localhost"), 1883)
    mqtt_client.subscribe("world/#")
    mqtt_client.loop_start()


class Command(BaseModel):
    zone: str
    msg: str


class Estop(BaseModel):
    on: bool


@app.get("/")
def index():
    """Purpose: the dashboard page. Args: none. Returns: HTML file."""
    return FileResponse(HERE / "static" / "index.html")


@app.get("/api/state")
def state():
    """Purpose: everything the page needs in one call. Args: none. Returns: dict."""
    return {"now": time.time(), "topics": latest, "log": list(log)[:60]}


@app.post("/api/cmd")
def command(cmd: Command):
    """Purpose: send a command to one zone (refused during E-stop).

    Args:
        cmd: zone name + message text
    Returns:
        {"sent": topic}
    """
    if cmd.zone not in ALLOWED_ZONES:
        raise HTTPException(400, f"unknown zone {cmd.zone}")
    if latest.get("world/estop", {}).get("value") == "true" and cmd.msg not in ("stop", "reset"):
        raise HTTPException(409, "World E-stop is active")
    topic = f"world/cmd/{cmd.zone}"
    mqtt_client.publish(topic, cmd.msg)
    return {"sent": topic}


@app.post("/api/estop")
def estop(body: Estop):
    """Purpose: switch the world E-stop. Retained, so devices that reconnect get it too.

    Args:
        body: {"on": true/false}
    Returns:
        {"estop": bool}
    """
    mqtt_client.publish("world/estop", "true" if body.on else "false", retain=True)
    return {"estop": body.on}
