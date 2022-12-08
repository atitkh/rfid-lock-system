// RFID Solenoid Lock

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define RESET_PIN 7 // pint to reset arduino
#define LED_G 5     // define green LED pin
#define LED_R 4     // define red LED
#define RELAY 3     // relay pin
#define BUZZER 2    // buzzer pin
#define ACCESS_DELAY 2000
#define DENIED_DELAY 300
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

void setup()
{
  Serial.begin(9600); // Initiate a serial communication
  SPI.begin();        // Initiate  SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  digitalWrite(RELAY, HIGH);

  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT); // set reset pin as output

  Serial.println("Put your card to the reader...");
  Serial.println();
}

void loop()
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // Show UID on serial monitor
  Serial.print(F("UID tag :"));
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "DA 30 5F 19" || content.substring(1) == "7A A4 F3 80" || content.substring(1) == "04 9C 7C D2 BA 5D 80" || content.substring(1) == "04 EE 7D D2 BA 5D 80") // change here the UID of the card/cards that you want to give access
  {
    if (content.substring(1) == "DA 30 5F 19" || content.substring(1) == "7A A4 F3 80")
    {
      grantAccessNoBuzzer();
    }
    else
    {
      grantAccessBuzzer();
    }
  }
  else if (content.substring(1) == "04 3E 7F D2 BA 5D 80")
  {
    bellBuzzer();
  }
  else if (content.substring(1,3) == "08") //workaround for apple wallet
  {
    grantAccessNoBuzzer();
  }
  else
  {
    denyAccess();
  }
  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

void grantAccessBuzzer()
{
  Serial.println("Authorized access");
  Serial.println();
  digitalWrite(RELAY, LOW);
  digitalWrite(LED_G, HIGH);
  tone(BUZZER, 2450, 100);
  delay(100);
  tone(BUZZER, 2500, 100);
  delay(ACCESS_DELAY);
  digitalWrite(RELAY, HIGH);
  digitalWrite(LED_G, LOW);
  resetArduino();
}

void grantAccessNoBuzzer()
{
  Serial.println("Authorized access");
  Serial.println();
  digitalWrite(RELAY, LOW);
  digitalWrite(LED_G, HIGH);
  delay(ACCESS_DELAY);
  digitalWrite(RELAY, HIGH);
  digitalWrite(LED_G, LOW);
  resetArduino();
}

void denyAccess()
{
  Serial.println("Access denied");
  digitalWrite(LED_R, HIGH);
  tone(BUZZER, 1000, 200);
  delay(DENIED_DELAY);
  tone(BUZZER, 1000, 200);
  delay(DENIED_DELAY);
  digitalWrite(LED_R, LOW);
  noTone(BUZZER);
  resetArduino();
}

void bellBuzzer()
{
  Serial.println("Ringing Bell");
  tone(BUZZER, 100);
  delay(1000);
  noTone(BUZZER);
  resetArduino();
}

void resetArduino()
{
  digitalWrite(RESET_PIN, LOW);
}
