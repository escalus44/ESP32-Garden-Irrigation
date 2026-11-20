
/*
 * ESP32 Garden Irrigation Controller
 * Version: 1.0
 *
 * Hardware:
 *   ESP32-WROOM-32
 *   5V single-channel relay (HIGH trigger)
 *   U.S. Solid 9-36V motorized ball valve
 *   LM2596 12V-to-5V buck converter
 *   12.8V LiFePO4 battery
 *
 * Features:
 *   - Home Wi-Fi connectivity
 *   - Built-in HTTP web server
 *   - Open / Close valve controls
 *   - Real-time commanded valve status
 *   - Automatic shutoff after 5 minutes
 *   - Valve OFF on startup
 *
 * Future:
 *   - NTP clock synchronization
 *   - Watering schedules
 *   - Persistent configuration
 *   - Mobile-friendly app interface
 */

#include <WiFi.h>
#include <WebServer.h>

// -----------------------------------------
// WIFI CONFIGURATION
// -----------------------------------------

// Replace locally with your Wi-Fi credentials.
// Do not commit your actual password to GitHub.

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// -----------------------------------------
// HARDWARE CONFIGURATION
// -----------------------------------------

#define RELAY_PIN 23

// Relay configured for HIGH triggering.
#define RELAY_ON HIGH
#define RELAY_OFF LOW

// Maximum continuous watering duration:
// 5 minutes in milliseconds.
const unsigned long MAX_OPEN_TIME = 300000UL;

// -----------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------

WebServer server(80);

bool valveOpen = false;
unsigned long valveOpenedAt = 0;

// -----------------------------------------
// VALVE CONTROL
// -----------------------------------------

void closeValve() {
  digitalWrite(RELAY_PIN, RELAY_OFF);

  valveOpen = false;

  Serial.println("Valve CLOSED");
}

void openValve() {
  digitalWrite(RELAY_PIN, RELAY_ON);

  valveOpen = true;
  valveOpenedAt = millis();

  Serial.println("Valve OPENED");
}

// -----------------------------------------
// WEB INTERFACE
// -----------------------------------------

void handleRoot() {

  String page = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>

<meta charset="UTF-8">
<meta name="viewport"
      content="width=device-width, initial-scale=1">

<title>Garden Irrigation Controller</title>

<style>

body {
  font-family: Arial, sans-serif;
  background: #17212b;
  color: white;
  text-align: center;
  padding: 30px 15px;
}

.container {
  max-width: 420px;
  margin: auto;
  background: #263544;
  padding: 25px;
  border-radius: 15px;
}

h1 {
  margin-bottom: 10px;
}

button {
  width: 100%;
  padding: 20px;
  margin: 10px 0;
  border: none;
  border-radius: 10px;
  font-size: 20px;
  font-weight: bold;
  cursor: pointer;
}

.open {
  background: #219653;
  color: white;
}

.close {
  background: #c0392b;
  color: white;
}

#status {
  font-size: 25px;
  margin: 25px;
}

.footer {
  color: #b7c9d6;
  margin-top: 25px;
  font-size: 14px;
}

</style>

</head>

<body>

<div class="container">

<h1>Garden Irrigation</h1>

<p>ESP32 Water Controller</p>

<div id="status">Checking...</div>

<button class="open"
 onclick="sendValveCommand('open')">
 OPEN VALVE
</button>

<button class="close"
 onclick="sendValveCommand('close')">
 CLOSE VALVE
</button>

<div class="footer">
Automatic shutoff: 5 minutes
<br>
ESP32 Irrigation Controller v1.0
</div>

</div>

<script>

// Send a command to the ESP32.
async function sendValveCommand(action) {

  try {
    const response = await fetch('/' + action);

    if (!response.ok) {
      throw new Error('Command failed');
    }

    await updateStatus();

  } catch (error) {
    console.error(error);
    document.getElementById('status').innerText =
      'CONNECTION ERROR';
  }
}

// Retrieve current valve status.
async function updateStatus() {

  try {
    const response = await fetch('/status');

    if (!response.ok) {
      throw new Error('Status request failed');
    }

    const data = await response.json();

    document.getElementById('status').innerText =
      data.open ? 'VALVE OPEN' : 'VALVE CLOSED';

  } catch (error) {
    console.error(error);
    document.getElementById('status').innerText =
      'CONNECTION ERROR';
  }
}

// Refresh status every two seconds.
setInterval(updateStatus, 2000);

updateStatus();

</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

// -----------------------------------------
// STATUS API
// -----------------------------------------

void handleStatus() {

  String response = "{\"open\":";

  response += valveOpen ? "true" : "false";

  response += "}";

  server.send(
    200,
    "application/json",
    response
  );
}

// -----------------------------------------
// WIFI CONNECTION
// -----------------------------------------

void connectWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

  }

  Serial.println();
  Serial.println("WiFi connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

// -----------------------------------------
// WEB SERVER ROUTES
// -----------------------------------------

void setupWebServer() {

  server.on("/", handleRoot);

  server.on("/open", []() {

    openValve();

    server.send(
      200,
      "text/plain",
      "Valve opened"
    );

  });

  server.on("/close", []() {

    closeValve();

    server.send(
      200,
      "text/plain",
      "Valve closed"
    );

  });

  server.on("/status", handleStatus);

  server.begin();

  Serial.println("Web server started!");
}

// -----------------------------------------
// ARDUINO SETUP
// -----------------------------------------

void setup() {

  // Set output LOW before enabling GPIO.
  digitalWrite(RELAY_PIN, RELAY_OFF);
  pinMode(RELAY_PIN, OUTPUT);

  valveOpen = false;

  Serial.begin(115200);

  Serial.println();
  Serial.println("ESP32 Garden Irrigation Controller");
  Serial.println("Version 1.0");

  // Connect to home network.
  connectWiFi();

  // Start web interface.
  setupWebServer();
}

// -----------------------------------------
// ARDUINO MAIN LOOP
// -----------------------------------------

void loop() {

  // Process browser requests.
  server.handleClient();

  // Five-minute safety shutoff.
  if (valveOpen &&
      millis() - valveOpenedAt >= MAX_OPEN_TIME) {

    closeValve();

    Serial.println(
      "Automatic shutoff activated"
    );
  }
}
