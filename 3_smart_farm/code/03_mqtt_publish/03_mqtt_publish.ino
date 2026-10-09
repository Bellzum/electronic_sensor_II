/*
  03_mqtt_publish — the farm joins the world network.

  Every 5 s it publishes JSON to the topic  world/farm/sensors
    {"temp":24.1,"humidity":55.0,"soil":38}
  soil is a 0–100 % moisture value from your calibration numbers.

  Libraries: "PubSubClient" by Nick O'Leary, "DHT sensor library" by Adafruit.
  Before uploading: copy secrets_example.h to secrets.h and fill it in.
  Test on the Pi:  mosquitto_sub -h localhost -t 'world/#' -v
*/
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "secrets.h"

const int DHT_PIN = 4;
const int SOIL_PIN = 34;
const int DRY_RAW = 3000;   // TODO: average "dry" from 02_soil_calibration
const int WET_RAW = 1300;   // TODO: average "wet"
const unsigned long PUBLISH_MS = 5000;

DHT dht(DHT_PIN, DHT22);
WiFiClient wifi;
PubSubClient mqtt(wifi);
unsigned long lastPublish = 0;

/**
 * Purpose: turn a raw soil reading into 0–100 % moisture.
 * Args: raw - analogRead value
 * Returns: percent, 0 = dry, 100 = wet
 */
int soilPercent(int raw) {
  return constrain(map(raw, DRY_RAW, WET_RAW, 0, 100), 0, 100);
}

/** Purpose: connect (or reconnect) Wi-Fi and MQTT. Args: none. Returns: nothing. */
void ensureConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.print(" ok, IP "); Serial.println(WiFi.localIP());
  }
  while (!mqtt.connected()) {
    Serial.print("MQTT...");
    if (mqtt.connect("farm-esp32")) Serial.println(" ok");
    else { Serial.printf(" failed (%d), retry in 2 s\n", mqtt.state()); delay(2000); }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  ensureConnected();
  mqtt.loop();
  if (millis() - lastPublish < PUBLISH_MS) return;
  lastPublish = millis();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soil = soilPercent(analogRead(SOIL_PIN));
  if (isnan(temp) || isnan(hum)) { Serial.println("DHT read failed"); return; }

  char msg[96];
  snprintf(msg, sizeof(msg), "{\"temp\":%.1f,\"humidity\":%.1f,\"soil\":%d}", temp, hum, soil);
  mqtt.publish("world/farm/sensors", msg);
  Serial.println(msg);
}
