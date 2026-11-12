/*
 * ----------------------------------------------
 * Project/Program Name : ESP8266 Sensor Node & Retransmission System
 * File Name            : main.cpp
 * Author               : Derek Kan
 * Date                 : 11/11/2026
 * Version              : 1.0
 * 
 * Purpose:
 *    Reads an analog Hall Effect sensor and digital PIR motion sensor, fetches a 
 *    precise timestamp via HTTPS from timeapi.io, and securely transmits the payload 
 *    to a remote MySQL database. Includes a struct-based buffer to hold data during 
 *    connection loss and retry until delivery is confirmed by the server.
 * 
 * Inputs:
 *    - Hall Effect Sensor (Analog Pin A0)
 *    - PIR Motion Sensor (Digital Pin)
 *    - Push Button / Tilt Switch (Digital Pins, INPUT_PULLUP)
 *    - Serial Monitor (Timezone selection input)
 * 
 * Outputs:
 *    - HTTPS GET requests containing telemetry data (URL Query Strings)
 *    - Serial Monitor (Debugging, status updates, and HTTP responses)
 * 
 * Example Application:
 *    Deployed as an IoT monitoring node that detects magnetic field variations 
 *    and physical presence, ensuring no data is lost during intermittent Wi-Fi 
 *    dropouts by queuing the reading until the server replies with "successfully".
 * 
 * Dependencies:
 *    - ESP8266WiFi.h
 *    - WiFiClientSecure.h
 *    - ESP8266HTTPClient.h
 *    - ArduinoJson.h
 * 
 * Usage Notes:
 *    Requires Wi-Fi credentials to be defined separately. Uses client.setInsecure() 
 *    to bypass SSL certificate validation, preventing memory exhaustion on the ESP8266.
 * 
 * ---------------------------------------------------------------------------
 */


#include <Arduino.h>
#include <ESP8266WiFi.h>
//Used for formatting the HTTPS request
#include <ESP8266HTTPClient.h>
//the pipeline that allows the ESP8266 to talk to the server over HTTPS
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "secrets.h"

// ==========================================
//           SYSTEM CONFIGURATION
// ==========================================

const String hostingerDomain = "https://mediumvioletred-sandpiper-465466.hostingersite.com";
String selectedTimeZone = "America/Los_Angeles"; 

// Hardware Pins
const int buttonPin = 4;        // D2
const int tiltSwitchPin = 5;    // D1
const int pirPin = 14;          // D5
const int hallAnalogPin = A0;   // A0

// Hall Sensor Settings
const float ESP_VCC = 3.3; 
const float SENSITIVITY = 0.0014; 
float nullVoltage = 1.65; 

// Switch State Tracking (for edge detection)
int lastButtonState = HIGH; 
int lastTiltState = HIGH; 

// ==========================================
//          RETRANSMISSION MEMORY
// ==========================================

struct SavedMessage {
  bool isWaiting;         // True if a message needs to be sent/retried
  String targetNode;      // Which node triggered the event
  int presenceValue;      // 0 or 1
  float gaussValue;       // Calculated magnetic field
  String lockedTimestamp; // Time of the physical trigger
  unsigned long lastAttemptTime; // Milliseconds since we last tried to send it
} retransmissionBuffer;

// ==========================================
//             SETUP FUNCTIONS
// ==========================================

// ---------------------------------------------------------
// Configures the ESP8266's physical pins and starts the 
// Serial Monitor so we can print messages to the screen.
// ---------------------------------------------------------
void initialize_hardware() {
  Serial.begin(115200);
  delay(100);
  
  // INPUT_PULLUP enables internal resistors to prevent the pins 
  // from floating and giving false readings when switches are open.
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(tiltSwitchPin, INPUT_PULLUP); 
  pinMode(pirPin, INPUT);
}

// ---------------------------------------------------------
// Connects the microcontroller to your local router using 
// the hidden credentials in secrets.h, and prints the 
// signal strength (RSSI) once connected.
// ---------------------------------------------------------
void connect_to_wifi() {
  Serial.print("\nConnecting to Wi-Fi");
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n--- Network Info ---");

// Get and print the IP Address
Serial.print("IP Address: ");
Serial.println(WiFi.localIP());

// Get and print the MAC Address
Serial.print("MAC Address: ");
Serial.println(WiFi.macAddress());

Serial.println("--------------------");

  Serial.println("\nWi-Fi Connected!");
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

// ---------------------------------------------------------
// Analog sensors drift based on temperature and power supply. 
// This function takes 50 rapid readings of the ambient room, 
// averages them, and sets that exact voltage as "0 Gauss" 
// so future magnet readings are perfectly accurate.
// ---------------------------------------------------------
void calibrate_hall_sensor() {
  Serial.println("Calibrating Hall Sensor to find baseline 0 Gauss...");
  long sum = 0;
  for (int i = 0; i < 50; i++) {
    sum += analogRead(hallAnalogPin);
    delay(10);
  }
  
  // Convert the raw analog average (0-1023) into standard Volts
  nullVoltage = (sum / 50.0) * (ESP_VCC / 1023.0);
  
  Serial.print("Baseline Voltage set to: ");
  Serial.println(nullVoltage, 3);
}

// ---------------------------------------------------------
// Creates an interactive menu in the Serial Monitor. It uses 
// a non-blocking timer (millis) to wait exactly 10 seconds 
// for the user to type a number. If they do nothing, it 
// safely defaults to Pacific Time.
// ---------------------------------------------------------
void display_timezone_menu() {
  Serial.println("\n==========================================");
  Serial.println("Select Timezone (Enter number 1-7 in Serial Monitor)");
  Serial.println("You have 10 seconds. No input defaults to PT.");
  Serial.println("1. Eastern Time Zone (ET)");
  Serial.println("2. Central Time Zone (CT)");
  Serial.println("3. Mountain Time Zone (MT)");
  Serial.println("4. Pacific Time Zone (PT)");
  Serial.println("5. Alaska Time Zone (AKT)");
  Serial.println("6. Hawaii-Aleutian Time (HAT)");
  Serial.println("7. Atlantic Time Zone (AT)");
  Serial.println("==========================================");

  long startTime = millis();
  bool inputReceived = false;

  while (millis() - startTime < 10000) { 
    if (Serial.available() > 0) {
      int choice = Serial.parseInt();
      switch (choice) {
        case 1: selectedTimeZone = "America/New_York"; break;
        case 2: selectedTimeZone = "America/Chicago"; break;
        case 3: selectedTimeZone = "America/Denver"; break;
        case 4: selectedTimeZone = "America/Los_Angeles"; break;
        case 5: selectedTimeZone = "America/Anchorage"; break;
        case 6: selectedTimeZone = "Pacific/Honolulu"; break;
        case 7: selectedTimeZone = "America/Puerto_Rico"; break;
        default: selectedTimeZone = "America/Los_Angeles"; break; 
      }
      
      // Clear any leftover characters (like the 'Enter' key) from the buffer
      while(Serial.available()) { Serial.read(); } 
      inputReceived = true;
      break; 
    }
    delay(10); 
  }

  if (!inputReceived) {
    Serial.println("\nTimeout reached. Defaulting to PT.");
  }
  
  Serial.print("Active Timezone: ");
  Serial.println(selectedTimeZone);
  Serial.println("\n--- System Ready. Press button or tilt switch to send data ---");
}

// ==========================================
//          DATA LOGIC FUNCTIONS
// ==========================================

String fetch_time_from_api() {
  // 1. SECURE CONNECTION SETUP
  // Create a secure client to handle the HTTPS (encrypted) connection
  WiFiClientSecure client;
  
  // Tell the ESP8266 to skip SSL certificate verification (saves memory/processing power)
  client.setInsecure();
  
  // Create the HTTP object to manage the actual web request
  HTTPClient http;

  // 2. MAKING THE REQUEST
  // Build the target URL, appending the user's chosen timezone from the menu
  String url = "https://timeapi.io/api/Time/current/zone?timeZone=" + selectedTimeZone;
  
  // Open the secure pipeline and aim it at the URL ("dial the number")
  http.begin(client, url);
  
  // Fire the HTTP GET request across the internet and capture the server's status code
  int httpCode = http.GET();
  String formattedTimestamp = "";

  // 3. READING AND TRANSLATING
  // If the server replies with HTTP 200 OK (Success)
  if (httpCode == HTTP_CODE_OK) {
    // Grab the raw JSON text response from the server
    String payload = http.getString();
    
    // Reserve 1024 bytes of memory to hold the parsed JSON data
    DynamicJsonDocument doc(1024);
    
    // Translate the raw text into a searchable JSON folder structure
    deserializeJson(doc, payload);
    
    // 4. SLICING THE STRING
    // Extract the exact field we need (e.g., "2026-11-11T14:30:00.0000000")
    String rawDateTime = doc["dateTime"].as<String>();
    
    // Format for MySQL: Grab the Date (first 10 chars) + URL space (%20) + Time (next 8 chars)
    // This perfectly cuts out the 'T' and the trailing zeroes
    formattedTimestamp = rawDateTime.substring(0, 10) + "%20" + rawDateTime.substring(11, 19);
    
  } else {
    Serial.println("Error fetching time from timeapi.io.");
  }
  
  // 5. CLEANUP
  // Close the connection to free up the Wi-Fi chip's memory for the next task
  http.end();
  
  // Return the perfectly formatted string (e.g., "2026-11-11%2014:30:00") back to the main loop
  return formattedTimestamp;
}


// ---------------------------------------------------------
// Constructs the URL from the saved buffer, sends it to the 
// database, and reads the PHP response. If the server confirms
// receipt or a duplicate, it clears the buffer to stop retrying.
// ---------------------------------------------------------
void attempt_server_delivery() {
  retransmissionBuffer.lastAttemptTime = millis(); 

  String requestUrl = hostingerDomain + "/insert2.php?node_name=" + retransmissionBuffer.targetNode + 
                      "&presence_detected=" + String(retransmissionBuffer.presenceValue) + 
                      "&gauss=" + String(retransmissionBuffer.gaussValue, 1) + 
                      "&time_received=" + retransmissionBuffer.lockedTimestamp;

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    Serial.print("Current RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    Serial.print("Attempting to send to " + retransmissionBuffer.targetNode + "... ");
    
    http.begin(client, requestUrl);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String serverText = http.getString();
      Serial.print("HTTP ");
      Serial.print(httpResponseCode);
      Serial.print(" | Server says: ");
      Serial.println(serverText);

      if (serverText.indexOf("successfully") >= 0 || serverText.indexOf("Duplicate") >= 0) {
        Serial.println("Delivery confirmed! Clearing message from queue.");
        retransmissionBuffer.isWaiting = false; 
      } else {
        Serial.println("Server rejected data. Will retry in 30 seconds.");
      }
    } else {
      Serial.print("Connection failed! HTTP Code: ");
      Serial.println(httpResponseCode);
      Serial.println("Will retry in 30 seconds.");
    }
    http.end();
  } else {
    Serial.println("Wi-Fi disconnected! Will retry in 30 seconds.");
  }
}



// ---------------------------------------------------------
// Takes a snapshot of the exact time and current sensor 
// readings, locks them safely into the retransmission buffer, 
// and triggers the very first attempt to send them to the database.
// ---------------------------------------------------------
void package_data_and_send(String targetNodeName) {
  String currentTimestamp = fetch_time_from_api();
  
  if (currentTimestamp == "") {
    Serial.println("Aborting: Could not get timestamp.");
    return;
  }

  int currentPresence = digitalRead(pirPin);
  float rawVoltage = analogRead(hallAnalogPin) * (ESP_VCC / 1023.0);
  float currentGauss = (rawVoltage - nullVoltage) / SENSITIVITY;

  retransmissionBuffer.targetNode = targetNodeName;
  retransmissionBuffer.presenceValue = currentPresence;
  retransmissionBuffer.gaussValue = currentGauss;
  retransmissionBuffer.lockedTimestamp = currentTimestamp;
  retransmissionBuffer.isWaiting = true; 

  Serial.println("\nData captured! Locked timestamp: " + currentTimestamp);
  
  attempt_server_delivery(); 
}

// ==========================================
//               LOOP FUNCTIONS
// ==========================================

void check_button_switch() {
  int buttonState = digitalRead(buttonPin);
  
  // If button is LOW (pressed) and it was previously HIGH
  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.println("\nButton press detected! Routing to Node 1.");
    package_data_and_send("ESP8266_Node1");
    delay(50); // Debounce
  }
  lastButtonState = buttonState; // Update history
}

void check_tilt_switch() {
  int tiltState = digitalRead(tiltSwitchPin);
  
  // If tilt switch is LOW (triggered) and it was previously HIGH
  if (tiltState == LOW && lastTiltState == HIGH) {
    Serial.println("\nTilt switch triggered! Routing to Node 2.");
    package_data_and_send("ESP8266_Node2");
    delay(50); // Debounce
  }
  lastTiltState = tiltState; // Update history
}

void check_retransmission() {
  // If a message is waiting AND 30,000 milliseconds (30 seconds) have passed
  if (retransmissionBuffer.isWaiting == true) {
    if (millis() - retransmissionBuffer.lastAttemptTime >= 30000) {
      Serial.println("\n--- Retransmission Timeout (30s) Reached ---");
      attempt_server_delivery();
    }
  }
}

// ==========================================
//               MAIN WORKFLOW
// ==========================================

void setup() {
  initialize_hardware();
  connect_to_wifi();
  calibrate_hall_sensor();
  display_timezone_menu();
}

void loop() {
  check_button_switch();
  check_tilt_switch();
  check_retransmission();
}
