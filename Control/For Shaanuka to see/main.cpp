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
#include "radar.h"

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
#define TRIG_PIN 2 //originally 25 and 26
#define ECHO_PIN 15

//define the walls here
#define RIGHTWALL 300
#define LEFTWALL 0
#define TOPWALL 200
#define BOTTOMWALL 0

//Optic Sensor stuff
class Alien
{
  public:
int x;
int y;
String colour;
};

Alien currentAlien;



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
  bool detection; //1 if yes, 0 if no
  bool walldetection;
  int targetX;
  int targetY;
  bool targetCoord;
};

locationdata rover; //create the rover object

void approximateLocation(int distance, int angle, String colour, int roverX, int roverY, int roverAngle)
{
  int angle_actual = roverAngle - angle;

  double xdisplacement = distance * sin(angle_actual*3.14159/180);
  double ydisplacement = distance * cos(angle_actual*3.14159/180);

  int alienX = roverX + xdisplacement;
  int alienY = roverY + ydisplacement;

  currentAlien.x = alienX;
  currentAlien.y = alienY;
  currentAlien.colour = colour;


  String JSON = "{\n\tcolour : \"" + colour + "\"\n\tX-Coordinate : " + String(alienX) + "\n\tY-Coordinate : " + String(alienY) + "\n}";
  char topic[] = "aliens";

  pub(JSON, topic);
}


void analyseData(String x)
{
  //int example = SPI.transfer16(0xff); //SPI input disabled for now
  //convert to binary
  //recievedData = toBinary(example);
  
  String alienColour = "";
  //Identification
  String alienBin; //alienBinary
  for (int y = 0; y < 3; y++) //extracting the colour data
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

  String distanceBin;
  
  for (int y = 3; y < 11; y++) {distanceBin += x[y];} //extracting the distance
    
    Serial.println(distanceBin);

    int distance = toInteger(distanceBin);

    Serial.print("Approximate distance from Rover : ");
    Serial.print(distance);
    Serial.println(" centimeters.");

  String angleBin;

  for (int y = 11; y < 16; y++) {angleBin += x[y];} //extracting angle data
  
  int angle = toInteger(angleBin);

    Serial.print("Approximate angle from Rover : ");
    Serial.print(angle);
    Serial.println(" degrees.");

    approximateLocation(distance, angle, alienColour, rover.X, rover.Y, rover.angle);
  }

  else  {rover.fpga_detection = 0;}
}

void roverMovement() //DONE
{
  sub("direction");
  //Serial.println("Subbed to Commands");
  //Serial.println("roverCommand = " + roverCommand);
  if (roverCommand == "{\"directionMove\":\"B\"}") // bwd
  {
   //Serial.println("backwards!");
   robot.rotate(motor1, 30, CCW);
   robot.rotate(motor2, 25, CW);
  }

  if (roverCommand == "{\"directionMove\":\"F\"}") //fwd
  {
   //Serial.println("forwards!");
   robot.rotate(motor1, 30, CW);
   robot.rotate(motor2, 25, CCW);
  }

  if (roverCommand == "{\"directionMove\":\"R\"}") //cw
  {
   //Serial.println("turn Clockwise!");
   robot.rotate(motor1, 30, CW);
   robot.rotate(motor2, 25, CW);
  }

  if (roverCommand == "{\"directionMove\":\"L\"}")  //ccw
  {
   //Serial.println("turning counter clockwise!");
   robot.rotate(motor1, 30, CCW);
   robot.rotate(motor2, 25, CCW);
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

void angleConversion() { //Convert angles to 360 degree format
  if (rover.angle < 0)
  {rover.angle+=360;}

  if (rover.angle >= 360)
  {rover.angle-=360;}  }

void angleCalc(){ //Calculate the current angle using the data from the gyroscope.
  mpu.getEvent(&acc, &g, &temp);
  double radianspersec = g.gyro.z;
  
    if (abs(radianspersec) > 0.1){
    double degrees = 0.5*radianspersec*180/3.14159;
    rover.angle = rover.angle - degrees;
    angleConversion();
    }
}

void roverCoordUpdate(int dist)
{
  rover.X -= dist*sin(rover.angle*3.14159/180); //X is right to left
  rover.Y -= dist*cos(rover.angle*3.14159/180); //Y is measured backwards because of the optic sensor
}

void printCoordinates() //Debug function
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
  Serial.print(' ');
  Serial.print((val*100)/351);
  Serial.print(' ');
  Serial.print(md.shutter); Serial.print(" (");
  Serial.print((int)md.dx); Serial.print(',');
  Serial.print((int)md.dy); Serial.println(')');

  delay(100);

    //distance_x = convTwosComp(md.dx);
    distance_y = convTwosComp(md.dy);
    Serial.println("Distance_y : " + String(distance_y));
angleCalc();
roverCoordUpdate(distance_y/45); //modified by /100 to approximate to 1cm per cm moved. //closer possible

printCoordinates();
}

void roverTurn(int angle)
{
  while (rover.angle != angle)
  {
  if (angle - rover.angle < 180) //RIGHT
  {
    robot.rotate(motor1, 30, CW);
    robot.rotate(motor2, 30, CW);
  }
  if (angle - rover.angle > 180) //LEFT
  {
    robot.rotate(motor1, 30, CCW);
    robot.rotate(motor2, 30, CCW);
  }
  }

    robot.brake(motor1);
    robot.brake(motor2);


}

void roverGoto(int x, int y)
{
  int angle;
  angle = -atan((x-rover.X)/(y-rover.Y));
  angleConversion(angle);
  roverTurn(angle);
  //Rover starts going forwards
  if (rover.angle == angle)
  {
  robot.rotate(motor1, 30, CW);
  robot.rotate(motor2, 30, CCW);
  }
}

void USensorSetup(){
  pinMode(TRIG_PIN, OUTPUT);     // configure the trigger pin to output mode
  pinMode(ECHO_PIN, INPUT);      // configure the echo pin to input mode
}

bool withinFive(int coordinate1, int coordinate2) //Checks if a coordinate is within radius 2 of the original
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

  int xlocation = RoverX - distance_cm * sin(RoverAngle*3.14159/180); //minus due to how the coords work
  int ylocation = RoverY + distance_cm * cos(RoverAngle*3.14159/180);

  // print the value to Serial Monitor (DEBUG)
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

    rover.walldetection = 0;

  if (xlocation >500 || ylocation > 300 || xlocation < 0 || ylocation < 0)
  {
    Serial.println("Wall found");
    rover.detection = 0;
    rover.walldetection = 1;
  }
 //mapObjects(xlocation, ylocation);


  if (distance_cm < 20 && !(xlocation >500 || ylocation > 300 || xlocation < 0 || ylocation < 0))
  {
    rover.detection = 1;
    if (rover.fpga_detection = 0){
       Serial.println("Building detected!");
    char* topic = "Obstacles";
    String JSON = "{\n\t\"Obstacle\" : \"Building\"\n\tX-Coordinate: " 
    + String(xlocation) + "\n\tY-Coordinate : " + String(ylocation) + "\n}";
    pub(JSON, topic);
    }
    
    //For when there is an alien detection but the alien is not at the same position as the building is.
    //the alien must not be within a radius of 2 cm of the building. Accuracy with FPGA needs to be good.
    //False inputs from the ultrasonic may occur.
    if (rover.fpga_detection == true && !(withinFive(xlocation, currentAlien.x)) && !(withinFive(ylocation, currentAlien.y)))
    { 
    Serial.println("Building detected!");
    char* topic = "Obstacles";
    String JSON = "{\n\t\"Obstacle\" : \"Building\"\n\tX-Coordinate: " 
    + String(xlocation) + "\n\tY-Coordinate : " + String(ylocation) + "\n}";
    pub(JSON, topic);
    }
  }

  else 
  {rover.detection = 0;}
  
  //Degug, X and Y of the object
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

void getTarget() //get Target and decode
{
  sub("Target"); //test format: "{t:1,x:010,y:010}"
  if (roverCommand[4] == '1'){
    String xstring = "";
    String ystring = "";

    for (int i = 8; i < 11; i++){
      xstring+=roverCommand[i];
      ystring+=roverCommand[i+6];
      }

    int xcoord;
    int ycoord;
  
    for (int j = 0; j < 3; j++) { //Decimal conversion
      xcoord = int(xstring[0])*pow(10, 2-j);
      ycoord = int(ystring[0])*pow(10, 2-j);
      }
    rover.targetX = xcoord;
    rover.targetY = ycoord;
    rover.targetCoord = 1;
    // DEBUG
    // Serial.println(xcoord);
    // Serial.println(ycoord);
  }
}

void halt() //stops the rover
{
      robot.brake(1);
      robot.brake(2);
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
  //USensorSetup();

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
  rover.autoMode = false;
}

int Lavoid;
int Ravoid;

void LavoidSequence()
{
    int targetangle;
    int targetx;
    int targety;
 
    switch (Lavoid)
    {
    case 0: break; //skip

    case 1: 
        targetangle = rover.angle -90;
        Lavoid++;
    case 2:
        robot.rotate(motor1, 30, CCW); //Left
        robot.rotate(motor2, 30, CCW);
        if (rover.angle <= targetangle){
        halt();
        Lavoid++;}
    case 3:
        targetx = rover.X + 20*sin(rover.angle);
        targety = rover.Y + 20*cos(rover.angle);
        Lavoid++;
    case 4:
        robot.rotate(motor1, 30, CW);
        robot.rotate(motor2, 30, CCW);
        if (withinFive(rover.X, targetx) && withinFive(rover.Y, targety))
        {halt(); Lavoid++;}
    case 5:
        targetangle = rover.angle +90;
        Lavoid++;
    case 6: 
        robot.rotate(motor1, 30, CW); //Right
        robot.rotate(motor2, 30, CW);
        if (rover.angle >= targetangle)
        {
        halt();
        Lavoid++;}
    case 7:
        targetx = rover.X + 30*sin(rover.angle);
        targety = rover.Y + 30*cos(rover.angle);
        Lavoid++;
    case 8:
        robot.rotate(motor1, 30, CW);
        robot.rotate(motor2, 30, CCW); //FWD
        if (withinFive(rover.X, targetx) && withinFive(rover.Y, targety))
        {halt(); Lavoid++;
        targetangle = rover.angle +90;}
    case 9: 
        robot.rotate(motor1, 30, CW); //Right
        robot.rotate(motor2, 30, CW);
        if (rover.angle >= targetangle){
            halt();
            Lavoid++;
        }
    case 10:
        targetx = rover.X + 30*sin(rover.angle);
        targety = rover.Y + 30*cos(rover.angle);
        Lavoid++;
    case 11:
        robot.rotate(motor1, 30, CW);
        robot.rotate(motor2, 30, CCW); //FWD
        if (withinFive(rover.X, targetx) && withinFive(rover.Y, targety))
        {halt(); Lavoid++;}
    case 12:
    robot.rotate(motor1, 30, CCW); //Left
    robot.rotate(motor2, 30, CCW);
    if (rover.angle <= targetangle){
        halt();
        Lavoid = 0;}

    }
}

// void detect() //All the operations of the rover to do with information, detection.
// {
//     angleCalc();
//     digitalWrite(PIN_SS_FPGA, LOW); //activate FPGA
//     digitalWrite(PIN_MOUSECAM_CS, HIGH); //disable the mousecam
//     delay(1); //tiny delay to let everything settle
//     recievedData = SPI.transfer16(0xff); //Get data from the FPGA
//     analyseData(recievedData);
//     digitalWrite(PIN_SS_FPGA, HIGH); 
//     digitalWrite(PIN_MOUSECAM_CS, LOW); //activate the mousecam
//     delay(1);
//     opticMain();
//     radarDetection(rover.X, rover.Y, rover.angle);
// }

void loop() {
  client.loop();
  wifi_check();
  //automaticMode(); //Checks for the automode being activated

  //detect();
  roverMovement();
  opticMain();
  radarDetection(rover.X, rover.Y, rover.angle);

    if (rover.autoMode)
    {
      if (!rover.detection && Lavoid == 0)
      {
        //fwd
      }
      if (rover.detection)
      {
        Lavoid = 1;
      }

      if (rover.walldetection)
      {
        if (rover.X + 30 > 300)
        {
            //left Uturn
        }

        if (rover.X - 30 < 0)
        {
            //right Uturn
        }
      }

    }

    LavoidSequence(); // will always skip unless not 0. Then it will start its processes.


    
  
  
  
  //analyseData("0000000000100000");
  //angleCalc(rover.angle);
  //printCoordinates();
  
  
  //automaticMode();
  //sendObjects();

  delay(10);
}

