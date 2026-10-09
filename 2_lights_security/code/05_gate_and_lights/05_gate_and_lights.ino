/*
  05_gate_and_lights — Zone 1 + Zone 2 together on ONE Arduino Uno.

  The first zone-to-zone links in Mini Smart World:
    - Good card       -> gate unlocks 3 s AND welcome lights go bright for 30 s
    - 3 bad cards     -> ALARM (someone is guessing cards)
    - Motion while ARMED -> ALARM
    - ARM button      -> arm / disarm / reset alarm

  Pins: gate uses D3 (servo), D9–D13 (RC522), A4/A5 (LCD).
        lights use A0 (LDR), D2 (PIR), D4 (button), D5/D6 (lamps), D7 (buzzer), D8 (red LED).

  No delay() in the main loop: every job checks the clock (millis) so the
  gate, lights and alarm all keep working at the same time.

  Serial prints one CSV line per event. In Zone 3 this becomes MQTT messages.
*/
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- pins ----------
#define SS_PIN 10
#define RST_PIN 9
const int SERVO_PIN = 3;
const int LDR_PIN = A0;
const int PIR_PIN = 2;
const int BUTTON_PIN = 4;
const int LAMP1_PIN = 5;
const int LAMP2_PIN = 6;
const int BUZZER_PIN = 7;
const int ALARM_LED_PIN = 8;

// ---------- settings ----------
const int LOCKED_ANGLE = 0;
const int UNLOCKED_ANGLE = 87;
const unsigned long OPEN_MS = 3000;          // gate always relocks
const unsigned long WELCOME_MS = 30000;
const int MAX_BAD_CARDS = 3;
const int DARK_ON = 300, DARK_OFF = 400;     // TODO: your LDR numbers
const unsigned long SIREN_MAX_MS = 20000;
const unsigned long PIR_WARMUP_MS = 60000;

// TODO: your card UIDs from 1_smart_gate/code/01_read_uid
const char* ALLOWED_UIDS[] = { "AA BB CC DD" };
const int NUM_ALLOWED = sizeof(ALLOWED_UIDS) / sizeof(ALLOWED_UIDS[0]);

MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- world state ----------
enum Gate { LOCKED, UNLOCKED };
enum Security { DISARMED, ARMED, ALARM };
Gate gate = LOCKED;
Security security = DISARMED;
unsigned long gateSince = 0, securitySince = 0, welcomeUntil = 0;
int badCards = 0;
bool dark = false;
bool lastButton = HIGH;
unsigned long lastButtonChange = 0;

/** Purpose: print one CSV event line. Args: event name, value. Returns: nothing. */
void logEvent(const char* event, const String& value) {
  Serial.print(millis()); Serial.print(","); Serial.print(event); Serial.print(","); Serial.println(value);
}

/** Purpose: card UID as "AA BB CC DD". Args: none. Returns: String. */
String readUid() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid;
}

/** Purpose: is this UID allowed? Args: uid. Returns: true/false. */
bool isAllowed(const String& uid) {
  for (int i = 0; i < NUM_ALLOWED; i++) if (uid == ALLOWED_UIDS[i]) return true;
  return false;
}

/** Purpose: show two lines on the LCD. Args: top, bottom. Returns: nothing. */
void show(const char* top, const String& bottom) {
  lcd.clear(); lcd.print(top); lcd.setCursor(0, 1); lcd.print(bottom);
}

/** Purpose: change the security state and log it. Args: next. Returns: nothing. */
void setSecurity(Security next) {
  security = next;
  securitySince = millis();
  const char* names[] = { "DISARMED", "ARMED", "ALARM" };
  logEvent("security", names[next]);
  if (next == ALARM) show("!! ALARM !!", "Press ARM button");
  else show(gate == LOCKED ? "Gate LOCKED" : "Gate OPEN", names[next]);
}

/** Purpose: one debounced button press. Args: none. Returns: true once per press. */
bool buttonPressed() {
  bool now = digitalRead(BUTTON_PIN);
  if (now != lastButton && millis() - lastButtonChange > 50) {
    lastButtonChange = millis();
    lastButton = now;
    if (now == LOW) return true;
  }
  return false;
}

/** Purpose: read a card if there is one and react. Args: none. Returns: nothing. */
void handleCard() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;
  String uid = readUid();
  bool ok = isAllowed(uid);
  logEvent("card", uid + (ok ? " GRANTED" : " DENIED"));

  if (ok && security != ALARM) {
    badCards = 0;
    gate = UNLOCKED; gateSince = millis();
    lockServo.write(UNLOCKED_ANGLE);
    welcomeUntil = millis() + WELCOME_MS;          // LINK: gate -> lights
    logEvent("gate", "UNLOCKED");
    show("Access Granted", "Welcome!");
  } else {
    badCards++;
    show("Access Denied", uid);
    if (badCards >= MAX_BAD_CARDS && security != ALARM) setSecurity(ALARM);  // LINK: gate -> security
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lockServo.attach(SERVO_PIN);
  lockServo.write(LOCKED_ANGLE);                    // safe default
  lcd.init(); lcd.backlight();
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LAMP1_PIN, OUTPUT); pinMode(LAMP2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT); pinMode(ALARM_LED_PIN, OUTPUT);
  Serial.println("ms,event,value");
  setSecurity(DISARMED);
}

void loop() {
  unsigned long now = millis();

  // ---- gate ----
  handleCard();
  if (gate == UNLOCKED && now - gateSince > OPEN_MS) {
    gate = LOCKED;
    lockServo.write(LOCKED_ANGLE);
    logEvent("gate", "LOCKED");
    show("Gate LOCKED", "Tap your card");
  }

  // ---- security ----
  bool motion = now > PIR_WARMUP_MS && digitalRead(PIR_PIN) == HIGH;
  if (buttonPressed()) {
    if (security == DISARMED) setSecurity(ARMED);
    else { badCards = 0; setSecurity(DISARMED); }
  }
  if (security == ARMED && motion && now - securitySince > 10000) setSecurity(ALARM);

  // ---- lights ----
  int light = analogRead(LDR_PIN);
  if (!dark && light < DARK_ON) dark = true;
  if (dark && light > DARK_OFF) dark = false;
  int level = 0;
  if (now < welcomeUntil) level = 255;               // welcome lights, even in daytime
  else if (dark) level = motion ? 255 : 40;
  analogWrite(LAMP1_PIN, level);
  analogWrite(LAMP2_PIN, level);

  // ---- alarm output ----
  bool blink = (now / 250) % 2;
  if (security == ALARM) {
    digitalWrite(ALARM_LED_PIN, blink);
    digitalWrite(BUZZER_PIN, (now - securitySince < SIREN_MAX_MS) && blink);
  } else {
    digitalWrite(ALARM_LED_PIN, security == ARMED ? (now / 1000) % 2 : LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}
