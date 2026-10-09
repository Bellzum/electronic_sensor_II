# Wiring (ESP32)

![Wiring diagram](figures/wiring_diagram.svg)

## Parts
ESP32 DevKit (ESP32-WROOM-32) · DHT22 (or BME280) · 10 kΩ resistor · capacitive soil moisture sensor v1.2 · 2 × MOSFET switch modules (e.g. IRF520 or AO3400 module) · small 5 V fan · 5 V mini submersible pump + tube · separate 5 V supply (USB power bank or 4×AA) · push button · plant pot

## Pin map
| Part | Pin on part | ESP32 |
|---|---|---|
| DHT22 | VCC / DATA / GND | 3V3 / **GPIO4** (10 kΩ from DATA to 3V3) / GND |
| Soil sensor | VCC / AOUT / GND | 3V3 / **GPIO34** / GND |
| Fan MOSFET module | SIG / GND | **GPIO25** / GND |
| Pump MOSFET module | SIG / GND | **GPIO26** / GND |
| E-stop button | legs | **GPIO27** / GND |

Fan and pump get power from the **separate 5 V supply** through the MOSFET modules (V+ / V− screw terminals). Connect the supply's GND to the ESP32 GND.

## Rules for this zone
- ⚠️ ESP32 pins are **3.3 V**. Never connect 5 V to a GPIO.
- ⚠️ **Water and electronics:** the ESP32, breadboard and supply sit **outside** the greenhouse, higher than the water. Only the soil sensor and the pump tube go inside.
- Use a **capacitive** soil sensor (black PCB), not the cheap two-fork resistive one, which corrodes in days.
- Don't use GPIO 0, 2, 12 or 15 for sensors: they change how the ESP32 boots.

## Sensor review cheat-sheet
| Part | Signal type | Simple meaning |
|---|---|---|
| DHT22 | One-wire digital | Sends temperature and humidity as a coded message; read at most every 2 s |
| Soil sensor | Analog (0–4095) | Wetter soil = **lower** number on capacitive sensors |
| MOSFET module | Digital switch | A 3.3 V pin turns a bigger 5 V load on and off |
