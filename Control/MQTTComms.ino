#include <string.h> 
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include<HTTPClient.h>
#include <WiFiUdp.h>
#include<NTPClient.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h>

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

//Json variables for Publishing
StaticJsonDocument <256> location_msg;
StaticJsonDocument <256> battery_msg;
StaticJsonDocument <256> obstacle_msg;

//Json variables for Subscribing
StaticJsonDocument <256> RControl_msg;
StaticJsonDocument<256> command_msg;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) { //Data received
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  //Serial.print("Message:");
  //for (int i = 0; i < length; i++) {

  deserializeJson(sub_msg,payload);
  //auto num = sub_msg["something"];
  char msg[128];
  int num = 10;
  num = sub_msg["something"];
  serializeJson(sub_msg,msg);
  Serial.println(msg);
  Serial.println(num);
  //}
    //Serial.print((char) payload[i]);
  //}
  Serial.println();
  Serial.println("-----------------------");

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

    //publishing and subscribing
    client.subscribe("#"); //subscribe to all topics 
    char msg_char[128];
    serializeJson(pub_msg,msg_char);
    client.publish("test",msg_char);
    
    
    
  }

}

void initWifi(){
  WiFi.mode(WIFI_STA); //Connection Mode (Connecting to Access Point Mode)

  int networks = WiFi.scanNetworks();
  Serial.println("Networks: "+networks);
  for (int i=0;i<networks;i++){
    Serial.println("Name: "+WiFi.SSID(i));}

      //**Access Point Details**//
  const char* ssid = "Wokwi-GUEST";
  const char* password = "";


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
void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(115200); //opens serial connection to print to console
  Serial.println("Hello, ESP32!");

  initWifi();
  mqttConnect();
  //initSocket();
}

void loop() {
  client.loop();
  wifi_check();
}

//Resources:
//https://www.emqx.com/en/blog/esp32-connects-to-the-free-public-mqtt-broker
//https://iotdesignpro.com/projects/real-time-data-transfer-between-two-esp32-using-websocket-client-on-arduino-ide
//https://masteringjs.io/tutorials/express/websockets
//http://www.martyncurrey.com/esp8266-and-the-arduino-ide-part-9-websockets/

//MQTT PubSub help:
//http://www.steves-internet-guide.com/using-arduino-pubsub-mqtt-client/