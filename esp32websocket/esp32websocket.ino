#include <WiFi.h>
#include <WebSocketClient.h>

const char* ssid = "ACSElab";
const char* password = "acselab1234";
char path[] = "/";
char host[] = "192.168.10.155";
int port = 5000;

WebSocketClient webSocketClient;
WiFiClient client;

void connectToWiFi() {
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
}

void connectToWebSocket() {
  if (client.connect(host, port)) {
    Serial.println("Connected to server");
    webSocketClient.path = path;
    webSocketClient.host = host;
    if (webSocketClient.handshake(client)) {
      Serial.println("Handshake successful");
    } else {
      Serial.println("Handshake failed.");
    }
  } else {
    Serial.println("Connection to server failed.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  connectToWiFi();
  delay(5000);
  connectToWebSocket();
}

void loop() {
  if (client.connected()) {
    String data;
    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
    }

    data = "15";  // Ensure data is a string
    webSocketClient.sendData(data);

  } else {
    Serial.println("Client disconnected. Reconnecting...");
    connectToWebSocket();
  }

  delay(1000);
}
