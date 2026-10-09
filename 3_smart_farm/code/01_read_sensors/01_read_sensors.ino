/*
  01_read_sensors — read temperature, humidity and soil moisture on the ESP32.
  Libraries (Arduino IDE → Library Manager):
    - "DHT sensor library" by Adafruit (also installs "Adafruit Unified Sensor")
  Board: ESP32 Dev Module (install "esp32 by Espressif" in Boards Manager).
  Serial Monitor: 115200 baud.
*/
#include <DHT.h>

const int DHT_PIN = 4;
const int SOIL_PIN = 34;      // input-only pin, good for analog
DHT dht(DHT_PIN, DHT22);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("ms,temp_c,humidity,soil_raw");
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soil = analogRead(SOIL_PIN);        // 0–4095 on the ESP32. WETTER = LOWER for capacitive sensors

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read failed: check wiring and the 10k pull-up");
  } else {
    Serial.printf("%lu,%.1f,%.1f,%d\n", millis(), temp, hum, soil);
  }
  delay(2000);   // the DHT22 can only be read every 2 s
}
