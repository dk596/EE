/*----------------------------------
Title: Battery_Voltage_Data_Logger
------------------------------------
Program Detail:
------------------------------------
Purpose: Periodically measures battery cell voltage on analog pin A0, applies a 16-sample noise-filtering average, converts the ADC count to true cell voltage using a calibrated resistor multiplier, and streams the data over UART in CSV format for real-time logging in Excel Data Streamer.
Inputs: Analog voltage signal on ADC pin A0 (interfaced through a 159k series resistor circuit).
Outputs: Serial UART data stream (115200 baud) outputting comma-separated values: Elapsed_Minutes, Filtered_ADC, Calculated_Voltage.
Date: October 5, 2026
Compiler: PlatformIO / AVR-GCC (Atmel AVR GNU toolchain)
Author: Derek Kan
Versions:

V1 - Initial implementation reading raw ADC values on pin A0 and printing single voltage readings over Serial.
V2 - Implemented non-blocking millis() timing for 60-second intervals, added a 16-sample noise filter, and structured output as CSV for Excel Data Streamer compatibility.
------------------------------------
File Dependencies:
------------------------------------
Arduino core library (Arduino.h)
*/

#include <Arduino.h>
//----------------------------------
//Main Program
//----------------------------------

const int ADC_PIN = A0;
const unsigned long INTERVAL_MS = 60000;
unsigned long previousMillis = 0;

const float VOLTAGE_MULTIPLIER = 3.906 / 881.5;

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= INTERVAL_MS || previousMillis == 0) {
    previousMillis = currentMillis;

    long adcSum = 0;
    for (int i = 0; i < 16; i++) {
      adcSum += analogRead(ADC_PIN);
      delay(5);
    }
    float rawAdc = adcSum / 16.0;
    float voltage = rawAdc * VOLTAGE_MULTIPLIER;

    Serial.print(currentMillis / 60000.0, 2);
    Serial.print(",");
    Serial.print(rawAdc, 1);
    Serial.print(",");
    Serial.println(voltage, 3);
  }
}
