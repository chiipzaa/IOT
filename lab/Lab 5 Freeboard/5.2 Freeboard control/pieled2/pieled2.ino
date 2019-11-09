#include <ESP8266WiFi.h>
#include <MicroGear.h>

const char* ssid     = "SSID"; 
const char* password = "PASSWORD";

#define APPID   "YOUR_APPID"
#define KEY     "YOUR_KEY"
#define SECRET  "YOUR_SECRET"
 
#define ALIAS   "pieled"

WiFiClient client;

char state = 0;
char stateOutdated = 0;
char buff[16];

MicroGear microgear(client);

void sendState() {
  if (state==0)
    microgear.publish("/pieled/state","0");
  else
    microgear.publish("/pieled/state","1");
  Serial.println("send state..");
  stateOutdated = 0;
}

void updateIO() {
  if (state >= 1) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    state = 0;
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  char m = *(char *)msg;
  
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
  
  if (m == '0' || m == '1') {
    state = m=='0'?0:1;
    updateIO();
  }
  if (m == '0' || m == '1' || m == '?') stateOutdated = 1;
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
  stateOutdated = 1;
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    pinMode(LED_BUILTIN, OUTPUT);

    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }

    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);    
    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);
}

void loop() {
  if (microgear.connected()) {
    if (stateOutdated) sendState();
    microgear.loop();
  }
  else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
}
