/*----------------------------------
Title: ESP8266_LED_Blink_Status
------------------------------------
Program Detail:
------------------------------------
Purpose: Blinks the onboard LED of the ESP8266 at a non-blocking 1-second interval using millis() and prints the real-time operational status (ON/OFF) of the LED over the Serial monitor.
Inputs: System clock / internal millis() timer ticks.
Outputs: Active-low digital signal on onboard LED GPIO pin (GPIO2 / D4), and Serial UART telemetry stream (115200 baud) displaying LED state transitions.
Date: October 5, 2026
Compiler: PlatformIO / Xtensa LX106 GCC (esp8266 toolchain)
Author: Derek Kan
Versions:

V1 - Initial implementation
------------------------------------
File Dependencies:
------------------------------------
Arduino core for ESP8266 (Arduino.h)
*/

#include <Arduino.h>

// On most ESP8266 boards (NodeMCU, Wemos D1 Mini, ESP-12E/F), 
// the blue onboard LED is connected to GPIO2 (D4) and is active-LOW.
const int LED_PIN = 2; 

const unsigned long BLINK_INTERVAL_MS = 1000; // Toggle state every 1000 ms (1 second)
unsigned long previousMillis = 0;
bool ledState = false; // Tracks logical LED state (true = ON, false = OFF)

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initialize GPIO pin
  pinMode(LED_PIN, OUTPUT);

  // Turn off initially (HIGH is OFF for active-low ESP8266 onboard LED)
  digitalWrite(LED_PIN, HIGH);

  Serial.println("--- ESP8266 LED Blink Initialized ---");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= BLINK_INTERVAL_MS) {
    previousMillis = currentMillis;

    // Toggle logical state
    ledState = !ledState;

    // Active-low logic: LOW turns the LED ON, HIGH turns it OFF
    digitalWrite(LED_PIN, ledState ? LOW : HIGH);

    // Print status over Serial
    Serial.print("Timestamp (ms): ");
    Serial.print(currentMillis);
    Serial.print(" | LED Status: ");
    Serial.println(ledState ? "ON" : "OFF");
  }
}
