//CURRENTLY SUPPORTS:
//GYROSCOPE
//MOTOR CONTROL
//WIFI
//MQTT CONNECTION
//FPGA 

//TODO:
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
#include <Robojax_L298N_DC_motor.h> //DL from github


#include "FPGAheader.h"
//#include "connection.h"
#include "OpticSensor.h"
#include "Radar.h"

//SPI STUFF
// #define SCK 18
// #define MISO 19
// #define MOSI 23
// #define CS 5

// #define RST_PIN 4
// #define SS_PIN 2

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

//Optic Sensor stuff

Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);


//RoverObject stuff
class locationdata
{
  public: 
  int X;
  int Y;
  double Xdoub;
  double Ydoub; //for precise tracking of coordinates
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

//OPTIC SENSOR STUFF

char asciiart(int k)
{
  static char foo[] = "WX86*3I>!;~:,`. ";
  return foo[k>>4];
}

byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];

void angleConversion() {
  if (rover.angle < 0)
  {rover.angle+=360;}

  if (rover.angle >= 360)
  {rover.angle-=360;}  }

void angleCalc(){
  mpu.getEvent(&acc, &g, &temp);
  double radianspersec = g.gyro.z;
  
    if (abs(radianspersec) > 0.1){
    double degrees = 0.1*radianspersec*180/3.14159;
    rover.angle = rover.angle + degrees;
    angleConversion();
    }
}

void roverCoordUpdate(int dist)
{
  double xdisplacement = dist*sin(rover.angle);
  double ydisplacement = dist*cos(rover.angle);
  rover.Xdoub += xdisplacement/50;
  rover.Ydoub += ydisplacement/50;
  rover.X = rover.Xdoub;
  rover.Y = rover.Ydoub;

}

void printCoordinates()
{
  Serial.print("X: ");
Serial.println(rover.X);
Serial.print("Y: ");
Serial.println(rover.Y);
  Serial.print("Xdoub: ");
Serial.println(rover.Xdoub);
Serial.print("Ydoub: ");
Serial.println(rover.Ydoub);
Serial.println("Angle: " );
Serial.println(rover.angle);
}

void opticMain()
{
int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
  MD md;
  mousecam_read_motion(&md);
  for(int i=0; i<md.squal/4; i++)
    Serial.print('*');
  Serial.print(' ');
  Serial.print((val*100)/351);
  Serial.print(' ');
  Serial.print(md.shutter); Serial.print(" (");
  Serial.print((int)md.dx); Serial.print(',');
  Serial.print((int)md.dy); Serial.println(')');

  //delay(100);

    //distance_x = convTwosComp(md.dx);
    distance_y = convTwosComp(md.dy);
    Serial.println("Distance_y : " + String(distance_y));

roverCoordUpdate(distance_y); //modified by /100 to approximate to 1cm per cm moved. //closer possible
angleCalc();
printCoordinates();
}

int track = 0;

void roverDataTransfer()
{
  if (track >= 100)
  {
  String JSON = "{\n\t\"Angle\" : " + String(rover.angle) + "\n\t\"X-Coordinate\" : " 
  + String(rover.X) + "\n\t\"Y-Coordinate\" : " + String(rover.Y) + "\n}";
  
  char* topic = "Rover Status";
  pub(JSON, topic);
  track = 0;
  }
  track++;
}


void setup() {
  
  mfrc522.PCD_Init();
  Serial.begin(115200); //opens serial connection to print to console

  gyroSetup();

  mousecam_init();
  
  rover.angle = 0;
  rover.anglePrev = 0;

  robot.begin();

  opticSetup();
  initWifi();
  mqttConnect();
  servoSetup();

  //servoDebug:
  digitalWrite(32, HIGH);
 
  //FPGA side stuff
  // pinMode(PIN_SS,OUTPUT);
  // pinMode(PIN_MISO,INPUT);
  // pinMode(PIN_MOSI,OUTPUT);
  // pinMode(PIN_SCK,OUTPUT);

 

  // SPI.begin();
  // SPI.setClockDivider(SPI_CLOCK_DIV32);
  // SPI.setDataMode(SPI_MODE3);
  // SPI.setBitOrder(MSBFIRST); //Setting up SPI bus

  //setting up rover initials
  rover.X = 0;
  rover.Y = 0; 
  rover.Xdoub = 0;
  rover.Ydoub = 0;
  rover.angle = 0;
}

void loop() {

  client.loop();
  wifi_check();
  
  //FUNCTIONS FOR LOOP
  //roverMovement();
  
  opticMain();
  //digitalWrite(5, HIGH);//disables the optic main after the function (it automatically goes into active mode) 
  //digitalWrite(FPGACS, LOW);
  //Get FPGA input here
  //analyseData("1000000000100000");
  //digitalWrite(FPGACS, HIGH);
  //angleCalc(rover.angle); //Done in opticMain().

  //roverDataTransfer(); //sends the rover's current angle, x and y coordinates.
  
  //radarDetection(rover.X, rover.Y, rover.angle);

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

