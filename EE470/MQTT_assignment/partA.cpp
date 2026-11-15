#include <Arduino.h>

// Hardware Pin Definitions
const int POT_PIN = A0;   // Analog input
const int SWITCH_PIN = 4; // D2 on NodeMCU
const int LED_PIN = 5;    // D1 on NodeMCU

// Variables for timing so we don't flood the serial monitor
unsigned long lastUpdate = 0;
const int updateInterval = 100; // milliseconds

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Wait a moment for serial to connect
  delay(1000); 
  Serial.println("\nESP8266 Hardware Monitor Started");
  Serial.println("Type '1' to turn LED ON, '0' to turn LED OFF");
  Serial.println("----------------------------------------");

  // Configure Pins
  pinMode(SWITCH_PIN, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);        // Ensure LED is off at start
}

void loop() {
  // 1. Check for incoming Serial data (User typing 1 or 0)
  if (Serial.available() > 0) {
    char incomingChar = Serial.read();
    
    // Ignore newline/carriage return characters
    if (incomingChar == '1') {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("\n>>> LED Turned ON (Received 1)");
    } else if (incomingChar == '0') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("\n>>> LED Turned OFF (Received 0)");
    }
  }

  // 2. Non-blocking delay to read hardware states
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();

    // Read Potentiometer
    // ESP8266 ADC is 10-bit (0-1023)
    int adcValue = analogRead(POT_PIN);
    
    // Convert ADC to voltage (Assuming 3.3V range)
    float voltage = adcValue * (3.3 / 1023.0);

    // Read Switch
    // PULLUP means unpressed = HIGH (1), pressed = LOW (0). 
    // We invert it with ! so pressed = 1 for the terminal.
    int switchState = !digitalRead(SWITCH_PIN);

    // 3. Print the formatted output
    // Carriage return '\r' overwrites the same line in compatible terminals
    Serial.print("Potentiometer: ");
    Serial.print(voltage);
    Serial.print(" V (Raw: ");
    Serial.print(adcValue);
    Serial.print(") | Switch Pressed: ");
    Serial.print(switchState);
    Serial.print("      \r"); // Spaces pad the end to clear old characters
  }
}
