#include <ESP8266WiFi.h>
#include <MicroGear.h>

const char* ssid     = "YOUR_SSID";        // ชื่อ ssid
const char* password = "YOUR_PASSWORD";    // รหัสผ่าน wifi

#define APPID   "YOUR_APPID"          // ให้แทนที่ด้วย AppID
#define KEY     "YOUR_KEY"            // ให้แทนที่ด้วย Key
#define SECRET  "YOUR_SECRET"         // ให้แทนที่ด้วย Secret

#define ALIAS   "pieled"

WiFiClient client;

int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) { // 
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
    if(*(char *)msg == '1'){
        digitalWrite(LED_BUILTIN, LOW);   // LED on
        microgear.chat("switch","1");
    }else{
        digitalWrite(LED_BUILTIN, HIGH);  // LED off
        microgear.chat("switch","0");
    }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setName(ALIAS);
}


void setup() {
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    pinMode(LED_BUILTIN, OUTPUT);

    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);
}

void loop() {
    if (microgear.connected()) {
        Serial.println("..."); 
        microgear.loop();
        timer = 0;
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID); 
            timer = 0;
        }
        else timer += 100;
    }
    delay(100);
}
