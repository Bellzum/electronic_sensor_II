/*
  04_mqtt_drive — the Delivery Rover joins the world.

  Listens to:
    world/cmd/rover   forward | backward | left | right | stop | reset
                      (the dashboard re-sends every 300 ms while you HOLD a button)
    world/estop       true | false
  Publishes every 500 ms:
    world/rover/state {"state":"DRIVING","cmd":"f","distance":42.0,"tilt":3.1,"estop":false}
    state = IDLE | DRIVING | BLOCKED | ESTOP

  Safety: same rules as 03_obstacle_tilt, plus
    - world E-stop stops the rover
    - lost Wi-Fi or MQTT = stop (that's just the 1 s timeout doing its job)
  Copy secrets_example.h to secrets.h first.
*/
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

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


WiFiClient wifi;
PubSubClient mqtt(wifi);
bool worldEstop = false;
bool blocked = false;
unsigned long lastReport = 0;

/** Purpose: name of the rover state. Args: none. Returns: text. */
const char* stateName() {
  if (estop || worldEstop) return "ESTOP";
  if (blocked) return "BLOCKED";
  return current == 's' ? "IDLE" : "DRIVING";
}

/** Purpose: publish the rover state. Args: none. Returns: nothing. */
void report() {
  char msg[128];
  snprintf(msg, sizeof(msg), "{\"state\":\"%s\",\"cmd\":\"%c\",\"distance\":%.1f,\"tilt\":%.1f,\"estop\":%s}",
           stateName(), current, distanceCm, tiltDeg, (estop || worldEstop) ? "true" : "false");
  mqtt.publish("world/rover/state", msg);
}

/** Purpose: handle MQTT messages. Args: topic, payload, length. Returns: nothing. */
void onMessage(char* topic, byte* payload, unsigned int length) {
  String t = topic, msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  if (t == "world/estop") { worldEstop = (msg == "true"); if (worldEstop) stopMotors(); report(); return; }
  if (t != "world/cmd/rover") return;
  if (msg == "reset") { if (tiltDeg <= MAX_TILT_DEG) estop = false; report(); return; }
  if (estop || worldEstop) { stopMotors(); return; }
  char c = 's';
  if (msg == "forward") c = 'f'; else if (msg == "backward") c = 'b';
  else if (msg == "left") c = 'l'; else if (msg == "right") c = 'r';
  blocked = (c == 'f' && distanceCm > 0 && distanceCm < STOP_CM);
  apply(c);
  lastCmd = millis();
}

/** Purpose: keep Wi-Fi + MQTT connected, stopping while offline. Args: none. Returns: nothing. */
void ensureConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    stopMotors();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) delay(250);
    if (WiFi.status() != WL_CONNECTED) return;
  }
  if (!mqtt.connected()) {
    stopMotors();
    if (mqtt.connect("rover-esp32")) { mqtt.subscribe("world/cmd/rover"); mqtt.subscribe("world/estop"); }
  }
}

void setup() {
  Serial.begin(115200);
  int pins[] = { PWMA, AIN1, AIN2, PWMB, BIN1, BIN2, STBY, TRIG_PIN };
  for (int p : pins) pinMode(p, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ESTOP_PIN, INPUT_PULLUP);
  digitalWrite(STBY, HIGH);
  stopMotors();
  Serial.println(imuBegin() ? "IMU ok" : "IMU NOT FOUND");
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMessage);
}

void loop() {
  unsigned long now = millis();
  ensureConnected();
  mqtt.loop();

  if (now - lastSense > 60) { lastSense = now; distanceCm = readDistanceCm(); tiltDeg = readTiltDeg(); }

  if (digitalRead(ESTOP_PIN) == LOW && !estop) { estop = true; stopMotors(); report(); }
  if (tiltDeg > MAX_TILT_DEG && !estop) { estop = true; stopMotors(); report(); }
  if (worldEstop && current != 's') stopMotors();
  if (current == 'f' && distanceCm > 0 && distanceCm < STOP_CM) { stopMotors(); blocked = true; }
  if (current != 's' && now - lastCmd > TIMEOUT_MS) stopMotors();     // NO NEWS = STOP
  if (current != 's') blocked = false;

  if (now - lastReport > 500) { lastReport = now; report(); }
}
