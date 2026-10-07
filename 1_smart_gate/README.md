# electronic_sensor_II — RFID Door Access System

Week project: review electronic sensors, design the circuit, then build it hands-on.
Reference: [Arduino Project Hub – Secure RFID Door Access](https://projecthub.arduino.cc/sonutest23/secure-rfid-door-access-system-using-arduino-and-lcd-with-servo-motor-31649c) · [YouTube walkthrough](https://www.youtube.com/watch?v=3xb2PLFjJxk)

## What it does
Tap an RFID card → Arduino checks the card ID → if allowed, LCD says "Welcome" and the servo "unlocks" the door for 3 s; otherwise "Access Denied".

## Folder layout
```
docs/plan.md        3-evening plan + checklists
docs/wiring.md      pin map, sensor review notes
code/01_read_uid/   sketch to read your card IDs
code/02_door_access/ main door system sketch
experiments/        test log (CSV)
```

## Link to Robot Science project
RFID check = an "authorization" sensor. Later it can become an "operator badge" that unlocks
Manual Control on the Freenove Dog dashboard (ROS topic idea: `/operator_auth`).
