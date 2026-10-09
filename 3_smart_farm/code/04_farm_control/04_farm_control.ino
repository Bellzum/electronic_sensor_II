/*
  04_farm_control — automatic fan + pump, with safety limits and the world E-stop.

  SENSE  : temperature, humidity, soil moisture
  DECIDE : fan if too hot (with hysteresis), short watering if too dry
  ACT    : MOSFET modules switch the fan and the pump

  Safety rules (same for every zone):
    1. World E-stop: message "true" on world/estop -> pump + fan OFF until "false"
    2. Silence means stop: no hub heartbeat for 10 s -> no watering
    3. Limits: pump max 3 s per run, at least 20 min between runs
    4. Log: every action is published (world/farm/...) and printed on Serial
    5. Manual first: dashboard can request pump/fan, but the limits still apply

  Topics
    publishes:  world/farm/sensors  {"temp":..,"humidity":..,"soil":..}
                world/farm/pump     ON | OFF
                world/farm/state    {"fan":true,"mode":"AUTO","estop":false,"pump_runs":2}
    listens to: world/estop          true | false
                world/hub/heartbeat  (anything, sent by the Pi every second)
                world/cmd/farm       pump | fan_on | fan_off | fan_auto

  Before uploading: copy secrets_example.h to secrets.h and fill it in.
*/
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "secrets.h"

// ---------- pins ----------
const int DHT_PIN = 4;
const int SOIL_PIN = 34;
const int FAN_PIN = 25;
const int PUMP_PIN = 26;
const int BUTTON_PIN = 27;       // local E-stop: press = stop, press again = clear

// ---------- settings ----------
const int DRY_RAW = 3000, WET_RAW = 1300;        // TODO: from 02_soil_calibration
const float FAN_ON_C = 30.0, FAN_OFF_C = 28.0;   // hysteresis
const int SOIL_DRY_PERCENT = 30;                 // water below this
const unsigned long PUMP_RUN_MS = 3000;
const unsigned long PUMP_GAP_MS = 20UL * 60UL * 1000UL;   // 20 minutes
const unsigned long HUB_TIMEOUT_MS = 10000;
const unsigned long SENSOR_MS = 5000;

DHT dht(DHT_PIN, DHT22);
WiFiClient wifi;
PubSubClient mqtt(wifi);

// ---------- state ----------
bool worldEstop = false, localEstop = false;
bool fanOn = false, pumpOn = false;
enum FanMode { FAN_AUTO, FAN_FORCE_ON, FAN_FORCE_OFF };
FanMode fanMode = FAN_AUTO;
bool pumpRequested = false;
unsigned long pumpStartedAt = 0, lastPumpEnd = 0, lastHeartbeat = 0, lastSensor = 0;
bool everPumped = false;
int pumpRuns = 0;
float lastTemp = NAN;
int lastSoil = -1;
bool lastButton = HIGH;
unsigned long lastButtonChange = 0;

/** Purpose: 0–100 % moisture from a raw reading. Args: raw. Returns: percent. */
int soilPercent(int raw) { return constrain(map(raw, DRY_RAW, WET_RAW, 0, 100), 0, 100); }

/** Purpose: true if any E-stop is active. Args: none. Returns: bool. */
bool stopped() { return worldEstop || localEstop; }

/** Purpose: publish the farm state as JSON. Args: none. Returns: nothing. */
void publishState() {
  const char* modes[] = { "AUTO", "FAN_ON", "FAN_OFF" };
  char msg[128];
  snprintf(msg, sizeof(msg), "{\"fan\":%s,\"pump\":%s,\"mode\":\"%s\",\"estop\":%s,\"pump_runs\":%d}",
           fanOn ? "true" : "false", pumpOn ? "true" : "false", modes[fanMode], stopped() ? "true" : "false", pumpRuns);
  mqtt.publish("world/farm/state", msg);
  Serial.println(msg);
}

/** Purpose: switch the pump and report it. Args: on. Returns: nothing. */
void setPump(bool on) {
  if (on == pumpOn) return;
  pumpOn = on;
  digitalWrite(PUMP_PIN, on ? HIGH : LOW);
  if (on) { pumpStartedAt = millis(); pumpRuns++; everPumped = true; }
  else lastPumpEnd = millis();
  mqtt.publish("world/farm/pump", on ? "ON" : "OFF");
  publishState();
}

/** Purpose: switch the fan and report it. Args: on. Returns: nothing. */
void setFan(bool on) {
  if (on == fanOn) return;
  fanOn = on;
  digitalWrite(FAN_PIN, on ? HIGH : LOW);
  publishState();
}

/**
 * Purpose: handle every incoming MQTT message.
 * Args: topic, payload bytes, length
 * Returns: nothing
 */
void onMessage(char* topic, byte* payload, unsigned int length) {
  String t = topic;
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();

  if (t == "world/hub/heartbeat") { lastHeartbeat = millis(); return; }
  if (t == "world/estop") {
    worldEstop = (msg == "true");
    Serial.printf("world E-stop = %d\n", worldEstop);
    publishState();
    return;
  }
  if (t == "world/cmd/farm") {
    if (msg == "pump") pumpRequested = true;
    else if (msg == "fan_on") fanMode = FAN_FORCE_ON;
    else if (msg == "fan_off") fanMode = FAN_FORCE_OFF;
    else if (msg == "fan_auto") fanMode = FAN_AUTO;
    publishState();
  }
}

/** Purpose: keep Wi-Fi and MQTT connected (pump OFF while offline). Args: none. Returns: nothing. */
void ensureConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    setPump(false);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) delay(250);
    if (WiFi.status() != WL_CONNECTED) return;
  }
  if (!mqtt.connected()) {
    if (mqtt.connect("farm-esp32")) {
      mqtt.subscribe("world/estop");
      mqtt.subscribe("world/hub/heartbeat");
      mqtt.subscribe("world/cmd/farm");
      publishState();
    }
  }
}

/** Purpose: one debounced press of the local E-stop button. Args: none. Returns: true once per press. */
bool buttonPressed() {
  bool now = digitalRead(BUTTON_PIN);
  if (now != lastButton && millis() - lastButtonChange > 50) {
    lastButtonChange = millis();
    lastButton = now;
    if (now == LOW) return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  pinMode(FAN_PIN, OUTPUT);  digitalWrite(FAN_PIN, LOW);     // safe default: everything OFF
  pinMode(PUMP_PIN, OUTPUT); digitalWrite(PUMP_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  dht.begin();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMessage);
}

void loop() {
  unsigned long now = millis();
  ensureConnected();
  mqtt.loop();

  if (buttonPressed()) { localEstop = !localEstop; Serial.printf("local E-stop = %d\n", localEstop); publishState(); }

  // ---- SENSE (every 5 s) ----
  if (now - lastSensor > SENSOR_MS) {
    lastSensor = now;
    float t = dht.readTemperature(), h = dht.readHumidity();
    int soil = soilPercent(analogRead(SOIL_PIN));
    if (!isnan(t)) {
      lastTemp = t; lastSoil = soil;
      char msg[96];
      snprintf(msg, sizeof(msg), "{\"temp\":%.1f,\"humidity\":%.1f,\"soil\":%d}", t, h, soil);
      mqtt.publish("world/farm/sensors", msg);
    }
  }

  // ---- SAFETY FIRST ----
  if (stopped()) { setPump(false); setFan(false); pumpRequested = false; return; }
  if (pumpOn && now - pumpStartedAt > PUMP_RUN_MS) setPump(false);      // limit: max run time

  // ---- DECIDE + ACT: fan ----
  if (fanMode == FAN_FORCE_ON) setFan(true);
  else if (fanMode == FAN_FORCE_OFF) setFan(false);
  else if (!isnan(lastTemp)) {
    if (!fanOn && lastTemp > FAN_ON_C) setFan(true);
    if (fanOn && lastTemp < FAN_OFF_C) setFan(false);
  }

  // ---- DECIDE + ACT: pump ----
  bool hubAlive = now - lastHeartbeat < HUB_TIMEOUT_MS;
  bool gapOk = !everPumped || now - lastPumpEnd > PUMP_GAP_MS;
  bool tooDry = lastSoil >= 0 && lastSoil < SOIL_DRY_PERCENT;
  if (!pumpOn && hubAlive && gapOk && (tooDry || pumpRequested)) setPump(true);
  if (pumpRequested && !gapOk) Serial.println("pump request refused: wait for the 20 min gap");
  pumpRequested = false;
}
