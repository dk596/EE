#include <Arduino.h>

const int ADC_PIN = A0;
const unsigned long INTERVAL_MS = 60000; // Log once every 60 seconds (1 minute)
unsigned long previousMillis = 0;

// Multiplier for 159k series resistor
const float VOLTAGE_MULTIPLIER = 3.906 / 881.5;

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();

  // Send a reading immediately at boot, then every INTERVAL_MS
  if (currentMillis - previousMillis >= INTERVAL_MS || previousMillis == 0) {
    previousMillis = currentMillis;

    // Average 16 samples to filter ADC noise
    long adcSum = 0;
    for (int i = 0; i < 16; i++) {
      adcSum += analogRead(ADC_PIN);
      delay(5);
    }
    float rawAdc = adcSum / 16.0;
    float voltage = rawAdc * VOLTAGE_MULTIPLIER;

    // Excel Data Streamer Format: Channel1,Channel2,Channel3\n
    Serial.print(currentMillis / 60000.0, 2); // Minutes elapsed
    Serial.print(",");
    Serial.print(rawAdc, 1);                  // Filtered ADC count
    Serial.print(",");
    Serial.println(voltage, 3);               // Calculated Voltage
  }
}
