#include <MFRC522.h>//
#include <WiFi.h>
#include <ArduinoJson.h>//
#include<HTTPClient.h>//
#include <WiFiUdp.h>
#include <NTPClient.h>//
//#include <WebSocketsClient.h> //gilmaimon/ArduinoWebsockets@^0.5.3 
#include <PubSubClient.h>//
#include <string.h> 
#include <Arduino.h>
#include <stdint.h>

#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

#define RST_PIN 4
#define SS_PIN 2

MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long previousMillis = 0;
unsigned long interval = 30000;
const char* host_ip= "35.176.71.115";
int host_port = 3000;

//MQTT Broker data:
const char *broker = "35.176.71.115";
const char *mqtt_user ="marsrover";
const char *mqtt_pass = "marsrover123";
const int mqtt_port = 1883;
char command_msg[256];
char coord_msg[256];
char rControl_msg[256];

//Json variables for Publishing
StaticJsonDocument <256> location;
StaticJsonDocument <256> battery;
StaticJsonDocument <256> aliens;
StaticJsonDocument <256> fans;
StaticJsonDocument <256> buildings;

//Json variables for Subscribing
StaticJsonDocument <256> centralCommand; // check setup()
StaticJsonDocument <256> rControl;
StaticJsonDocument <256>coordinates;

bool autoMode = false; //true if autopilot is on

//String command = "";
//String roverCommand = "";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) { //Data received
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.print(command_msg);

  if (String(topic) =="coordinates"){
    if (String(command_msg) == "{\"mode\":2}"){
      deserializeJson(coordinates,payload);
      serializeJson(coordinates,coord_msg);
      Serial.println(coord_msg);
      if (coordinates["ycoord"] ==5){
        Serial.println("Hallelujah");
      }
    }
  }

  else if (String(topic) == "rControl"){
    if (String(command_msg) =="{\"mode\":1}"){
      deserializeJson(rControl,payload);
      Serial.println("REmote: ");
      serializeJson(rControl,rControl_msg);
      Serial.println(rControl_msg);
      if (rControl["directionMove"] =="F"){
        Serial.print("Rover moving Forward");
      }
  }
  }

  else if (String(topic)=="centralCommand"){
    deserializeJson(centralCommand,payload);
    //serializeJson(centralCommand,msg);
    Serial.println("Central msg: ");
    serializeJson(centralCommand,command_msg);
    Serial.print(command_msg);
  
  }
  //if (centralCommand["mode"]!=0){
  

  if (String(command_msg) =="{\"mode\":3}")
    Serial.println("Autopilot Mode entered");
    { autoMode = true;}


Serial.println();
Serial.println("-----------------------");

}

void pub(String message,char *topic){
  //convert string to char*
  const char *cstr = message.c_str();
  client.publish(topic, cstr); 
  Serial.println("Message sent");
}

void sub(char *topic){
  Serial.println("Waiting for sub...");
  client.subscribe(topic);
}

void mqttConnect(){
  client.setServer(broker,mqtt_port);
  client.setCallback(callback);

  while(!client.connected()){ //connects if not connected
    String client_id = "esp32-client-";
    client_id+= String(WiFi.macAddress());
    if (client.connect(client_id.c_str(),mqtt_user,mqtt_pass)){
      Serial.println("Connected to MQTT Broker");
    }
    else{
      Serial.println("Failed with state: ");
      Serial.print(client.state());
    }
  }
}

void initWifi(){
  WiFi.mode(WIFI_STA); //Connection Mode (Connecting to Access Point Mode)

  int networks = WiFi.scanNetworks();
  Serial.println("Networks: "+networks);
  for (int i=0;i<networks;i++){
    Serial.println("Name: "+WiFi.SSID(i));}

      //**Access Point Details**//
  const char* ssid = "angelophone"; //CHANGE FOR OTHER WIFI/NETWORK CONNECTIONS
  const char* password = "hotpasss";


  WiFi.begin(ssid,password);
  Serial.print(" Connecting to WiFi ...");
  while (WiFi.status()!=WL_CONNECTED){
    Serial.print('.');
    delay(1000);
  }
  Serial.println("Connected Successfully");
  Serial.println(WiFi.localIP());

}

void wifi_check(){
  unsigned long currentMillis = millis();
  if (WiFi.status()!=WL_CONNECTED && (currentMillis - previousMillis >=interval)){
    Serial.println("Reconnecting to Wifi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

//Resources:
//https://www.emqx.com/en/blog/esp32-connects-to-the-free-public-mqtt-broker
//https://iotdesignpro.com/projects/real-time-data-transfer-between-two-esp32-using-websocket-client-on-arduino-ide
//https://masteringjs.io/tutorials/express/websockets
//http://www.martyncurrey.com/esp8266-and-the-arduino-ide-part-9-websockets/

//MQTT PubSub help:
//http://www.steves-internet-guide.com/using-arduino-pubsub-mqtt-client/