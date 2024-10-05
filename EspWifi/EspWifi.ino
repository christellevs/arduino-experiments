#include <ESP8266WiFi.h>  // Include the Wi-Fi library for ESP8266

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

void setup() {
  Serial.begin(115200);             // Initialize serial communication at 115200 baud rate
  delay(10);
  
  Serial.println();
  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);       // Connect to Wi-Fi network

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");              // Print dots while connecting
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());   // Display the IP address
}

void loop() {
  // Main code goes here (if needed)
}
