#include <WiFi.h>
#include <WebSocketsClient.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(21, 22); // RX, TX
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;
const char* ssid = "ACSElab";
const char* password = "acselab1234";
const char* host = "192.168.10.155";
const int port = 5000;
const char* path = "/";

// Create an instance of the WebSocketsClient
WebSocketsClient webSocket;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  delay(10);

  // We start by connecting to a Wi-Fi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to the WebSocket server
  webSocket.begin(host, port, path);

  // Set up event handlers
  webSocket.onEvent(webSocketEvent);

  // Give the WebSocket some time to connect
  delay(5000);
}

void loop() {
  webSocket.loop(); // Maintain WebSocket connection

  int index = 0;
  char value;
  char previousValue;

  while (mySerial.available()) {
    value = mySerial.read();
    if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)) {
      Serial.println("Cannot find the data header.");
      break;
    }

    if (index == 4 || index == 6 || index == 8 || index == 10 || index == 12 || index == 14) {
      previousValue = value;
    } else if (index == 5) {
      pm1 = 256 * previousValue + value;
    } else if (index == 7) {
      pm2_5 = 256 * previousValue + value;
    } else if (index == 9) {
      pm10 = 256 * previousValue + value;
    } else if (index > 15) {
      break;
    }
    index++;
  }
  while (mySerial.available()) mySerial.read();

  // Print values to the Serial Monitor
  Serial.print("{ ");
  Serial.print("\"pm1\": ");
  Serial.print(pm1);
  Serial.print(" ug/m3, ");
  Serial.print("\"pm2_5\": ");
  Serial.print(pm2_5);
  Serial.print(" ug/m3, ");
  Serial.print("\"pm10\": ");
  Serial.print(pm10);
  Serial.println(" ug/m3 }");

  // Send data over WebSocket
  String jsonData = "{ \"pm1\": " + String(pm1) + ", \"pm2_5\": " + String(pm2_5) + ", \"pm10\": " + String(pm10) + " }";
  webSocket.sendTXT(jsonData);

  delay(1000); // wait a second before next loop
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}
