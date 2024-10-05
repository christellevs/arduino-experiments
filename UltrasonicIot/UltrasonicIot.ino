#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

#include "secrets.h"
// vars in secrets: SSID, PASSWORD, SERVER_ADDRESS, API_PATH

// API endpoint details
const int serverPort = 443; // HTTPS port

// HC-SR04 sensor configuration
const int trigPin = 9;  // Digital pin 9 for Trig
const int echoPin = 10; // Digital pin 10 for Echo

// Interval between posts (milliseconds)
const unsigned long POST_INTERVAL = 5000;

// Variables to manage timing
unsigned long previousMillis = 0;

// WiFi client and HTTP client for main API
WiFiSSLClient wifiClient;
HttpClient client = HttpClient(wifiClient, SERVER_ADDRESS, serverPort);

// LED Pin
const int ledPin = LED_BUILTIN;

// ---------------------------

// Function Prototypes

// ---------------------------
bool connectToWiFi();
int readDistance();

// ---------------------------

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  // Initialize built-in LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Initialize HC-SR04 sensor pins
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input

  Serial.println("Starting distance reading...");

  // Connect to WiFi
  if (connectToWiFi()) {
    Serial.println("WiFi connected");
  } else {
    Serial.println("WiFi connection failed");
    // You can choose to retry or handle the failure accordingly
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= POST_INTERVAL) {
    previousMillis = currentMillis;

    // Read distance from HC-SR04 sensor
    int distance = readDistance();

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Create JSON payload
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["distance"] = distance;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    // Debug: Print JSON
    Serial.println(jsonString);

    // Prepare HTTP POST request
    client.beginRequest();
    client.post(API_PATH);
    Serial.println(API_PATH);
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", jsonString.length());
    client.endRequest();
    client.print(jsonString);

    // Read the response
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    // Optional: Handle different status codes
    if (statusCode == 200 || statusCode == 201) {
      Serial.println("Data successfully sent!");
    } else {
      Serial.println("Failed to send data.");
    }

    // Blink the built-in LED to indicate a successful POST
    digitalWrite(ledPin, HIGH); // LED on
    delay(100);                  // Short delay
    digitalWrite(ledPin, LOW);   // LED off
  }
}

// ---------------------------

// Function Definitions

// ---------------------------

// Function to connect to WiFi
bool connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);

  // Attempt to connect to WiFi network
  int status = WiFi.begin(SSID, PASSWORD);

  // Wait for connection with a timeout
  unsigned long startAttemptTime = millis();

  // Keep trying to connect until timeout (10 seconds)
  while (status != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
    status = WiFi.status();
  }

  if (status == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nFailed to connect to WiFi.");
    return false;
  }
}

// Function to read distance from HC-SR04 sensor
int readDistance() {
  // Ensure trigger pin is LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Trigger the sensor by setting the trigPin high for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout after 30ms (approx 5m)

  if (duration == 0) {
    // Timeout occurred, no echo received
    return -1;
  }

  // Calculate the distance (in centimeters)
  int distance = duration * 0.034 / 2;

  return distance;
}
