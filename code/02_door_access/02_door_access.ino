/*
  02_door_access — RFID door access with LCD + servo lock.
  Flow: idle -> card tapped -> check UID -> granted (unlock 3 s) or denied.
  Also prints a CSV line on Serial for each scan (future dashboard/log link).
*/
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN     10
#define RST_PIN    9
#define SERVO_PIN  3

const int LOCKED_ANGLE   = 0;
const int UNLOCKED_ANGLE = 87;
const unsigned long OPEN_MS = 3000;   // safety: always re-lock after 3 s

// TODO Day 3: replace with your card UIDs from 01_read_uid (uppercase, spaces)
const char* ALLOWED_UIDS[] = { "AA BB CC DD" };
const int NUM_ALLOWED = sizeof(ALLOWED_UIDS) / sizeof(ALLOWED_UIDS[0]);

MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);   // try 0x3F if screen stays blank

/**
 * Purpose: convert the current card UID to text like "AA BB CC DD".
 * Args: none (uses rfid.uid)
 * Returns: String UID
 */
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

/**
 * Purpose: check if a UID is in the allowed list.
 * Args: uid - card UID text
 * Returns: true if allowed
 */
bool isAllowed(const String& uid) {
  for (int i = 0; i < NUM_ALLOWED; i++) {
    if (uid == ALLOWED_UIDS[i]) return true;
  }
  return false;
}

/** Purpose: show the idle screen. Args: none. Returns: nothing. */
void showIdle() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Door LOCKED");
  lcd.setCursor(0, 1); lcd.print("Tap your card");
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lockServo.attach(SERVO_PIN);
  lockServo.write(LOCKED_ANGLE);   // start locked (safe default)
  lcd.init();
  lcd.backlight();
  showIdle();
  Serial.println("ms,uid,result");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  String uid = readUid();
  bool ok = isAllowed(uid);

  Serial.print(millis()); Serial.print(",");
  Serial.print(uid); Serial.print(",");
  Serial.println(ok ? "GRANTED" : "DENIED");

  lcd.clear();
  if (ok) {
    lcd.print("Access Granted");
    lcd.setCursor(0, 1); lcd.print("Welcome!");
    lockServo.write(UNLOCKED_ANGLE);
    delay(OPEN_MS);
    lockServo.write(LOCKED_ANGLE);
  } else {
    lcd.print("Access Denied");
    lcd.setCursor(0, 1); lcd.print(uid);
    delay(2000);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  showIdle();
}
