#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <DHT.h>

// Pin configuration for DHT sensor
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials
const char *ssid = " ";       // Replace with your network SSID
const char *password = " "; // Replace with your network password

AsyncWebServer server(80); // Create AsyncWebServer object on port 80

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());  // Print local IP address
  
  dht.begin();  // Initialize DHT sensor
  
  // Define web server route to handle root ("/")
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    
      String html = "<html><head><meta charset='utf-8'>";
    html += "<script>setTimeout(function(){ location.reload(); }, 10000);</script>"; // JavaScript for page refresh every 10 seconds
    html += "</head><body><h1>ESP32 DHT11</h1>";
    html += "<p>Temperature: " + String(temperature) + " *C</p>";
    html += "<p>Humidity: " + String(humidity) + " %RH</p>";
    html += "</body></html>";
    
    request->send(200, "text/html", html);
  });

  // Start the web server
  server.begin();
}

void loop() {
  // Reading the DHT sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Print sensor data to Serial Monitor for debugging
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %RH | ");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  delay(10000); // Wait 10 seconds between readings
}
