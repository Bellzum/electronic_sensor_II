# How to design the circuit (and test it without hardware)

![Wiring diagram](figures/wiring_diagram.svg)

## Wokwi (~20 min)
1. **wokwi.com → New project → ESP32**.
2. Add 2 × **Servo** and a **Pushbutton**. Wire as in [wiring.md](wiring.md) (Wokwi powers servos from the board, which is fine in simulation only).
3. Paste `code/01_servo_limits` → ▶ → type `200 10` in the Serial Monitor. The servos must stop at the limits (160, 40), not at 200 and 10.
4. Watch how slowly they move: that's `MAX_STEP`.
5. Screenshot → `docs/figures/wokwi_tower.png`.

## Testing the camera code without the tower
The Python scripts work on your laptop's built-in webcam:
```bash
cd 4_watchtower/pi
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
python ball_tracker.py --host test.mosquitto.org --show
```
Hold up something red. You'll see a yellow circle and the FPS (frames per second).
> `test.mosquitto.org` is public: fine for testing, use your own Pi for the real world.

## Check before real wiring
- [ ] Servo power is separate, GND shared
- [ ] Limits tested: the bracket never hits its end stops
- [ ] Freeze button stops movement immediately
