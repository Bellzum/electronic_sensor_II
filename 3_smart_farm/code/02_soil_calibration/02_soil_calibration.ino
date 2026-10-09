/*
  02_soil_calibration — your first real science experiment.

  Goal: find what "dry" and "wet" mean for YOUR soil sensor.
  How:
    1. Open Serial Monitor (115200), set line ending to "Newline".
    2. Put the sensor in DRY soil, type  d  and press Enter -> 20 readings.
    3. Water the soil well, wait 1 minute, type  w  -> 20 readings.
    4. Optional: sensor in a glass of water, type  x  -> 20 readings.
    5. Copy the CSV lines into experiments/soil_calibration.csv
  Then compute the average of each group and set DRY_RAW / WET_RAW in 04_farm_control.
*/
const int SOIL_PIN = 34;

/**
 * Purpose: take 20 readings, one every 500 ms, and print them with a label.
 * Args: label - "dry", "wet" or "water"
 * Returns: nothing (prints CSV + the average)
 */
void takeReadings(const char* label) {
  long sum = 0;
  for (int i = 1; i <= 20; i++) {
    int v = analogRead(SOIL_PIN);
    sum += v;
    Serial.printf("%s,%d,%d\n", label, i, v);
    delay(500);
  }
  Serial.printf("# average %s = %ld\n", label, sum / 20);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("label,reading,raw");
  Serial.println("# type d (dry), w (wet) or x (water) then Enter");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'd') takeReadings("dry");
    if (c == 'w') takeReadings("wet");
    if (c == 'x') takeReadings("water");
  }
}
