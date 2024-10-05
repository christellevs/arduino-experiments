#include <ESP8266WiFi.h>          // Wi-Fi library
#include <ESP8266HTTPClient.h>    // HTTP client library
#include <DHT.h>                  // DHT library
#include <Adafruit_Sensor.h>      // Adafruit sensor library
#include <WiFiClientSecure.h>     // Secure Wi-Fi client for HTTPS
#include <time.h>                 // Time functions for NTP

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// NTP Server Details
const char* ntpServer = "pool.ntp.org"; // NTP server
const long  gmtOffset_sec = 0;          // Adjust according to your timezone (in seconds)
const int   daylightOffset_sec = 0;     // Daylight offset (in seconds)

// DHT11 setup
#define DHTPIN 2                 // DHT11 sensor pin connected to GPIO2 (D4 on D1 Mini)
#define DHTTYPE DHT11            // Define the sensor type

DHT dht(DHTPIN, DHTTYPE);        // Initialize DHT sensor

// API endpoints
const char* temperatureEndpoint = "";
const char* helloEndpoint = "";

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

  dht.begin();                      // Start DHT sensor

  // Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Waiting for NTP time sync...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println("Time synchronized.");
}

void sendHelloRequest() {
  if (WiFi.status() == WL_CONNECTED) { // Ensure Wi-Fi is connected
    WiFiClientSecure client;
    client.setInsecure(); // Disable SSL certificate verification (for testing purposes)
    HTTPClient http;

    // Begin HTTPS connection
    if (http.begin(client, helloEndpoint)) {
      int httpResponseCode = http.GET(); // Send GET request
      if (httpResponseCode > 0) {
        Serial.print("Hello Endpoint Response Code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.print("Hello Response: ");
        Serial.println(response);
      } else {
        Serial.print("Failed to connect to Hello endpoint, Error Code: ");
        Serial.println(httpResponseCode);
      }
      http.end(); // Close connection
    } else {
      Serial.println("Unable to connect to Hello endpoint");
    }
  } else {
    Serial.println("WiFi not connected for Hello endpoint");
  }
}

void sendTemperatureData(float temperature, float humidity, unsigned long timestamp) {
  if (WiFi.status() == WL_CONNECTED) { // Ensure Wi-Fi is connected
    WiFiClientSecure client;
    client.setInsecure(); // Disable SSL certificate verification (for testing purposes)
    HTTPClient http;
    http.setTimeout(10000);              // Set timeout to 10 seconds

    // Begin HTTPS connection
    if (http.begin(client, temperatureEndpoint)) {
      http.addHeader("Content-Type", "application/json"); // Set content type to JSON

      // Create JSON payload
      String payload = "{\"timestamp\":" + String(timestamp) + ",\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
      Serial.print("Payload: ");
      Serial.println(payload);

      // Send HTTP POST request
      int httpResponseCode = http.POST(payload);

      // Check HTTP response
      if (httpResponseCode > 0) {
        Serial.print("Temperature Endpoint Response Code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.print("Temperature Response: ");
        Serial.println(response);
      } else {
        Serial.print("Failed to send data to Temperature endpoint, Error Code: ");
        Serial.println(httpResponseCode);
      }
      http.end(); // Free resources
    } else {
      Serial.println("Unable to connect to Temperature endpoint");
    }
  } else {
    Serial.println("WiFi not connected for Temperature endpoint");
  }
}

void loop() {
  delay(5000); // Wait 5 seconds between readings

  // Send Hello GET request
  sendHelloRequest();

  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print the results
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  // Get current Unix timestamp
  time_t now = time(nullptr); // Get the current time in seconds since Jan 1, 1970
  unsigned long timestamp = now;

  // Check if time is valid
  if (now < 100000) { // Arbitrary threshold to check if time is valid
    Serial.println("Failed to obtain valid time");
    return;
  }

  // Print the timestamp
  Serial.print("Unix Timestamp: ");
  Serial.println(timestamp);

  // Send Temperature POST request
  sendTemperatureData(temperature, humidity, timestamp);
}
