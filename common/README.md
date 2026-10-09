# common/

Shared rules for every zone.

## Topic names
The full list is in the [main README](../README.md#how-the-zones-talk). Pattern: `world/<zone>/<thing>`, commands on `world/cmd/<zone>`.

## Shared files
| File | Where it's copied | Why |
|---|---|---|
| `secrets_example.h` | every ESP32 sketch folder | Copy to `secrets.h` and add your Wi‑Fi + Pi IP. Never committed |

Arduino can only include files from the sketch's own folder, so each ESP32 sketch keeps its own copy of `secrets_example.h`.

## Safety code pattern (every moving thing)
```cpp
if (estop) { stopEverything(); return; }               // 1. E-stop wins
if (millis() - lastCmd > TIMEOUT_MS) stopEverything(); // 2. no news = stop
speed = constrain(speed, -MAX_SPEED, MAX_SPEED);       // 3. limits
logEvent(...);                                         // 4. log it
```

Rule: if two zones need the same code, it moves here.
