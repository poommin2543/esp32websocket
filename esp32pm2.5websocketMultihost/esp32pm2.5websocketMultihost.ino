#include <WiFi.h>
#include <WebSocketClient.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(21, 22); // RX, TX
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

const char* ssid = "ACSElab";
const char* password = "acselab1234";

// List of hosts
char host1[] = "192.168.10.155";
char host2[] = "192.168.10.130";
char* hosts[] = {host1, host2};
const int port = 5000;
const int numHosts = sizeof(hosts) / sizeof(hosts[0]);

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

bool connectToWebSocket() {
  for (int i = 0; i < numHosts; i++) {
    if (client.connect(hosts[i], port)) {
      Serial.print("Connected to server: ");
      Serial.println(hosts[i]);
      webSocketClient.path = "/";
      webSocketClient.host = hosts[i];
      if (webSocketClient.handshake(client)) {
        Serial.println("Handshake successful");
        return true;  // Connected successfully
      } else {
        Serial.println("Handshake failed.");
      }
    } else {
      Serial.print("Connection to server failed: ");
      Serial.println(hosts[i]);
    }
  }
  return false;  // All connection attempts failed
}

void setup() {
  Serial.begin(115200);
  delay(10);
  while (!Serial);
  mySerial.begin(9600);
  connectToWiFi();
  delay(5000);

  if (!connectToWebSocket()) {
    Serial.println("All connection attempts failed.");
    while (1);  // Hang if unable to connect to any server
  }
}

void loop() {
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
    }
    else if (index == 5) {
      pm1 = 256 * previousValue + value;
      Serial.print("{ ");
      Serial.print("\"pm1\": ");
      Serial.print(pm1);
      Serial.print(" ug/m3");
      Serial.print(", ");
    }
    else if (index == 7) {
      pm2_5 = 256 * previousValue + value;
      Serial.print("\"pm2_5\": ");
      Serial.print(pm2_5);
      Serial.print(" ug/m3");
      Serial.print(", ");
    }
    else if (index == 9) {
      pm10 = 256 * previousValue + value;
      Serial.print("\"pm10\": ");
      Serial.print(pm10);
      Serial.print(" ug/m3");
    } else if (index > 15) {
      break;
    }
    index++;
  }
  while (mySerial.available()) mySerial.read();
  Serial.println(" }");

  if (client.connected()) {
    String data;
    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
    }

    String pm2_5_str = String(pm2_5);  // Convert pm2_5 to a string
    webSocketClient.sendData(pm2_5_str.c_str());

  } else {
    Serial.println("Client disconnected. Reconnecting...");
    if (!connectToWebSocket()) {
      Serial.println("All connection attempts failed.");
      while (1);  // Hang if unable to reconnect to any server
    }
  }

  delay(500);
}
