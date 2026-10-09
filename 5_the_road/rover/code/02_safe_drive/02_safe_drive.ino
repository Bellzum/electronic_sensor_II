/*
  02_safe_drive — drive from the Serial Monitor with the 3 core safety rules.

  Type (115200, "Newline"):  f = forward   b = back   l = left   r = right   s = stop   x = reset E-stop
  Each command lasts only TIMEOUT_MS. To keep driving you must keep sending.
  That's the most important robot rule: NO NEWS = STOP.

  Rules in this sketch:
    1. Timeout: no command for 1 s -> stop
    2. Speed limit: never faster than MAX_SPEED
    3. E-stop button (GPIO18): latched, motors off until you type x
*/
const int PWMA = 25, AIN1 = 26, AIN2 = 27, PWMB = 32, BIN1 = 33, BIN2 = 19, STBY = 23;
const int ESTOP_PIN = 18;
const int MAX_SPEED = 150;                 // out of 255. Start slow!
const int TURN_SPEED = 110;
const unsigned long TIMEOUT_MS = 1000;

unsigned long lastCmd = 0;
bool estop = false;
char current = 's';

/** Purpose: drive one motor. Args: pins, speed -255..255. Returns: nothing. */
void motor(int pwm, int in1, int in2, int speed) {
  speed = constrain(speed, -MAX_SPEED, MAX_SPEED);   // SPEED LIMIT, always
  digitalWrite(in1, speed > 0);
  digitalWrite(in2, speed < 0);
  analogWrite(pwm, abs(speed));
}

/** Purpose: set both wheels. Args: left, right speeds. Returns: nothing. */
void drive(int left, int right) {
  motor(PWMA, AIN1, AIN2, left);
  motor(PWMB, BIN1, BIN2, right);
}

/** Purpose: stop both motors right now. Args: none. Returns: nothing. */
void stopMotors() { drive(0, 0); current = 's'; }

/** Purpose: turn a command letter into wheel speeds. Args: c. Returns: nothing. */
void apply(char c) {
  if (c == 'f') drive(MAX_SPEED, MAX_SPEED);
  else if (c == 'b') drive(-MAX_SPEED, -MAX_SPEED);
  else if (c == 'l') drive(-TURN_SPEED, TURN_SPEED);
  else if (c == 'r') drive(TURN_SPEED, -TURN_SPEED);
  else { stopMotors(); return; }
  current = c;
}

void setup() {
  Serial.begin(115200);
  int pins[] = { PWMA, AIN1, AIN2, PWMB, BIN1, BIN2, STBY };
  for (int p : pins) pinMode(p, OUTPUT);
  pinMode(ESTOP_PIN, INPUT_PULLUP);
  digitalWrite(STBY, HIGH);
  stopMotors();                                       // safe default
  Serial.println("f b l r s, x = reset E-stop");
}

void loop() {
  if (digitalRead(ESTOP_PIN) == LOW && !estop) { estop = true; stopMotors(); Serial.println("E-STOP!"); }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r' || c == ' ') continue;
    if (c == 'x') { estop = false; Serial.println("E-stop reset"); continue; }
    if (estop) { Serial.println("refused: E-stop active (type x)"); continue; }
    apply(c);
    lastCmd = millis();
    Serial.printf("%lu,cmd,%c\n", millis(), c);
  }

  if (current != 's' && millis() - lastCmd > TIMEOUT_MS) {   // NO NEWS = STOP
    stopMotors();
    Serial.printf("%lu,timeout,stop\n", millis());
  }
}
