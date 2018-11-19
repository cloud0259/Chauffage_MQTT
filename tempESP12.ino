#include <sht21.h> //lib sht21
#include <ESP8266WiFi.h> //lib ESP8266 Wifi
#include <PubSubClient.h> //lib MQTT
#include <Wire.h> //lib One Wire

const char* ssid = ""; //Entrée le SSID de votre point d'accès
const char* password = ""; //Entrée le mot de passe
const char* mqtt_server = ""; //Adresse du serveur MQTT

char temp[5]; //Tableau de char pour la temperature 
char humd[5]; //Tableau de char pour l'humidite
char cons[5]; //Tableau de char pour la consigne
char hyst[5]; //Tableau de char pour l'hysteresis

const char* keyDevice = "TOPIC"; //Nom du topic principal
sht21 SHT21;  //Instance de sht21
float t = 0.0; //Variable temperature float
float h = 0.0; //Variable humidite float
float c = 19.5; //Variable consigne float
float hysteresis = 0.4; //Variable consigne hysteresis float
int sda = D1;  //  SDA sur D1
int scl = D2;  // SDL sur D2

WiFiClient espClient; // Instance de WifiClient
PubSubClient client(espClient); //Déclaration du client MQTT

/***************************************************************************/
/**************************SETUP********************************************/
/***************************************************************************/
void setup() {
  Serial.begin(9600); //Initialisation de la voie serie à 9600 bauds  
  setup_wifi(); // Appel de la fonction connection wifi
  client.setServer(mqtt_server, 1883); //Initialise le client MQTT avec l'hote et le port (1883 par defaut)
  client.setCallback(callback);   //Initialise le Callback sur la fonction callback
  SHT21.begin(sda, scl);  //Initialise SHT21 sur D1 et D2 pour ESP12E
}

/**************************************************************************/
/************************SETUP_WIFI****************************************/
/**************************************************************************/
void setup_wifi() 
{
  delay(10); //Attendre 10 ms
  WiFi.begin(ssid, password); //Demarre la connection wifi avec le SSID et le mot de passe
  while (WiFi.status() != WL_CONNECTED)
  { //Tant que l'on est pas connecté
    delay(500); //Attendre 500ms
  } 
}

/**************************************************************************/
/**********************CALLBACK********************************************/
/**************************************************************************/
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.println((char *)topic); //Verifie sur le terminal Serie le topic
  payload[length] = '\0'; //Inscrit '\0' sur le dernier caractere de la chaine recu
  String top = String((char*)topic); //Converti le topic en String
  String s = String((char*)payload); //Converti la chaine recu en string
  if(top == "temp/cons") //Si le topic et temp/cons (consigne
  {
    if(c != s.toFloat()) //Si la consigne est différente de la consigne reçu
    {
      c = s.toFloat(); //alors la consigne egale la consigne reçu
    }
    Serial.println(c); //On vérifie dans le terminal la consigne
  }
  if(top == "temp/hyst") //Si le topic est temp/hyst (hysteresis)
  {
    hysteresis = s.toFloat(); //alors l'hysteresis est égale a celui reçu
    Serial.println(c); //On vérifie l'hysteresis
  }
  
}

/****************************************************************************/
/************************RECONNECT*******************************************/
/****************************************************************************/
void reconnect() {

  while (!client.connected()) { //Si le client n'est pas connecté

    if (client.connect(keyDevice)) //Connection du client
    {     
      Serial.println("reconnect"); //On valide dans le terminal    
      client.subscribe("temp/cons"); //On souscrit a la consigne
      client.subscribe("temp/hyst"); //On souscrit a l'hysteresis
    } 
    else 
    {
      delay(5000); //Attend 5 secondes
    }
  }
}

int tellstate = 0; //Variable pour l'écart de temp

/******************************************************************************/
/**********************LOOP****************************************************/
/******************************************************************************/
void loop() 
{
  if (!client.connected()) //Si me client n'est pas connecté
  {
    reconnect(); //Lance la fonction de reconnection
  } 
  t = SHT21.lecture_temperature(); //Récupération de la température
  h = SHT21.lecture_humidite(); //Récupération de l'humidité
  
  if ( testMillis() - tellstate >= 4000 ) //si 4 secondes sont passé
  {
    dtostrf (t,5,2,temp); //converti la temperature en tableau de char 
    dtostrf(h,5,0,humd);  //converti l'humidite en tableau de char
    dtostrf (c,5,2,cons); //converti la consigne en tableau de char
    dtostrf (hysteresis,5,2,hyst); //converti l'hysteresis en tableau de char
    client.publish("temp",temp);   //publie la temperature
    client.publish("humd", humd);  //publie l'humidite
    client.publish("temp/consM", cons); //publie la consigne
    client.publish("temp/hysteresis", hyst); //publie l'hysteresis
    chauffe(); //Appel la fonction de chauffe
    tellstate = testMillis(); // Met a jour le temp actuel
  }
  client.loop(); //Boucle client

}


/***************************************************************/
/*******************TESTMILLIS**********************************/
/***************************************************************/
unsigned long testMillis()
{
  return 0xFFFEB3F8 + millis(); // return un millis qui ne bloque jamais.
}

