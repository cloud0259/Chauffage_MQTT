#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";

const char* keyDevice = "topic";

#define relay RX
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
WiFiClient espClient;
PubSubClient client(espClient);

String temp = "0.0";
void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
   
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println(WiFi.localIP());
  display.display();
}

void setup_wifi() {

  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  if(strcmp(topic,"topic")==0)
  {
    if ((char)payload[0] == '0')
    {
      digitalWrite(relay, LOW); 
    }
    else if((char)payload[0] == '1')
    {
     digitalWrite(relay, HIGH);
    }
  }
  else if(strcmp(topic,"temp")==0)
  {
    payload[length] = '\0';
    temp = String((char*)payload);
    Serial.println(temp);
  }
}

void reconnect() {

  while (!client.connected()) {

    if (client.connect(keyDevice)) {
      
      Serial.println("reconnect");    
      client.subscribe(keyDevice);
      client.subscribe("temp");
    } else {
      delay(5000);
    }
  }
}

int tellstate = 0;

void loop() {
  
  if (!client.connected()) {
    reconnect();
    delay(5000);
    //client.publish("casa", keyDevice);
  } 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30,0);
  display.clearDisplay();
  if ( digitalRead(relay) ) {
        client.publish("topic/relay","on");
         display.println("ON");
  
      } else {
        client.publish("topic/relay","off");
        display.println("OFF");
      }
   display.setCursor(25,20);
   display.println(temp);
  
  display.display();
 
  client.loop();
}
