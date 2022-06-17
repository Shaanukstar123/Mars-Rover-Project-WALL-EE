//CURRENTLY SUPPORTS:
//GYROSCOPE
//MOTOR CONTROL
//WIFI
//MQTT CONNECTION

//TODO:
//FPGA (ready to implement)
//OPTIC SENSOR (needs some testing but otherwise ready)
//RADAR (waiting for component)
//BATTERY (unresolved problems currently on hardware side)


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
#include <WebSocketsClient.h> //gilmaimon/ArduinoWebsockets@^0.5.3 
#include <PubSubClient.h>
#include <Robojax_L298N_DC_motor.h> //DL from github

//Gyroscope Header
#include "gyro.h"
#include "FPGAheader.h"
#include "connection.h"

//SPI STUFF
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

#define RST_PIN 4
#define SS_PIN 2

//MOTOR SETTINGS
#define CHA 0
#define ENA 14 
#define IN1 13
#define IN2 12 
// motor 2 settings
#define IN3 17
#define IN4 16
#define ENB 25
#define CHB 1
const int CCW = 2; // do not change
const int CW  = 1; // do not change
#define motor1 1 // do not change
#define motor2 2 // do not change

Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);


//RoverObject stuff
class locationdata
{
  public: 
  int X;
  int Y;
  int angle;
  int anglePrev;
  int BatteryPercentage;
};

locationdata rover; //create the rover object

void approximateLocation(int distance, int angle, String colour, int roverX, int roverY, int roverAngle)
{
  int angle_actual = roverAngle - angle;

  double xdisplacement = distance * sin(angle_actual*3.14159/180);
  double ydisplacement = distance * cos(angle_actual*3.14159/180);

  int alienX = roverX + xdisplacement;
  int alienY = roverY + ydisplacement;

  String JSON = "{\n\tcolour : \"" + colour + "\"\n\tX-Coordinate : " + String(alienX) + "\n\tY-Coordinate : " + String(alienY) + "\n}";
  char topic[] = "Alien";

  pub(JSON, topic);
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
    if (alienBin == "001"){
      Serial.println("Red Alien detected.");
      alienColour = "Red";
    }

    if (alienBin == "010") {
      Serial.println("Green Alien detected.");
      alienColour = "Green";
    }

    if (alienBin == "011"){
      Serial.println("Blue Alien detected.");
      alienColour = "Blue";
    }

    if (alienBin == "100"){
      Serial.println("Orange Alien detected.");
      alienColour = "Orange";
    }

    if (alienBin == "101"){
      Serial.println("Pink Alien detected.");
      alienColour = "Pink";
    }

    if (alienBin == "110") {

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

    approximateLocation(distance, angle, alienColour, rover.X, rover.Y, rover.angle);
  }
}

void roverMovement()
{

  sub("Commands");
  Serial.println("Subbed to Commands");

  //Serial.println("roverCommand = " + roverCommand);
  if (roverCommand == "{\n\"direction\" : 1\n}") //bwd
  {
   Serial.println("backwards!");
   robot.rotate(motor1, 30, CCW);
   robot.rotate(motor2, 30, CW);
  }

  if (roverCommand == "{\n\"direction\" : 2\n}") //fwd
  {
   Serial.println("forwards!");
   robot.rotate(motor1, 30, CW);
   robot.rotate(motor2, 30, CCW);
  }

  if (roverCommand == "{\n\"direction\" : 3\n}") //cw
  {
   Serial.println("turn Clockwise!");
   robot.rotate(motor1, 30, CW);
   robot.rotate(motor2, 30, CW);
  }

  if (roverCommand == "{\n\"direction\" : 4\n}")  //ccw
  {
   Serial.println("turning counter clockwise!");
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


void setup() {
  
  mfrc522.PCD_Init();
  Serial.begin(115200); //opens serial connection to print to console

  // mpu.begin();
  // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  // mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  // mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  gyroSetup();
  
  rover.angle = 0;
  rover.anglePrev = 0;

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

  //FUNCTIONS FOR LOOP
  roverMovement();
  //analyseData("1000000000100000");
//   angleCalc(rover.angle);

// //ANGLE MEASUREMENT DEBUG
// if (rover.angle != rover.anglePrev)
// {
//   Serial.println(" rad/s");
//   Serial.print("Angle: ");
//   Serial.println(rover.angle);
//   Serial.println("");
//   rover.anglePrev = rover.angle;
// }
  delay(100);
}

