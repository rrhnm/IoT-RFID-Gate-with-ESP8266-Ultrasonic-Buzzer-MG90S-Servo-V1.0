# IoT-RFID-Gate-with-ESP8266-Ultrasonic-Buzzer-MG90S-Servo-V1.0
Implement IoT with ESP8266, RFID sensor, MG90S Servo, Buzzer, NFC Card, and Ultrasonic

# Smart Gate with IoT and RFID

This project implements a smart gate system using NodeMCU ESP8266, RFID, a servo motor, a buzzer, and an ultrasonic sensor. It leverages Blynk for IoT control and monitoring. The system allows authorized access via RFID cards and provides logging for card usage, as well as alerts for invalid card attempts.

## Features
- **RFID Authentication:** Only authorized cards can open the gate.
- **Ultrasonic Sensor:** Detects objects to ensure safe gate operation.
- **Buzzer Alerts:** Signals invalid card attempts with a beep pattern.
- **Servo Motor Control:** Opens and closes the gate based on card validity.
- **IoT Integration with Blynk:** Monitors gate activity and logs events in real-time.

## Hardware Requirements
- NodeMCU ESP8266
- MFRC522 RFID Reader
- Servo Motor
- Ultrasonic Sensor (HC-SR04)
- Buzzer
- Breadboard and Jumper Wires

## Software Requirements
- Arduino IDE
- Blynk Library
- MFRC522 Library

## Circuit Diagram
- **RFID Pins:**
  - RST_PIN -> D1
  - SS_PIN -> D2
- **Ultrasonic Sensor Pins:**
  - TRIG_PIN -> D3
  - ECHO_PIN -> D4
- **Servo Motor Pin:**
  - SERVO_PIN -> D0
- **Buzzer Pin:**
  - BUZZER_PIN -> D8

## Configuration
1. Clone this repository:
   ```bash
   git clone <repository-url>
   ```
2. Open the project in Arduino IDE.
3. Update the following placeholders in the code:
   - Wi-Fi credentials:
     ```cpp
     char ssid[] = "put_your_ssid_here"; 
     char pass[] = "put_your_pass_here";
     ```
   - Blynk Auth Token:
     ```cpp
     #define BLYNK_AUTH_TOKEN "YOUR-AUTH-HERE"
     ```
   - Add your valid card UIDs to the `validCards` array:
     ```cpp
     const String validCards[] = {"3DXXXXX", "4DXXXXXX"};
     ```
4. Connect your hardware as per the circuit diagram.
5. Upload the code to your NodeMCU.

## Usage
1. Power up the system.
2. Scan an RFID card:
   - If valid, the gate opens for 3 seconds and logs the event in Blynk.
   - If invalid, the buzzer alerts and logs the attempt in Blynk.
3. Monitor gate activity in the Blynk app.

## Contributing
Feel free to fork this repository and make your contributions. Pull requests are welcome.
