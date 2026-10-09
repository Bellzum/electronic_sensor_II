# Zone 5 · The Road 🛣️

The road connects the Gate and the Farm. Two robots drive it:

| Part | Robot | Weeks | Folder |
|---|---|---|---|
| A | **Delivery Rover** (2 wheels, ESP32) | 11–14 | [`rover/`](rover/) |
| B ⭐ | **Freenove robot dog** patrol | 18–21 | [`dog/`](dog/) |

```text
5_the_road/
├── README.md            ← you are here
├── rover/               ← code, docs, cardboard shell + road tiles, tests
└── dog/                 ← mock dog, read-only sensors, Freenove code map, collar
```

## Road tiles
Use the road part of [`rover/hardware/cardboard_rover_and_road.svg`](rover/hardware/cardboard_rover_and_road.svg): 4–6 tiles of 300 × 100 mm from the gate to the farm, with one **gate stop line** tile. See [rover/hardware/cardboard_demo.md](rover/hardware/cardboard_demo.md).

## Same rules for both robots
1. **No news = stop:** movement commands last 1 s. The dashboard re-sends while you hold the button.
2. **Obstacle < 20 cm:** forward is refused.
3. **Tipped over:** E-stop.
4. **World E-stop:** stops both.
5. **Every command logged** in the Control Center.

| | Rover | Dog |
|---|---|---|
| Commands | `world/cmd/rover` | `world/cmd/dog` |
| State | `world/rover/state` | `world/dog/state`, `world/dog/sensors` |
| RFID | tag under the shell | tag on the collar |

## Checkpoint
- [ ] Wi‑Fi off → robot stops by itself within 1 s
- [ ] Gate handoff works 5 times in a row
- [ ] Every drive is in the world log
