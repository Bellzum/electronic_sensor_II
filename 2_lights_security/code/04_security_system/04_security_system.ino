/*
  04_security_system — a small alarm with a STATE MACHINE.

  States:
    DISARMED  -> nothing happens (lights work as normal)
    ARMED     -> motion = intruder!
    ALARM     -> siren + flashing red LED (latched: stays until you press the button)

  Button (D4 to GND) toggles DISARMED <-> ARMED, and stops an ALARM.
  Safety/limits: the siren goes quiet after SIREN_MAX_MS so it never beeps forever,
  but the red LED keeps flashing until a person resets it.
*/
const int PIR_PIN = 2;
const int BUTTON_PIN = 4;
const int BUZZER_PIN = 7;
const int ALARM_LED_PIN = 8;

const unsigned long SIREN_MAX_MS = 20000;     // 20 s of noise max
const unsigned long ARM_DELAY_MS = 10000;     // 10 s to walk away after arming
const unsigned long PIR_WARMUP_MS = 60000;

enum State { DISARMED, ARMED, ALARM };
State state = DISARMED;
unsigned long stateSince = 0;

bool lastButton = HIGH;
unsigned long lastButtonChange = 0;

/** Purpose: readable state name for Serial. Args: s. Returns: text. */
const char* stateName(State s) {
  if (s == DISARMED) return "DISARMED";
  if (s == ARMED) return "ARMED";
  return "ALARM";
}

/** Purpose: change state and log it. Args: next state. Returns: nothing. */
void setState(State next) {
  state = next;
  stateSince = millis();
  Serial.print(millis()); Serial.print(",state,"); Serial.println(stateName(state));
}

/**
 * Purpose: detect one clean button press (debounced).
 * Args: none
 * Returns: true once per press
 */
bool buttonPressed() {
  bool now = digitalRead(BUTTON_PIN);
  if (now != lastButton && millis() - lastButtonChange > 50) {
    lastButtonChange = millis();
    lastButton = now;
    if (now == LOW) return true;     // pressed (INPUT_PULLUP)
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ALARM_LED_PIN, OUTPUT);
  Serial.println("ms,event,value");
  setState(DISARMED);
}

void loop() {
  unsigned long now = millis();
  bool motion = (now > PIR_WARMUP_MS) && digitalRead(PIR_PIN) == HIGH;
  bool pressed = buttonPressed();

  // ---- DECIDE ----
  switch (state) {
    case DISARMED:
      if (pressed) setState(ARMED);
      break;
    case ARMED:
      if (pressed) setState(DISARMED);
      else if (motion && now - stateSince > ARM_DELAY_MS) setState(ALARM);
      break;
    case ALARM:
      if (pressed) setState(DISARMED);   // a person must reset it
      break;
  }

  // ---- ACT ----
  bool blink = (now / 250) % 2;          // toggles every 250 ms
  if (state == ALARM) {
    digitalWrite(ALARM_LED_PIN, blink);
    bool sirenAllowed = now - stateSince < SIREN_MAX_MS;
    digitalWrite(BUZZER_PIN, sirenAllowed && blink);
  } else if (state == ARMED) {
    digitalWrite(ALARM_LED_PIN, (now / 1000) % 2);   // slow blink = armed
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    digitalWrite(ALARM_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  delay(10);
}
