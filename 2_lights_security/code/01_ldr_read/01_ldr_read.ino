/*
  01_ldr_read — read the light sensor (LDR) and print the value.
  Wiring: LDR between 5V and A0, 10 kΩ between A0 and GND.
  Bright room = high number, dark = low number (0–1023 on the Uno).
  Goal: find YOUR "dark" and "bright" numbers and write them in experiments/test_log.csv.
*/
const int LDR_PIN = A0;

int minSeen = 1023;   // darkest value seen so far
int maxSeen = 0;      // brightest value seen so far

void setup() {
  Serial.begin(9600);
  Serial.println("ms,light,min,max");
}

void loop() {
  int light = analogRead(LDR_PIN);      // SENSE
  if (light < minSeen) minSeen = light;
  if (light > maxSeen) maxSeen = light;

  Serial.print(millis()); Serial.print(",");
  Serial.print(light);    Serial.print(",");
  Serial.print(minSeen);  Serial.print(",");
  Serial.println(maxSeen);
  delay(200);   // 5 readings per second is plenty for light
}
