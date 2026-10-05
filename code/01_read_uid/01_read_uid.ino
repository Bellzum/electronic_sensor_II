/*
  01_read_uid — Step 1: read and print RFID card UIDs.
  Purpose: find the unique ID of each card so we can whitelist it.
  Wiring: see docs/wiring.md (RC522 on 3.3V!)
  Open Serial Monitor at 9600 baud, tap a card.
*/
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Tap a card...");
}

void loop() {
  // Wait until a new card is present and readable
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) Serial.print(" ");
  }
  Serial.println();

  rfid.PICC_HaltA();  // stop reading this card
}
