# Freenove server code map

Where things live in `Freenove_Robot_Dog_Kit_for_Raspberry_Pi/Code/Server/` (v1 of this part: read it, fill in the "What I found" column).

| File | What it does | Use in Mini Smart World | What I found |
|---|---|---|---|
| `ADS7830.py` | Battery voltage: `ADS7830().power(0)` | `world/dog/sensors` battery. **Shield v1.0–v1.9: fix the formula first** | |
| `Ultrasonic.py` | Distance: `Ultrasonic().get_distance()` (cm) | Obstacle refusal | |
| `IMU.py` | `IMU().imuUpdate()` → pitch, roll, yaw | Tipped-over E-stop | |
| `Control.py` | Movement: `forWard()`, `backWard()`, `turnLeft()`, `turnRight()`, `stop()`, `relax()` | Only from v4, behind the safety rules | |
| `Command.py` | Command names: `CMD_MOVE_FORWARD`, `CMD_MOVE_STOP`, `CMD_RELAX`, `CMD_POWER`… | Map world/cmd/dog → these | |
| `Server.py` | Freenove's TCP server for their phone/PC app. Checks the battery and shuts down below **6.4 V** | Keep the same 6.4 V limit | |
| `Servo.py`, `PCA9685.py` | Low-level servo driver (16-channel PWM chip) | Don't touch | |
| `Buzzer.py`, `Led.py` | Buzzer, LED strip | Show E-stop / alarm on the dog | |
| `camera.py` | Camera stream | Later: Watchtower-style vision on the dog | |

## Shield version check
Look at the text printed on the Robot Shield board.
- **v2.0 or later:** no change.
- **v1.0 – v1.9:** in `ADS7830.py`, change `data[4]/255.0*5.0*2` to `data[4]/255.0*5.0*3`.

Then compare `power(0)` with a **multimeter** on the battery. Write both numbers in `../experiments/test_log.csv`.
