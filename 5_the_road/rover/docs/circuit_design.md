# How to design the circuit (and test it without hardware)

![Wiring diagram](figures/wiring_diagram.svg)

Wokwi has the ESP32, **MPU6050**, **HC-SR04** and buttons, but no TB6612FNG. Use **4 LEDs** in place of the motor direction pins (AIN1, AIN2, BIN1, BIN2) to see what the motors would do.

## Wokwi step by step (~30 min)
1. **wokwi.com → New project → ESP32**.
2. Add: MPU6050, HC-SR04, Pushbutton, 4 LEDs + 220 Ω on GPIO26, 27, 33, 19.
3. Paste `code/03_obstacle_tilt` → ▶.
4. Type `f` → the two "forward" LEDs light, then go off after 1 s (timeout!).
5. Click the HC-SR04, set distance to 10 cm, type `f` → refused.
6. Click the MPU6050, tilt it over 30° → E-STOP.
7. Screenshot → `docs/figures/wokwi_rover.png`.

## Check before the real rover drives
- [ ] Timeout works: it stops 1 s after the last command
- [ ] Obstacle refusal works
- [ ] Tilt E-stop works
- [ ] E-stop button works
- [ ] First real test with wheels in the air
