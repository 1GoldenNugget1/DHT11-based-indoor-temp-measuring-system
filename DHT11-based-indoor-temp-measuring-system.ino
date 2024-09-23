#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <DHT.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

// Pin configuration for DHT sensor
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials
const char *ssid = " ";       // Replace with your network SSID
const char *password = " "; // Replace with your network password

AsyncWebServer server(80); // Create AsyncWebServer object on port 80

// Global variables to store the latest sensor readings
float currentTemperature = 0.0;
float currentHumidity = 0.0;

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

  // Route to serve sensor data as JSON
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    // JSON response for the temperature and humidity
    String json = "{\"temperature\":" + String(currentTemperature) + ",\"humidity\":" + String(currentHumidity) + "}";
    request->send(200, "application/json", json);
  });

  // Define web server route to handle root ("/")
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  // HTML content with Chart.js and styled text
  String html = "<html><head><meta charset='utf-8'>";
  html += "<style> body { font-family: Arial, sans-serif; text-align: center; }";
  html += "h1 { color: #333; }";
  html += "#temperature, #humidity { font-size: 24px; margin-top: 10px; }";
  html += "#temperature { color: red; }";
  html += "#humidity { color: blue; }</style>";
  html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"; // Load Chart.js
  html += "<script>";
  html += "async function fetchData() {";
  html += "  const response = await fetch('/data');";
  html += "  const data = await response.json();";
  html += "  document.getElementById('temperature').innerText = 'Temperature: ' + data.temperature + ' °C';";
  html += "  document.getElementById('humidity').innerText = 'Humidity: ' + data.humidity + ' %RH';";
  html += "  updateChart(data.temperature, data.humidity);";
  html += "}";
  html += "function updateChart(temperature, humidity) {";
  html += "  const now = new Date().toLocaleTimeString();";
  html += "  myChart.data.labels.push(now);";
  html += "  myChart.data.datasets[0].data.push(temperature);";
  html += "  myChart.data.datasets[1].data.push(humidity);";
  html += "  if (myChart.data.labels.length > 10) {";
  html += "    myChart.data.labels.shift();";
  html += "    myChart.data.datasets[0].data.shift();";
  html += "    myChart.data.datasets[1].data.shift();";
  html += "  }";
  html += "  myChart.update();";
  html += "}";
  html += "setInterval(fetchData, 10000);"; // Fetch new data every 10 seconds
  html += "</script></head><body onload='fetchData()'><h1>ESP32 DHT11 Sensor</h1>";
  
  // Moved temperature and humidity text above the graph
  html += "<div id='temperature'></div><div id='humidity'></div>"; 
  
  // Smaller graph canvas (e.g., width=300, height=150)
  html += "<canvas id='myChart' width='100' height='20'></canvas>";
  
  html += "<script>";
  html += "const ctx = document.getElementById('myChart').getContext('2d');";
  html += "const myChart = new Chart(ctx, {";
  html += "  type: 'line',";
  html += "  data: { labels: [], datasets: [{label: 'Temperature (°C)', borderColor: 'red', data: []}, {label: 'Humidity (%RH)', borderColor: 'blue', data: []}] },";
  html += "  options: { scales: { x: { title: { display: true, text: 'Time' } }, y: { title: { display: true, text: 'Value' } } } }";
  html += "});";
  html += "</script>";
  html += "</body></html>";
  request->send(200, "text/html", html);
});

  // Start the web server
  server.begin();
}

void loop() {
  // Reading the DHT sensor data
  currentHumidity = dht.readHumidity();
  currentTemperature = dht.readTemperature();
  
  // Print sensor data to Serial Monitor for debugging
  Serial.print("Humidity: ");
  Serial.print(currentHumidity);
  Serial.print(" %RH | ");
  Serial.print("Temperature: ");
  Serial.print(currentTemperature);
  Serial.println(" °C");

  delay(10000); // Wait 10 seconds between readings
}