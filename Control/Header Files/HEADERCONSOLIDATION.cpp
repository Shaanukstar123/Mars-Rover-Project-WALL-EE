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
#include "connection.h"
#include "OpticSensor.h"

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
#define IN1 12
#define IN2 13
// motor 2 settings
#define IN3 17
#define IN4 16
#define ENB 26
#define CHB 1
const int CCW = 2; // do not change
const int CW  = 1; // do not change
#define motor1 1 // do not change
#define motor2 2 // do not change

//USensor stuff:
#define TRIG_PIN 25 //originally 25 and 26
#define ECHO_PIN 30

//Optic Sensor stuff

Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);

class objectdata
{
public:
bool used = 0;
int x;
int y;
bool sent = 0;
};

objectdata Objects[10];

//RoverObject stuff
class locationdata
{
  public: 
  int X;
  int Y;
  int angle;
  int anglePrev;
  int BatteryPercentage;
  bool fpga_detection;
  bool autoMode;
  bool turning;
  bool detection; //1 if yes, 0 if no
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
    rover.fpga_detection = 1;
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

  else
  {
    rover.fpga_detection = 0;
    
  }
}

void roverMovement()
{

  sub("direction");
  //Serial.println("Subbed to Commands");

  //Serial.println("roverCommand = " + roverCommand);
  if (roverCommand == "{\"directionMove\":\"B\"}") // bwd
  {
   //Serial.println("backwards!");
   robot.rotate(motor1, 30, CCW);
   robot.rotate(motor2, 30, CW);
  }

  if (roverCommand == "{\"directionMove\":\"F\"}") //fwd
  {
   //Serial.println("forwards!");
   robot.rotate(motor1, 30, CW);
   robot.rotate(motor2, 30, CCW);
  }

  if (roverCommand == "{\"directionMove\":\"R\"}") //cw
  {
   //Serial.println("turn Clockwise!");
   robot.rotate(motor1, 30, CW);
   robot.rotate(motor2, 30, CW);
  }

  if (roverCommand == "{\"directionMove\":\"L\"}")  //ccw
  {
   //Serial.println("turning counter clockwise!");
   robot.rotate(motor1, 30, CCW);
   robot.rotate(motor2, 30, CCW);
  }

  if (roverCommand == "{\"directionMove\":\"S\"}") 
  {
   //Serial.println("brake!");
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
  
    if (radianspersec > 0.08 || radianspersec < -0.08){
    double degrees = 0.2*radianspersec*180/3.14159;
    rover.angle = rover.angle - degrees;
    angleConversion();
    }
}

void roverCoordUpdate(int dist)
{
  // rover.X += dist*sin(rover.angle);
  // rover.Y += dist*cos(rover.angle);
  rover.X -= dist*sin(rover.angle*3.14159/180);
  rover.Y -= dist*cos(rover.angle*3.14159/180);
}

void printCoordinates()
{
  Serial.print("X: ");
Serial.println(rover.X);
Serial.print("Y: ");
Serial.println(rover.Y);
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
  // Serial.print(' ');
  // Serial.print((val*100)/351);
  // Serial.print(' ');
  // Serial.print(md.shutter); Serial.print(" (");
  // Serial.print((int)md.dx); Serial.print(',');
  // Serial.print((int)md.dy); Serial.println(')');

  //delay(100);

    //distance_x = convTwosComp(md.dx);
    distance_y = convTwosComp(md.dy);
    // Serial.println("Distance_y : " + String(distance_y));
angleCalc();
roverCoordUpdate(distance_y/45); //modified by /100 to approximate to 1cm per cm moved. //closer possible

printCoordinates();
}

void roverGoto(int x, int y)
{
  Serial.println("Going forwards");
}

void USensorSetup()
{
   // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);
}

bool withinFive(int coordinate1, int coordinate2) //Checks if a coordinate is within 2 of the original
{
  for (int i = -2; i < 3; i++)
  {
    if (coordinate1 == coordinate2)
    {return true;}
  }
  return false;
}
 //creates an array that tracks object location.
 //repeat values rejected by having a 5cm diameter of detection per object.
void mapObjects(int x_coord, int y_coord)
{
  Serial.println("Mapping Object");
  
  for (int i = 0; i < 10; i++) //up to ten objects can be mapped
  {
    if (Objects[i].used == false)
    {
      Serial.println("Success!");
      Objects[i].x = x_coord;
      Objects[i].y = y_coord;
      Objects[i].used = true;
    }
    // && withinFive(x_coord, Objects[i].x)
    //  && withinFive(y_coord, Objects[i].y
  }
}

void sendObjects()
{
  for (int i = 0; i < 10; i++)
  {
    if (Objects[i].sent == false && Objects[i].used == true) //sends used object slots
    {
      String JSON = "{\n\t\"Obstacle\" : \"Unknown\"\n\tX-Coordinate: " 
                    + String(Objects[i].x) + "\n\tY-Coordinate : " 
                    + String(Objects[i].y) + "\n}";
      char* topic = "Obstacles";
      pub(JSON, topic);
      Objects[i].sent = true;
    }
  }
}

void USensorFunction(int &RoverX, int &RoverY, int &RoverAngle, bool FPGA_detect) //send in rover coordinates and angle along with if the fpga has detected anything.
                                                                                  //if yes, nothing will happen. 
{
  float duration_us;
  float distance_cm;
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);

  // calculate the distance
  distance_cm = 0.017 * duration_us;

  // int xlocation = RoverX + distance_cm * sin(RoverAngle);
  // int ylocation = RoverY + distance_cm * cos(RoverAngle);
  int xlocation = RoverX + distance_cm * sin(RoverAngle*3.14159/180);
  int ylocation = RoverY + distance_cm * cos(RoverAngle*3.14159/180);

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  if (xlocation >500 || ylocation > 300 || xlocation < 0 || ylocation < 0)
  {
    Serial.println("Wall found");
    rover.detection = 0;
  }
 //mapObjects(xlocation, ylocation);


  if (distance_cm < 20 && !(xlocation >500 || ylocation > 300 || xlocation < 0 || ylocation < 0))
  {
    rover.detection = 1;
    if (rover.fpga_detection = 0)
    {
       Serial.println("Building detected!");
    char* topic = "Obstacles";
    String JSON = "{\n\t\"Obstacle\" : \"Building\"\n\tX-Coordinate: " 
    + String(xlocation) + "\n\tY-Coordinate : " + String(ylocation) + "\n}";
    //pub(JSON, topic);
    }
    //do nothing if anything else
   
  }

  else 
  {rover.detection = 0;}
  

  Serial.println("X: " + String(xlocation));
  Serial.println("Y: " + String(ylocation));
}

void automaticMode()
{
  sub("Autonomous");
  String mode = roverCommand;
  if(mode == "{\n\"mode\" : 1\n}")
  {
  rover.autoMode = true;
  Serial.println("AutoMode enabled");
  }
  else 
  {
  rover.autoMode = false;
  Serial.println("AutoMode disabled");
  }
}

void roverDataTransfer()
{
    char* topic = "location";
    String JSON = "{\n\txcoord:" + String(rover.X) + ",\n\tycoord : " + String(rover.Y) + 
    ",\n\tobstacle : " + String(rover.detection) + "\n}";
    pub(JSON, topic);
}

void batteryPercent(int example){
    char* topic = "battery";
    String JSON = "{percentage:" + String(example) + "}";
    pub(JSON, topic);
}

void setup() {
  
  mfrc522.PCD_Init();
  Serial.begin(115200); //opens serial connection to print to console

  //gyroSetup();

  //mousecam_init();
  
  rover.angle = 0;
  rover.anglePrev = 0;

  robot.begin();

  //opticSetup();
  initWifi();
  mqttConnect();
  USensorSetup();

//initialising object array
  for (int x = 0; x < 10; x++)
  {
    Objects[x].used = 0;
    Objects[x].sent = 0;
  }

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
  rover.angle = 0;
  rover.fpga_detection = 0;
}

void loop() {
  client.loop();
  wifi_check();

  roverMovement();
  USensorFunction(rover.X, rover.Y, rover.angle, rover.fpga_detection);
  //FUNCTIONS FOR LOOP
  //angleCalc();
  //opticMain();
  
  roverDataTransfer();
  batteryPercent(50);

 
   
  
  
  //analyseData("0000000000100000");
  //angleCalc(rover.angle);
  //printCoordinates();
  // if (loopCount >= 10)
  // {
  
  // Serial.println(rover.angle);
  // loopCount = 0;
  // }
  // loopCount++;

//   ANGLE MEASUREMENT DEBUG
// if (rover.angle != rover.anglePrev)
// {
//   Serial.print(g.gyro.z);
//   Serial.println(" rad/s");
//   Serial.print("Angle: ");
//   Serial.println(rover.angle);
//   Serial.println("");
//   rover.anglePrev = rover.angle;
// }
  
  //automaticMode();
  //sendObjects();

  delay(500);
}

