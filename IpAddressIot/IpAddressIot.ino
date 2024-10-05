#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

// === Wi-Fi Configuration ===
const char* WIFI_SSID     = "";       // Replace with your Wi-Fi SSID
const char* WIFI_PASSWORD = "";   // Replace with your Wi-Fi Password

// === API Configuration ===
const char* API_HOST   = "; // API Hostname
const int   API_PORT   = 443;                                    // HTTPS Port
const char* API_PATH   = "";                   // API Endpoint Path

// === Network Clients ===
WiFiSSLClient sslClient; // SSL client for HTTPS
HttpClient   httpClient(sslClient, API_HOST, API_PORT);

// === Timing Configuration ===
const unsigned long SEND_INTERVAL_MS = 5000; // 5000 milliseconds = 5 seconds
unsigned long previousSendTime = 0;           // Tracks the last send time

// === Function Prototypes ===
void connectToWiFi();
void sendIPAddress();

// === Setup Function ===
void setup() {
  Serial.begin(9600);
  while (!Serial) { ; } // Wait for Serial to initialize

  Serial.println("=== Arduino IoT 33 Nano IP Sender ===");

  // Connect to Wi-Fi
  connectToWiFi();
}

// === Main Loop ===
void loop() {
  unsigned long currentMillis = millis();

  // Check if it's time to send the IP address
  if (currentMillis - previousSendTime >= SEND_INTERVAL_MS) {
    previousSendTime = currentMillis; // Update the last send time
    sendIPAddress();
  }

  // Optional: Add a small delay to prevent overwhelming the CPU
  delay(100);
}

// === Function Definitions ===

/**
 * Connects the Arduino to the specified Wi-Fi network.
 */
void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi SSID: %s\n", WIFI_SSID);

  // Begin Wi-Fi connection
  int connectionStatus = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait until connected
  while (connectionStatus != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
    connectionStatus = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  // Connection successful
  Serial.println("\nWi-Fi connected successfully!");
  Serial.print("Device IP Address: ");
  Serial.println(WiFi.localIP());
}

/**
 * Sends the device's IP address to the specified API endpoint via HTTPS POST.
 */
void sendIPAddress() {
  // Check Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi not connected. Attempting to reconnect...");
    connectToWiFi();
    return;
  }

  // Retrieve IP Address as String
  String deviceIPAddress = WiFi.localIP().toString();

  // Prepare JSON payload with clear variable names
  String jsonPayload = "{ \"ipAddress\": \"" + deviceIPAddress + "\" }";

  Serial.print("Preparing to send POST request to %s\n", API_PATH);
  Serial.print("JSON Payload: %s\n", jsonPayload.c_str());

  // Begin HTTP POST Request
  httpClient.beginRequest();
  httpClient.post(API_PATH);

  // Set HTTP Headers
  httpClient.sendHeader("Content-Type", "application/json");
  httpClient.sendHeader("Content-Length", jsonPayload.length());

  // Send JSON Payload in Body
  httpClient.beginBody();
  httpClient.print(jsonPayload);

  // End Request and Send
  httpClient.endRequest();

  // Read and Display Response Status
  int httpStatusCode = httpClient.responseStatusCode();
  String httpResponse = httpClient.responseBody();

  Serial.print("Response Status Code: %d\n", httpStatusCode);
  Serial.print("Response Body: %s\n", httpResponse.c_str());

  // Optional: Add a delay between requests if necessary
}
