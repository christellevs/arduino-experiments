#include <ESP8266WiFi.h>  // Wi-Fi library
#include <DHT.h>          // DHT library
#include <Adafruit_Sensor.h>

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// DHT11 setup
#define DHTPIN 2          // DHT11 sensor pin connected to GPIO2 (D4 on D1 Mini)
#define DHTTYPE DHT11     // Define the sensor type

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

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

  dht.begin(); // Start DHT sensor
}

void loop() {
  // Wait a few seconds between measurements
  delay(5000);

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
}
