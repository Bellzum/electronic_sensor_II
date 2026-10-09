/*
  03_obstacle_tilt — 02_safe_drive + two sensors that can say NO.

    Ultrasonic (HC-SR04): refuse "forward" when something is closer than STOP_CM
    IMU (MPU6050):        stop everything when tilted more than MAX_TILT_DEG (tipped over / lifted)

  Commands (Serial, 115200):  f b l r s, x = reset E-stop, ? = print sensors
  MPU6050 is read directly over I2C (no library needed).
  ⚠️ HC-SR04 ECHO is 5 V: use the 1 kΩ / 2 kΩ divider before GPIO17.
*/
#include <Wire.h>

const int PWMA = 25, AIN1 = 26, AIN2 = 27, PWMB = 32, BIN1 = 33, BIN2 = 19, STBY = 23;
const int ESTOP_PIN = 18, TRIG_PIN = 16, ECHO_PIN = 17;
const int MAX_SPEED = 150, TURN_SPEED = 110;
const unsigned long TIMEOUT_MS = 1000;
const float STOP_CM = 20.0;
const float MAX_TILT_DEG = 30.0;
const int MPU_ADDR = 0x68;

unsigned long lastCmd = 0, lastSense = 0;
bool estop = false;
char current = 's';
float distanceCm = -1, tiltDeg = 0;

/** Purpose: drive one motor with the speed limit. Args: pins, speed. Returns: nothing. */
void motor(int pwm, int in1, int in2, int speed) {
  speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
  digitalWrite(in1, speed > 0);
  digitalWrite(in2, speed < 0);
  analogWrite(pwm, abs(speed));
}
void drive(int left, int right) { motor(PWMA, AIN1, AIN2, left); motor(PWMB, BIN1, BIN2, right); }
void stopMotors() { drive(0, 0); current = 's'; }

/**
 * Purpose: measure distance with the HC-SR04.
 * Args: none
 * Returns: distance in cm, or -1 if no echo (nothing in range)
 */
float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long us = pulseIn(ECHO_PIN, HIGH, 25000);        // 25 ms timeout (~4 m)
  return us == 0 ? -1 : us * 0.0343 / 2.0;
}

/** Purpose: wake the MPU6050. Args: none. Returns: true if it answered. */
bool imuBegin() {
  Wire.begin(21, 22);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); Wire.write(0);                 // power register: wake up
  return Wire.endTransmission() == 0;
}

/**
 * Purpose: tilt angle from the accelerometer (0 = flat, 90 = on its side).
 * Args: none
 * Returns: degrees, or -1 if the IMU didn't answer
 */
float readTiltDeg() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                                // first accelerometer register
  if (Wire.endTransmission(false) != 0) return -1;
  Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)6);
  if (Wire.available() < 6) return -1;
  uint8_t b[6];
  for (int i = 0; i < 6; i++) b[i] = Wire.read();  // read in order: high byte, low byte
  int16_t ax = (b[0] << 8) | b[1];
  int16_t ay = (b[2] << 8) | b[3];
  int16_t az = (b[4] << 8) | b[5];
  float g = sqrt((float)ax * ax + (float)ay * ay + (float)az * az);
  if (g < 1) return -1;
  return acos(constrain(az / g, -1.0f, 1.0f)) * 180.0 / PI;
}

/** Purpose: command letter -> wheels, with the obstacle rule. Args: c. Returns: nothing. */
void apply(char c) {
  if (c == 'f' && distanceCm > 0 && distanceCm < STOP_CM) {
    stopMotors(); Serial.printf("%lu,refused,obstacle %.0f cm\n", millis(), distanceCm); return;
  }
  if (c == 'f') drive(MAX_SPEED, MAX_SPEED);
  else if (c == 'b') drive(-MAX_SPEED, -MAX_SPEED);
  else if (c == 'l') drive(-TURN_SPEED, TURN_SPEED);
  else if (c == 'r') drive(TURN_SPEED, -TURN_SPEED);
  else { stopMotors(); return; }
  current = c;
}

void setup() {
  Serial.begin(115200);
  int pins[] = { PWMA, AIN1, AIN2, PWMB, BIN1, BIN2, STBY, TRIG_PIN };
  for (int p : pins) pinMode(p, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ESTOP_PIN, INPUT_PULLUP);
  digitalWrite(STBY, HIGH);
  stopMotors();
  Serial.println(imuBegin() ? "IMU ok" : "IMU NOT FOUND: check SDA 21 / SCL 22");
  Serial.println("f b l r s, x = reset, ? = sensors");
}

void loop() {
  unsigned long now = millis();
  if (now - lastSense > 60) {                      // sense ~16 times per second
    lastSense = now;
    distanceCm = readDistanceCm();
    tiltDeg = readTiltDeg();
  }

  if (digitalRead(ESTOP_PIN) == LOW && !estop) { estop = true; stopMotors(); Serial.println("E-STOP!"); }
  if (tiltDeg > MAX_TILT_DEG && !estop) { estop = true; stopMotors(); Serial.printf("E-STOP: tilted %.0f deg\n", tiltDeg); }
  if (current == 'f' && distanceCm > 0 && distanceCm < STOP_CM) { stopMotors(); Serial.println("obstacle: stopped"); }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r' || c == ' ') continue;
    if (c == '?') { Serial.printf("distance=%.1f cm tilt=%.1f deg estop=%d\n", distanceCm, tiltDeg, estop); continue; }
    if (c == 'x') { if (tiltDeg <= MAX_TILT_DEG) { estop = false; Serial.println("E-stop reset"); } continue; }
    if (estop) { Serial.println("refused: E-stop active"); continue; }
    apply(c);
    lastCmd = now;
  }

  if (current != 's' && now - lastCmd > TIMEOUT_MS) { stopMotors(); Serial.println("timeout: stop"); }
}
