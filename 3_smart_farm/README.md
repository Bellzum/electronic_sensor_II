# Zone 3 · Smart Farm 🌱

A tiny greenhouse with a plant, sensors, a fan and a mini pump. This is when the world goes online: the Control Center (Raspberry Pi hub) starts in parallel, see [`../6_control_center`](../6_control_center/). **Weeks 6–8.**

**New parts:** DHT22 or BME280, capacitive soil moisture sensor, small 5 V fan and/or mini pump, MOSFET or relay module, second ESP32.

**New words:** calibration · noise / filtering · MQTT · CSV log.

| | Build | Status |
|---|---|---|
| v1 | Read temp, humidity, soil every 5 s | ☐ |
| v2 | **Calibration experiment:** soil sensor dry vs wet, 20 readings each → `notes/calibration.csv` | ☐ |
| v3 | Publish `world/farm/sensors` over Wi‑Fi (MQTT) | ☐ |
| v4 | Fan if hot; pump 3 s if dry, **max 3 runs/hour**; obey `world/estop` | ☐ |
| v5 | **Gate link:** only an allowed RFID card can switch the farm to manual mode | ☐ |

## Checkpoint
- [ ] Calibration CSV + written conclusion in `notes/`
- [ ] Pump can never run forever
- [ ] Farm data appears on the Control Center dashboard
