/*
  02_pan_tilt_mqtt — the Watchtower listens to the world and aims the camera.

  Listens to:
    world/cmd/tower        GATE | FARM | ROAD | HOME | "pan,tilt" (e.g. "100,70")
    world/gate/card        any card scan  -> aim at GATE
    world/security/alarm   true           -> aim at GATE
    world/estop            true           -> FREEZE (servos hold still) until false
  Publishes:
    world/tower/position   {"pan":100,"tilt":70,"preset":"GATE","estop":false}

  The Raspberry Pi script pi/tower_camera.py takes the photo once the tower has turned.
  Libraries: ESP32Servo, PubSubClient. Copy secrets_example.h to secrets.h first.
*/
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include "secrets.h"

const int PAN_PIN = 13, TILT_PIN = 14, BUTTON_PIN = 27;
const int PAN_MIN = 20, PAN_MAX = 160, TILT_MIN = 40, TILT_MAX = 120;
const int MAX_STEP = 2;

// TODO: aim the camera by hand with 01_servo_limits, then write the angles here
struct Preset { const char* name; int pan; int tilt; };
Preset PRESETS[] = { {"HOME", 90, 80}, {"GATE", 40, 70}, {"FARM", 140, 70}, {"ROAD", 90, 60} };
const int NUM_PRESETS = sizeof(PRESETS) / sizeof(PRESETS[0]);

Servo pan, tilt;
WiFiClient wifi;
PubSubClient mqtt(wifi);
int panNow = 90, tiltNow = 80, panTarget = 90, tiltTarget = 80;
String presetName = "HOME";
bool worldEstop = false, localEstop = false;
bool reportPending = true;
bool lastButton = HIGH;
unsigned long lastButtonChange = 0;

/** Purpose: set a new target inside the limits. Args: p, t, name. Returns: nothing. */
void aim(int p, int t, const String& name) {
  panTarget = constrain(p, PAN_MIN, PAN_MAX);
  tiltTarget = constrain(t, TILT_MIN, TILT_MAX);
  presetName = name;
  reportPending = true;
}

/** Purpose: aim at a named preset. Args: name. Returns: true if found. */
bool aimPreset(const String& name) {
  for (int i = 0; i < NUM_PRESETS; i++) {
    if (name == PRESETS[i].name) { aim(PRESETS[i].pan, PRESETS[i].tilt, name); return true; }
  }
  return false;
}

/** Purpose: publish where the tower points. Args: none. Returns: nothing. */
void publishPosition() {
  char msg[96];
  snprintf(msg, sizeof(msg), "{\"pan\":%d,\"tilt\":%d,\"preset\":\"%s\",\"estop\":%s}",
           panNow, tiltNow, presetName.c_str(), (worldEstop || localEstop) ? "true" : "false");
  mqtt.publish("world/tower/position", msg);
  Serial.println(msg);
}

/** Purpose: react to MQTT messages. Args: topic, payload, length. Returns: nothing. */
void onMessage(char* topic, byte* payload, unsigned int length) {
  String t = topic, msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  if (t == "world/estop") { worldEstop = (msg == "true"); reportPending = true; return; }
  if (t == "world/gate/card") { aimPreset("GATE"); return; }
  if (t == "world/security/alarm" && msg == "true") { aimPreset("GATE"); return; }
  if (t == "world/cmd/tower") {
    if (aimPreset(msg)) return;
    int comma = msg.indexOf(',');
    if (comma > 0) aim(msg.substring(0, comma).toInt(), msg.substring(comma + 1).toInt(), "MANUAL");
  }
}

/** Purpose: keep Wi-Fi + MQTT connected. Args: none. Returns: nothing. */
void ensureConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) delay(250);
    if (WiFi.status() != WL_CONNECTED) return;
  }
  if (!mqtt.connected() && mqtt.connect("tower-esp32")) {
    mqtt.subscribe("world/cmd/tower");
    mqtt.subscribe("world/gate/card");
    mqtt.subscribe("world/security/alarm");
    mqtt.subscribe("world/estop");
    reportPending = true;
  }
}

/** Purpose: one step closer to the target. Args: now, target. Returns: new angle. */
int stepToward(int now, int target) {
  if (target > now) return now + min(MAX_STEP, target - now);
  if (target < now) return now - min(MAX_STEP, now - target);
  return now;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pan.attach(PAN_PIN); tilt.attach(TILT_PIN);
  pan.write(panNow); tilt.write(tiltNow);
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMessage);
}

void loop() {
  ensureConnected();
  mqtt.loop();

  bool b = digitalRead(BUTTON_PIN);
  if (b != lastButton && millis() - lastButtonChange > 50) {
    lastButtonChange = millis(); lastButton = b;
    if (b == LOW) { localEstop = !localEstop; reportPending = true; }
  }

  if (!(worldEstop || localEstop)) {          // E-stop = freeze where we are
    panNow = stepToward(panNow, panTarget);
    tiltNow = stepToward(tiltNow, tiltTarget);
    pan.write(panNow); tilt.write(tiltNow);
  }
  bool arrived = panNow == panTarget && tiltNow == tiltTarget;
  if (reportPending && (arrived || worldEstop || localEstop)) { publishPosition(); reportPending = false; }
  delay(20);
}
