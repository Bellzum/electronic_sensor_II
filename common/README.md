# common/

Code shared by every zone.

| File (planned) | Purpose |
|---|---|
| `topics.md` | The topic list from the main README; update it when a zone adds a topic |
| `world_mqtt.h` | ESP32 helper: connect to Wi‑Fi + MQTT, publish state, listen for `world/estop` (from Zone 3) |
| `world_mqtt.py` | Python helper for the Pi with the same topic names |

Rule: if two zones need the same code, it moves here.
