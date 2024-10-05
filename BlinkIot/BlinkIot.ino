#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoBearSSL.h>

// Replace with your network credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// API endpoint details
const char* serverAddress = "YOUR_SERVER_ADDRESS"; // Hostname
const int serverPort = 443; // HTTPS port

// URL path
const char* url = "YOUR_URL";

// Define a WiFiSSLClient for HTTPS
WiFiSSLClient client;

// Initialize the HTTP client
HttpClient http(client, serverAddress, serverPort);

// LED pin (Built-in LED)
const int ledPin = LED_BUILTIN;

// Timing variables
unsigned long previousMillis = 0;        // Stores the last time a GET request was made
const long interval = 5000;               // Interval at which to make GET requests (milliseconds)
const long ledBlinkDuration = 500;        // LED on/off duration (milliseconds)

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB
  }

  Serial.println("Starting...");

  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Ensure LED is off initially

  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found!");
    while (true); // Don't proceed, loop forever
  }

  // Attempt to connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  int status = WiFi.begin(ssid, password);

  // Wait for connection
  while (status != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
    status = WiFi.status();
  }

  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if it's time to make a GET request
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a GET request was made
    previousMillis = currentMillis;

    // Blink the LED to indicate a GET request is being made
    digitalWrite(ledPin, HIGH);   // Turn the LED on
    delay(ledBlinkDuration);      // Wait for the blink duration
    digitalWrite(ledPin, LOW);    // Turn the LED off
    delay(ledBlinkDuration);      // Wait for the blink duration

    // Make the GET request
    Serial.println("Making GET request...");

    // Send the GET request
    int statusCode = http.get(url);

    // Read the status code
    Serial.print("Status code: ");
    Serial.println(statusCode);

    // Read the response
    String response = http.responseBody();

    Serial.println("Response:");
    Serial.println(response);

    // Close the connection
    http.stop();
  }

  // You can add other non-blocking code here if needed
}
