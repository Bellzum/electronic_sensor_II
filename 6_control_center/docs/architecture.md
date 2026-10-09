# Architecture

```mermaid
flowchart LR
  subgraph Zones
    G[1 Smart Gate<br/>Arduino Uno] -- USB serial CSV --> SB
    L[2 Lights & Security<br/>same Uno] -. same USB .-> SB
    F[3 Smart Farm<br/>ESP32]
    T[4 Watchtower<br/>ESP32 + camera]
    R[5 Rover<br/>ESP32]
    D[★ Robot dog<br/>Pi on the dog]
  end
  SB[serial_bridge.py] --> B((MQTT broker<br/>Mosquitto))
  F <--> B
  T <--> B
  R <--> B
  D <--> B
  B --> WL[world_logger.py<br/>CSV log + heartbeat]
  B <--> DB[dashboard/app.py<br/>FastAPI]
  DB <--> P[Phone / laptop browser]
  B <-. v2 .-> ROS[ROS 2 mqtt_bridge_node]
```

## Message rules
- Topic names: `world/<zone>/<thing>`. Commands: `world/cmd/<zone>`.
- The E-stop `world/estop` is **retained**: a device that reconnects gets the current value at once.
- The hub sends `world/hub/heartbeat` every second. No heartbeat = zones go safe.
- Moving robots stop 1 s after the last command. The dashboard re-sends while you hold a button.
