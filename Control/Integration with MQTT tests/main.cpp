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
#include <NTPClient.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h>


#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

#define RST_PIN 4
#define SS_PIN 2

#include <Robojax_L298N_DC_motor.h>

#define CHA 0
#define ENA 19 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 18
#define IN2 5 //will not use this in the final version as it clashes with optic sensor. 
//All seem to clash with optic sensor in some way but can be changed.
// motor 2 settings
#define IN3 17
#define IN4 16
#define ENB 4// this pin must be PWM enabled pin if Arduino board is used
#define CHB 1
const int CCW = 2; // do not change
const int CW  = 1; // do not change
#define motor1 1 // do not change
#define motor2 2 // do not change
// for two motors without debug information // Watch video instruciton for this line: https://youtu.be/2JTMqURJTwg
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);

MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long previousMillis = 0;
unsigned long interval = 30000;
const char* host_ip= "35.176.71.115";
int host_port = 3000;
WebSocketsClient webSocket;

//MQTT Broker data:

const char *broker = "35.176.71.115";
char *topic = "test";
char *topic2 = "epic";
const char *mqtt_user ="marsrover";
const char *mqtt_pass = "marsrover123";
const int mqtt_port = 1883;

String roverCommand = "";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) { //Data received
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 String command = "";
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
     command+= (char) payload[i];
 }
 roverCommand = command;

 Serial.println();
 Serial.println("-----------------------");
}

void pub(String message,char *topic){
  client.publish(topic,"Message");
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
    pub("hello",topic);
    sub(topic2);
    
  }
  
  
}


void initWifi(){
  WiFi.mode(WIFI_STA); //Connection Mode (Connecting to Access Point Mode)

  int networks = WiFi.scanNetworks();
  Serial.println("Networks: "+networks);
  for (int i=0;i<networks;i++){
    Serial.println("Name: "+WiFi.SSID(i));}

      //**Access Point Details**//
  const char* ssid = "angelophone";
  const char* password = "hotpasss";


  WiFi.begin("angelophone","hotpasss");
  Serial.print(" Connecting to WiFi ...");
  while (WiFi.status()!=WL_CONNECTED){
    Serial.print('.');
    delay(1000);
  }
  Serial.println("Connected Successfully");
  Serial.println(WiFi.localIP());

}

/*void webSocketEvent(WStype_t type, uint8_t * payload, size_t length){
  if (type == WStype_TEXT){
  }
  webSocket.sendTXT("Hello there");
  Serial.println("Message sent");
} 
void initSocket(){
  webSocket.begin(host_ip, host_port, "/"); //address, port, URL route
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}
 */ //Websockets Not used anymore. MQTT used instead

void roverMovement()
{

  sub("Commands");

  Serial.println("roverCommand = " + roverCommand);
  if (roverCommand == "{\"direction\" : 1}") // left?
  {
   Serial.println("turned left!");
  }

  if (roverCommand == "{\"direction\" : 2}") // right?
  {
   Serial.println("turned right!");
  }

  if (roverCommand == "{\"direction\" : 3}") //forward
  {
   Serial.println("forward!");
  }

  if (roverCommand == "{\"direction\" : 4}") // left?
  {
   Serial.println("backwards!");
  }

  

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


  roverMovement();

  //while(1);

  delay(2000);



  /*webSocket.loop();
  if (mfrc522.PICC_IsNewCardPresent()){
    if(mfrc522.PICC_ReadCardSerial()){
      String idcard = "";
      for (byte i = 0; i<mfrc522.uid.size;i++){
        idcard +=(mfrc522.uid.uidByte[i]<0x10? "0": "") + String(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println("tag rfid :"+idcard);
      
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }
  delay(10); // this speeds up the simulation */
}

//Resources:
//https://www.emqx.com/en/blog/esp32-connects-to-the-free-public-mqtt-broker
//https://iotdesignpro.com/projects/real-time-data-transfer-between-two-esp32-using-websocket-client-on-arduino-ide
//https://masteringjs.io/tutorials/express/websockets
//http://www.martyncurrey.com/esp8266-and-the-arduino-ide-part-9-websockets/

//MQTT PubSub help:
//http://www.steves-internet-guide.com/using-arduino-pubsub-mqtt-client/