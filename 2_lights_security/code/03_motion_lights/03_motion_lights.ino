/*
  03_motion_lights — street lights glow softly in the dark and go bright when something moves.
  PIR (HC-SR501) OUT -> D2. It outputs HIGH while it sees motion.
  Note: the PIR needs ~60 s to warm up after power on. Ignore it until then.
*/
const int LDR_PIN = A0;
const int PIR_PIN = 2;
const int LAMP1_PIN = 5;
const int LAMP2_PIN = 6;

const int DARK_ON  = 300;      // TODO: your numbers
const int DARK_OFF = 400;
const int DIM_LEVEL = 40;      // soft glow
const int BRIGHT_LEVEL = 255;  // full
const unsigned long BRIGHT_MS = 30000;   // stay bright 30 s after the last motion
const unsigned long PIR_WARMUP_MS = 60000;

bool dark = false;
unsigned long lastMotion = 0;
bool everMoved = false;

/** Purpose: set both lamps. Args: level 0–255. Returns: nothing. */
void setLamps(int level) {
  analogWrite(LAMP1_PIN, level);
  analogWrite(LAMP2_PIN, level);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
  pinMode(LAMP1_PIN, OUTPUT);
  pinMode(LAMP2_PIN, OUTPUT);
  Serial.println("ms,light,dark,motion,level");
}

void loop() {
  unsigned long now = millis();
  int light = analogRead(LDR_PIN);
  bool motion = (now > PIR_WARMUP_MS) && digitalRead(PIR_PIN) == HIGH;

  if (!dark && light < DARK_ON)  dark = true;
  if (dark  && light > DARK_OFF) dark = false;
  if (motion) { lastMotion = now; everMoved = true; }

  int level = 0;
  if (dark) {
    bool recent = everMoved && (now - lastMotion < BRIGHT_MS);
    level = recent ? BRIGHT_LEVEL : DIM_LEVEL;
  }
  setLamps(level);

  Serial.print(now);    Serial.print(",");
  Serial.print(light);  Serial.print(",");
  Serial.print(dark);   Serial.print(",");
  Serial.print(motion); Serial.print(",");
  Serial.println(level);
  delay(100);
}
