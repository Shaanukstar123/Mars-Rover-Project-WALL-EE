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

// #define CHA 0
// #define ENA 19 // this pin must be PWM enabled pin if Arduino board is used
// #define IN1 18
// #define IN2 5 //will not use this in the final version as it clashes with optic sensor. 
// //All seem to clash with optic sensor in some way but can be changed.
// // motor 2 settings
// #define IN3 17
// #define IN4 16
// #define ENB 4// this pin must be PWM enabled pin if Arduino board is used
// #define CHB 1

#define CHA 0
#define ENA 14 //for originals check the example code
#define IN1 12
#define IN2 13 

// motor 2 settings
#define IN3 17
#define IN4 16
#define ENB 4
#define CHB 1
const int CCW = 2; // do not change
const int CW  = 1; // do not change
#define motor1 1 // do not change
#define motor2 2 // do not change
// for two motors without debug information // Watch video instruciton for this line: https://youtu.be/2JTMqURJTwg
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);

//FPGA includes
#define PIN_SS        5
#define PIN_MISO      19
#define PIN_MOSI      23
#define PIN_SCK       18

//RoverObject stuff
class locationdata
{
  public: 
  int X;
  int Y;
  int angle;
  int BatteryPercentage;
};

locationdata rover; //create the rover object

//global testing variables for rover:
// rover.X = 0;
// rover.Y = 0; 
// rover.angle = 0;



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

 String recievedData;

String toBinary(int n)
{
    String r;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    return r;
}

int toInteger(String abc)
{
  int conv = 0;
  
  //for (int x = 0; x < abc.length(); x++)
  for (int x = abc.length() - 1; x >= 0; x--)
  {
    if (abc[x] == '1')
    {
      conv += pow(2, abc.length()-1 - x);
    }

    
  }
return conv;
}

String stringFlip(String input)
{
  String flipped;
  for (int x = 0; x < 16; x++)
  {
    flipped+=input[16-x];
  }
  return flipped;
}

void approximateLocation(int distance, int angle, String colour)
{
  //angle will be 0 to 180. 
  int angle_actual = rover.angle - angle;

  double xdisplacement = distance * sin(angle_actual*3.14159/180);
  double ydisplacement = distance * cos(angle_actual*3.14159/180);

  int alienX = rover.X + xdisplacement;
  int alienY = rover.Y + ydisplacement;

  String JSON = "{\n\tcolour : \"" + colour + "\"\n\tX-Coordinate : " + String(alienX) + "\n\tY-Coordinate : " + String(alienY) + "\n}";
  char topic[] = "Alien";
  //Serial.println("JSON string : " + JSON);
  

 
  pub(JSON, topic);

  //get current rover x and y. Using coordinates and the current angle the rover faces, find the real angle of the alien in relation to plane
  //from this, find coefficients (sin(angle)*distance, cos(angle)*distance)
  //use to generate approximate coordinates for the alien. 
  //alien.X = Rover.X + Xdisplacement;
  //alien.Y = Rpver.Y + Ydisplacement;
  //then send in JSON format the alien's colour, and location. 
  //String JSON = "{ colour : " + colourString + "\nxcoordinate : " + alienX + "\nycoordinate : " + alienY + "/n}";
  //pub(JSON, alien) 
}


void analyseData(String x)
{
  //int example = SPI.transfer16(0xff); //SPI input disabled for now
  //convert to binary
  //recievedData = toBinary(example);
  
  String alienColour = "";
  //Identification
  String alienBin;
  for (int y = 0; y < 3; y++) //extracting the distance
    {alienBin += x[y];}

  if (alienBin != "000")
  {
    if (alienBin == "001")
    {
      Serial.println("Red Alien detected.");
      alienColour = "Red";
      
    }

    if (alienBin == "010")
    {
      //green
      Serial.println("Green Alien detected.");
      alienColour = "Green";
    }

    if (alienBin == "011")
    {
      //blue
      Serial.println("Blue Alien detected.");
      alienColour = "Blue";
    }

    if (alienBin == "100")
    {
      //orange 
      Serial.println("Orange Alien detected.");
      alienColour = "Orange";
    }

    if (alienBin == "101")
    {
      //pink
      Serial.println("Pink Alien detected.");
      alienColour = "Pink";
    }

    if (alienBin == "110")
    {
      //grey
      Serial.println("Grey Alien detected.");
      alienColour = "Grey";
    }

    //Distance
    String distanceBin;
  
  for (int y = 3; y < 11; y++) //extracting the distance
    {distanceBin += x[y];}
    Serial.println(distanceBin);

    int distance = toInteger(distanceBin);

    Serial.print("Approximate distance from Rover : ");
    Serial.print(distance);
    Serial.println(" centimeters.");
    //angle

  String angleBin;

  for (int y = 11; y < 16; y++) //extracting the angle
    {angleBin += x[y];}
  
  int angle = toInteger(angleBin);

    Serial.print("Approximate angle from Rover : ");
    Serial.print(angle);
    Serial.println(" degrees.");

    approximateLocation(distance, angle, alienColour);
    
  }
}

void roverMovement(int speed)
{

  sub("Commands");

  //Serial.println("roverCommand = " + roverCommand);
  if (roverCommand == "{\n\"direction\" : 1\n}")
  {
   Serial.println("turned left!");
   robot.rotate(motor1, 30, CCW);
   robot.rotate(motor2, 30, CW);
  }

  if (roverCommand == "{\n\"direction\" : 2\n}")
  {
   Serial.println("turned right!");
   robot.rotate(motor1, 30, CW);
   robot.rotate(motor2, 30, CCW);
  }

  if (roverCommand == "{\n\"direction\" : 3\n}")
  {
   Serial.println("forwards!");
   robot.rotate(motor1, 30, CW);
   robot.rotate(motor2, 30, CW);
  }

  if (roverCommand == "{\n\"direction\" : 4\n}") 
  {
   Serial.println("backwards!");
   robot.rotate(motor1, 30, CCW);
   robot.rotate(motor2, 30, CCW);
  }

  if (roverCommand == "{\n\"direction\" : 5\n}") 
  {
   Serial.println("brake!");
   robot.brake(motor1);
   robot.brake(motor2);
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
  //SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(115200); //opens serial connection to print to console
  Serial.println("Hello, ESP32!");

  robot.begin();

  initWifi();
  mqttConnect();
  //initSocket();

  //FPGA side stuff
  pinMode(PIN_SS,OUTPUT);
  pinMode(PIN_MISO,INPUT);
  pinMode(PIN_MOSI,OUTPUT);
  pinMode(PIN_SCK,OUTPUT);

  Serial.begin(115200);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST); //Setting up SPI bus

  //setting up rover initials
  rover.X = 0;
  rover.Y = 0; 
  rover.angle = 0;
}

void loop() {
  client.loop();
  wifi_check();

  //low speed of only 10 for now for testing. 
  roverMovement(1);
  //analyseData("1000000000100000");
  
  delay(100);



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