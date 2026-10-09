/*
  02_street_lights — street lights turn on when it gets dark.
  Uses HYSTERESIS: two thresholds so the lights don't flicker at dusk.
    light < DARK_ON   -> lights ON
    light > DARK_OFF  -> lights OFF
    in between        -> keep the current state
  Brightness uses PWM (analogWrite 0–255) on pins D5 and D6.
*/
const int LDR_PIN = A0;
const int LAMP1_PIN = 5;   // PWM pin
const int LAMP2_PIN = 6;   // PWM pin

// TODO: replace with your numbers from 01_ldr_read
const int DARK_ON  = 300;
const int DARK_OFF = 400;   // must be bigger than DARK_ON

bool lightsOn = false;

/**
 * Purpose: set both street lamps to one brightness.
 * Args: level - 0 (off) to 255 (full)
 * Returns: nothing
 */
void setLamps(int level) {
  analogWrite(LAMP1_PIN, level);
  analogWrite(LAMP2_PIN, level);
}

void setup() {
  Serial.begin(9600);
  pinMode(LAMP1_PIN, OUTPUT);
  pinMode(LAMP2_PIN, OUTPUT);
  setLamps(0);
  Serial.println("ms,light,lights_on,level");
}

void loop() {
  int light = analogRead(LDR_PIN);                 // SENSE

  if (!lightsOn && light < DARK_ON)  lightsOn = true;    // DECIDE
  if (lightsOn  && light > DARK_OFF) lightsOn = false;

  // Darker room -> brighter lamps (map 0..DARK_OFF to 255..60)
  int level = 0;
  if (lightsOn) level = constrain(map(light, 0, DARK_OFF, 255, 60), 60, 255);
  setLamps(level);                                  // ACT

  Serial.print(millis()); Serial.print(",");
  Serial.print(light);    Serial.print(",");
  Serial.print(lightsOn); Serial.print(",");
  Serial.println(level);
  delay(100);
}
