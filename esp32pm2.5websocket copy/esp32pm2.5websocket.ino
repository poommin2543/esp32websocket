#include <WiFi.h>
#include <WebSocketClient.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(21,22); // RX, TX
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;
const char* ssid     = "ACSElab";
const char* password = "acselab1234";
char path[] = "/";
char host[] = "192.168.10.155:5000";
  
WebSocketClient webSocketClient;

// Use WiFiClient class to create TCP connections
WiFiClient client;

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  mySerial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network

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

  delay(5000);
  

  // Connect to the websocket server
  if (client.connect("192.168.10.155", 5000)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    while(1) {
      // Hang on failure
    }
  }

  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
    while(1) {
      // Hang on failure
    }  
  }

}


void loop() {
  String data;
  int index = 0;
char value;
char previousValue;
 
while (mySerial.available()) {
value = mySerial.read();
if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)){
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
while(mySerial.available()) mySerial.read();
Serial.println(" }");
delay(1000);

  if (client.connected()) {
    
    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
    }
   
    data = pm2_5;
    
    webSocketClient.sendData(data);
    
  } else {
    Serial.println("Client disconnected.");
    if (client.connect("192.168.10.155", 5000)) {
        Serial.println("Connected");}
//    continue;
    // while (1) {
    //   // Hang on disconnect.
    // }
  }
  
  // wait to fully let the client disconnect
  delay(1000);
  
}
