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
WebSocketsClient webSocket;

//MQTT Broker data:

const char *broker = "broker.emqx.io";
const char *topic = "";
const char *mqtt_user ="username";
const char *mqtt_pass = "password";
const int mqtt_port = 1883;

void mqttConnect(){
  client.setServer(broker,mqtt_port);
  client.setCallback(callback);
  
}

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(115200); //opens serial connection to print to console
  Serial.println("Hello, ESP32!");

  initWiFi();
  initSocket();
}



void initWiFi(){
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

void initSocket(){

  webSocket.begin(host_ip, host_port, "/"); //address, port, URL route

  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

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

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length){
  if (type == WStype_TEXT){

  }
  webSocket.sendTXT("Hello there");
  Serial.println("Message sent");

}

void loop() {

  wifi_check();
  webSocket.loop();
  // put your main code here, to run repeatedly:
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
  delay(10); // this speeds up the simulation
}