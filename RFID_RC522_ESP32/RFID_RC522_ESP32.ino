#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 4    // SDA pin
#define RST_PIN 15   // RST pin
#define BTN_PIN 5    // Button pin
#define LED_PIN 27    // LED pin (for visual feedback)
#define BUZZER_PIN 2

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Create LCD instance (change the address if needed)

int buttonState = 0;
bool registrationMode = false;
String registeredCardUID;

void setup() {
  lcd.init();                 // initialize the lcd 
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.backlight(); // Turn on the backlight

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO RFID");
  lcd.setCursor(0, 1);
  lcd.print("SCANNING PROJECT");
  delay(2000);
  lcd.clear();

}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print(" PLACE THE RFID ");
  lcd.setCursor(0, 1);
  lcd.print("  CARD TO SCAN  ");
  buttonState = digitalRead(BTN_PIN);

  if (buttonState == LOW) {
    registrationMode = !registrationMode;

    if (registrationMode) {
      Serial.println("Registration Mode");
      lcd.setCursor(0, 0);
      lcd.print("   WELCOME TO   ");
      lcd.setCursor(0, 1);
      lcd.print(" REGISTER MODE  ");
      delay(5000);

    } else {
      Serial.println("Access Mode");
      lcd.setCursor(0, 0);
      lcd.print("   WELCOME TO   ");
      lcd.setCursor(0, 1);
      lcd.print("  ACCESS MODE   ");
      delay(5000);
    }
    delay(500); // Debounce
  }

  if (registrationMode) {
    registerCard();
  } else {
    accessControl();
  }
}

void registerCard() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Place the card for registration near the reader.");
    Serial.print("UID: ");
    printCardUID();
    lcd.setCursor(0, 0);
    lcd.print("CARD REGISTERED ");
    lcd.setCursor(0, 1);
    lcd.print("  SUCCESSFULLY  ");
    delay(3000);
    registeredCardUID = getCardUID();
    delay(1000); // Delay to avoid multiple registrations
  }
}

void accessControl() {

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Place the card for access near the reader.");
    Serial.print("UID: ");
    printCardUID();

    if (isRegisteredCard()) {
      Serial.println("Access granted!");
      lcd.setCursor(0, 0);
      lcd.print("CHECKING ACCESS ");
      lcd.setCursor(0, 1);
      lcd.print(" PLEASE WAIT... ");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" ACCESS GRANTED ");
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(2500);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      
    } else {
      lcd.setCursor(0, 0);
      lcd.print("CHECKING ACCESS ");
      lcd.setCursor(0, 1);
      lcd.print(" PLEASE WAIT... ");
      Serial.println("Access denied!");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" ACCESS DENIED ");
      delay(2500);
      //lcd.clear();
    }

    delay(1000); // Delay to avoid multiple reads
  }
}

bool isRegisteredCard() {
  String currentCardUID = getCardUID();
  return currentCardUID == registeredCardUID;
}

void printCardUID() {
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
}

String getCardUID() {
  String cardUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    cardUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  return cardUID;
}
