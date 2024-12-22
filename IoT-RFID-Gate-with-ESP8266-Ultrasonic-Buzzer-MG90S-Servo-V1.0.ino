#define BLYNK_TEMPLATE_ID "TEMPLATE_ID" 
#define BLYNK_TEMPLATE_NAME "TEMPLATE-NAME" 
#define BLYNK_AUTH_TOKEN "YOUR-AUTH-HERE" 

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <BlynkSimpleEsp8266.h>

// Wi-Fi Credentials
char ssid[] = "put_your_ssid_here";        
char pass[] = "_put_your_pass_here";  

// RFID pin setup
#define RST_PIN D1
#define SS_PIN D2
MFRC522 rfid(SS_PIN, RST_PIN);

// Ultrasonic pin setup
#define TRIG_PIN D3
#define ECHO_PIN D4

// Buzzer pin
#define BUZZER_PIN D8

// Servo setup
Servo gateServo;
#define SERVO_PIN D0

// Variables
const String validCards[] = {"3DXXXXX", "4DXXXXXX"}; // Array of valid card UIDs
const int numValidCards = sizeof(validCards) / sizeof(validCards[0]);
int cardCounters[numValidCards] = {0}; // Array to store count for each card
unsigned long objectCheckStart = 0; // Start time for ultrasonic detection
bool isServoOpen = false; // To track if the servo is open
unsigned long lastBeepTime = 0; // Timer for buzzer
int beepCount = 0; // Counter for invalid card buzzer pattern

void setup() {
  Serial.begin(9600);

  // Initialize Wi-Fi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // RFID initialization
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID reader initialized.");

  // Ultrasonic initialization
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("Ultrasonic sensor initialized.");

  // Buzzer initialization
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Servo initialization
  gateServo.attach(SERVO_PIN);
  gateServo.write(0); // Set servo to closed position
  Serial.println("Servo initialized.");

  Serial.println("Waiting for input...");
}

void loop() {
  Blynk.run();  // Run Blynk

  // RFID Handling
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String cardID = getCardID();
    cardID.toUpperCase(); // Ensure consistent format
    Serial.print("Card detected: ");
    Serial.println(cardID);

    int cardIndex = getCardIndex(cardID);
    if (cardIndex != -1) { // Valid card
      cardCounters[cardIndex]++; // Update count for this card
      String status = (cardCounters[cardIndex] % 2 == 0) ? "Exit" : "Entry";

      // Actions for valid card
      Serial.print("Valid card ");
      Serial.print(cardID);
      Serial.print(" ");
      Serial.println(status);

      // Send data to Blynk
      Blynk.virtualWrite(V1, cardID); // Send card ID to virtual pin V1
      Blynk.virtualWrite(V2, status); // Send status to virtual pin V2
      Blynk.virtualWrite(V3, "Card ID " + cardID + " " + status); // Log activity to virtual pin V3

      // Log event for valid card
      Blynk.logEvent("Card_Entry_Exit", "ID: " + cardID + " Status: " + status);

      // Buzzer long beep
      digitalWrite(BUZZER_PIN, HIGH);
      delay(1000); // 1-second beep
      digitalWrite(BUZZER_PIN, LOW);

      // Servo opens for valid card
      gateServo.write(180);
      isServoOpen = true;
      delay(3000); // Hold for 3 seconds

      // Start ultrasonic checking
      objectCheckStart = millis();
    } else {
      // Actions for invalid card
      Serial.print("Card ");
      Serial.print(cardID);
      Serial.println(" Invalid!!!");

      // Log event for invalid card
      Blynk.logEvent("Card_Invalid", "ID: " + cardID);

      // Buzzer pattern for invalid card
      beepCount = 0;
      lastBeepTime = millis();
      invalidCardBuzzer();
    }

    rfid.PICC_HaltA(); // Stop RFID communication
  }

  // Ultrasonic Handling
  if (isServoOpen && (millis() - objectCheckStart > 2000)) {
    if (!isObjectDetected()) {
      Serial.println("No object detected. Servo returning to closed position.");
      gateServo.write(0); // Close the servo
      isServoOpen = false;
    } else {
      Serial.println("Object still detected. Servo remains open.");
      objectCheckStart = millis(); // Reset the timer
    }
  }

  // Handle invalid card buzzer pattern
  invalidCardBuzzer();
}

// Function to get card ID as a string
String getCardID() {
  String cardID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      cardID += "0"; // Add leading zero for consistent length
    }
    cardID += String(rfid.uid.uidByte[i], HEX);
  }
  return cardID;
}

// Function to check for objects with ultrasonic
bool isObjectDetected() {
  long duration, distance;

  // Trigger the sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the Echo
  duration = pulseIn(ECHO_PIN, HIGH, 20000); // Timeout after 20ms
  if (duration == 0) {
    return false; // No echo detected
  }

  // Calculate distance in cm
  distance = duration * 0.034 / 2;

  // Return true if distance < 10 cm
  return (distance > 0 && distance <= 10);
}

// Function to handle buzzer for invalid card
void invalidCardBuzzer() {
  if (beepCount < 10 && millis() - lastBeepTime > 200) { // 5 beeps (on/off)
    digitalWrite(BUZZER_PIN, beepCount % 2 == 0 ? HIGH : LOW);
    lastBeepTime = millis();
    beepCount++;
  } else if (beepCount >= 10) {
    digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off
  }
}

// Function to get the index of the card in the validCards array
int getCardIndex(String cardID) {
  for (int i = 0; i < numValidCards; i++) {
    if (cardID == validCards[i] || (i == 2 && cardID.startsWith("08"))) {
      return i;
    }
  }
  return -1; // Card not found
}