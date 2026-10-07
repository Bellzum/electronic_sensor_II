# Zone 4 · Watchtower 📷

A pan/tilt camera tower that watches the world and reacts to other zones. **Weeks 9–10.**

**New parts:** 2 SG90 servos + pan/tilt bracket, USB webcam or Pi camera.

**New words:** OpenCV · frame / FPS · HSV colour · proportional control.

| | Build | Status |
|---|---|---|
| v1 | Dashboard sliders move pan/tilt (with angle limits) | ☐ |
| v2 | Live camera stream on the dashboard | ☐ |
| v3 | Preset positions: GATE, FARM, ROAD | ☐ |
| v4 | **Event links:** card scan / ALARM → aim at GATE + photo; motion in Zone 2 → photo; publish `world/tower/event` | ☐ |
| v5 | OpenCV: find a coloured ball and follow it | ☐ |
| v6 | Photo gallery on the dashboard, linked to log rows | ☐ |

## Checkpoint
- [ ] I know my camera FPS
- [ ] Servo limits protect the bracket
- [ ] E-stop freezes the tower

A Pi 4/5 makes vision much smoother than a Pi 2.
