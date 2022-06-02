#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <WiFi.h>

#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

#define RST_PIN 4
#define SS_PIN 2

MFRC522 mfrc522(SS_PIN, RST_PIN);

unsigned long previousMillis = 0; //Connection Time-out constants
unsigned long interval = 30000;

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  initWiFi();
}

void initWiFi(){
  WiFi.mode(WIFI_STA); //Connection Mode (Connecting to Access Point Mode)


  int networks = WiFi.scanNetworks();
  Serial.println("Networks: "+networks);
  for (int i=0;i<networks;i++){
    Serial.println("Name: "+WiFi.SSID(i));}

      //**Access Point Details**//
  std::string ssid = "Wokwi-GUEST";
  const char *ssidchar = ssid.c_str();
  std::string password = "";
  const char *passwordchar = ssid.c_str(); //converts string to appropriate format


  WiFi.begin("Wokwi-GUEST","");
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

void loop() {
  wifi_check();
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