/*
  01_servo_limits — move the pan/tilt servos safely from the Serial Monitor.

  Type in Serial Monitor (115200, "Newline"):  pan tilt   e.g.  90 60
  The servos move SLOWLY (MAX_STEP degrees every 20 ms) and never past the limits,
  so the bracket and camera cable can't be damaged.

  Library: "ESP32Servo" by Kevin Harrington (Library Manager).
*/
#include <ESP32Servo.h>

const int PAN_PIN = 13, TILT_PIN = 14;
const int PAN_MIN = 20, PAN_MAX = 160;    // TODO: find the safe range of YOUR bracket
const int TILT_MIN = 40, TILT_MAX = 120;
const int MAX_STEP = 2;                   // degrees per 20 ms = smooth + slow

Servo pan, tilt;
int panNow = 90, tiltNow = 80;            // where the servos are
int panTarget = 90, tiltTarget = 80;      // where we want them

/**
 * Purpose: move one step closer to the target, never faster than MAX_STEP.
 * Args: now (current angle), target
 * Returns: new angle
 */
int stepToward(int now, int target) {
  if (target > now) return now + min(MAX_STEP, target - now);
  if (target < now) return now - min(MAX_STEP, now - target);
  return now;
}

void setup() {
  Serial.begin(115200);
  pan.attach(PAN_PIN);
  tilt.attach(TILT_PIN);
  pan.write(panNow);
  tilt.write(tiltNow);
  Serial.println("type: pan tilt   (e.g. 90 60)");
}

void loop() {
  if (Serial.available()) {
    int p = Serial.parseInt();
    int t = Serial.parseInt();
    while (Serial.available()) Serial.read();          // clear the rest of the line
    panTarget = constrain(p, PAN_MIN, PAN_MAX);        // LIMITS
    tiltTarget = constrain(t, TILT_MIN, TILT_MAX);
    Serial.printf("target pan=%d tilt=%d\n", panTarget, tiltTarget);
  }
  panNow = stepToward(panNow, panTarget);
  tiltNow = stepToward(tiltNow, tiltTarget);
  pan.write(panNow);
  tilt.write(tiltNow);
  delay(20);
}
