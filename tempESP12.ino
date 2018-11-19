#include <sht21.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

const char* ssid = "Bbox-CB1A2827";
const char* password = "5E72C534A564317ADF2D13F7C99C4F";
const char* mqtt_server = "192.168.1.11";

char temp[5];
char humd[5];
char cons[5];
char hyst[5];
const char* keyDevice = "TOPIC";
sht21 SHT21;  
float t = 0.0;
float h = 0.0;
int sda = D1;  // GPIO5
int scl = D2; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
float consigne = 19.5;
float c = 19.5;
float hysteresis = 0.4;

void setup() {
  Serial.begin(9600);
  Wire.begin(D1,D2);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);   
  SHT21.begin(sda, scl); 
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.println((char *)topic);
  payload[length] = '\0';
  String top = String((char*)topic);
  Serial.println(top);
  String s = String((char*)payload);
  Serial.println(s);
  if(top == "tempghtd/cons")
  {
    Serial.println("test");
    if(c != s.toFloat())
    {
      Serial.println("différent");
      c = s.toFloat();
    }
    Serial.println(c);
  }
  if(top == "tempghtd/hyst")
  {
    hysteresis = s.toFloat();
  }
}
void reconnect() {

  while (!client.connected()) {

    if (client.connect(keyDevice)) {
      
      Serial.println("reconnect");     
      client.subscribe("tempghtd/cons");
      client.subscribe("tempghtd/hyst");
    } else {
      delay(5000);
    }
  }
}

int tellstate = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  } 
  t = SHT21.lecture_temperature();
  h = SHT21.lecture_humidite();
  
  if ( testMillis() - tellstate >= 4000 ) {
  dtostrf (t,5,2,temp);  
  dtostrf(h,5,0,humd);
  dtostrf (c,5,2,cons);
  dtostrf (hysteresis,5,2,hyst);
  client.publish("tempghtd",temp);   
  client.publish("humdghtd", humd);  
  client.publish("tempghtd/consM", cons);
  client.publish("tempghtd/hysteresis", hyst);
  chauffe();
  tellstate = testMillis();
  }
  client.loop();

}

unsigned long testMillis()
{
  return 0xFFFEB3F8 + millis();
}


