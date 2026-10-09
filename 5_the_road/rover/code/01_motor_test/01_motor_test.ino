/*
  01_motor_test — check each motor turns the right way.  ⚠️ WHEELS OFF THE GROUND!
  Put the rover on a box so the wheels spin in the air.

  Sequence (repeats): left forward, left back, right forward, right back, both stop.
  If a wheel turns the wrong way, swap that motor's two wires on the driver.
  Driver: TB6612FNG. Board: ESP32 Dev Module.
*/
const int PWMA = 25, AIN1 = 26, AIN2 = 27;   // motor A = LEFT
const int PWMB = 32, BIN1 = 33, BIN2 = 19;   // motor B = RIGHT
const int STBY = 23;
const int TEST_SPEED = 120;                  // 0–255, slow on purpose

/**
 * Purpose: drive one motor.
 * Args: pwm/in1/in2 pins, speed -255..255 (negative = backward)
 * Returns: nothing
 */
void motor(int pwm, int in1, int in2, int speed) {
  digitalWrite(in1, speed > 0);
  digitalWrite(in2, speed < 0);
  analogWrite(pwm, abs(speed));
}

void setup() {
  Serial.begin(115200);
  int pins[] = { PWMA, AIN1, AIN2, PWMB, BIN1, BIN2, STBY };
  for (int p : pins) pinMode(p, OUTPUT);
  digitalWrite(STBY, HIGH);                  // driver on
}

void loop() {
  Serial.println("LEFT forward");  motor(PWMA, AIN1, AIN2, TEST_SPEED);  delay(1500);
  Serial.println("LEFT back");     motor(PWMA, AIN1, AIN2, -TEST_SPEED); delay(1500);
  motor(PWMA, AIN1, AIN2, 0);
  Serial.println("RIGHT forward"); motor(PWMB, BIN1, BIN2, TEST_SPEED);  delay(1500);
  Serial.println("RIGHT back");    motor(PWMB, BIN1, BIN2, -TEST_SPEED); delay(1500);
  motor(PWMB, BIN1, BIN2, 0);
  Serial.println("STOP");          delay(3000);
}
