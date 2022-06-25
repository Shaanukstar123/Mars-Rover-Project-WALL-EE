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
#include <unordered_map>

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

struct Coordinate //storing specific locations requires its own struct
{
  int x;
  int y;
};

std::vector<Coordinate> CoordinateVector;

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
  double Xdoub;
  double Ydoub;
  int angle;
  double angleDoub;
  int anglePrev;
  int BatteryPercentage;
  //FLAGS
  bool fpga_detection;
  bool autoMode;
  bool detection; //1 if yes, 0 if no
  bool walldetection;
  bool avoiding;
  bool coordinateMode;
  //step tracker for avoid/automatic mode
  int avoidStep; //Tracks the steps of avoidance;
  int autoStep;
  

};

//quality of life

bool withinFive(int coordinate1, int coordinate2) //Checks if a coordinate is within radius 2 of the original
{
  for (int i = -2; i < 3; i++)
  {
    if (coordinate1+i== coordinate2)
    {return true;}
  }
  return false;
}

locationdata rover; //create the rover object

//FPGA

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
  char *topic = "aliens";

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
{ if centralCommand["mode"] ==1 {
    int speed = 90;
    //Serial.println(roverCommand);
    if (roverCommand["mode"] == "B") // bwd
    {
    //Serial.println("backwards!");
    robot.rotate(motor1, speed, CCW);
    robot.rotate(motor2, speed-5, CW);
    }

    if (roverCommand["mode"] == "F") //fwd
    {
    //Serial.println("forwards!");
    robot.rotate(motor1, speed, CW);
    robot.rotate(motor2, speed-5, CCW);
    }

    if (roverCommand["mode"] == "R") //cw
    {
    //Serial.println("turn Clockwise!");
    robot.rotate(motor1, speed, CW);
    robot.rotate(motor2, speed-5, CW);
    }

    if (roverCommand["mode"] == "L")  //ccw
    {
    //Serial.println("turning counter clockwise!");
    robot.rotate(motor1, speed, CCW);
    robot.rotate(motor2, speed-5, CCW);
    }

    if (roverCommand["mode"] == "S") 
    {
    //Serial.println("brake!");
    robot.brake(motor1);
    robot.brake(motor2);
    }
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
    if (abs(radianspersec) > 0.08){
    double degrees = 1.13*0.1*radianspersec*180/3.14159;
    rover.angleDoub = rover.angleDoub - degrees;
    rover.angle = rover.angleDoub;
    angleConversion();
    }
}

void roverCoordUpdate(int dist)
{
  rover.Xdoub += dist*sin(rover.angle*3.14159/180);
  rover.Ydoub -= dist*cos(rover.angle*3.14159/180);
  rover.X = rover.Xdoub;
  rover.Y = rover.Ydoub;
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

  delay(50);

    //distance_x = convTwosComp(md.dx);
    distance_y = convTwosComp(md.dy);
    Serial.println("Distance_y : " + String(distance_y));
angleCalc();
roverCoordUpdate(distance_y/35); //modified by /100 to approximate to 1cm per cm moved. //closer possible
printCoordinates();
}

int differenceFunction(int angleA, int angleB)
{
  int difference;
  if (angleA > angleB)
  {difference = angleA - angleB;}

  if (angleA < angleB)
  {difference = angleB - angleA;}

  if(difference > 180)
  {difference = abs(difference-360);}

  return difference;
}
  

int angleconvert(int argument)
{
  if (argument < 0)
  {
    argument += 360;
  }
  return argument;
}

void roverTurn(int angle){

    Serial.println("HERE!");
    int diff= differenceFunction(rover.angle, angle);
    int speed;
    
    if (diff > 90)
    {speed = 50;}
    if (diff < 90)
    {speed = 40;}
    if (diff < 45)
    {speed = 30;}
    if (diff < 30)
    {speed = 20;}
    else {diff = 20;}
    
    int start = rover.angle;
    int target = angle;

    //start < 180
    if (target!= start)
    {

    if (start < 180 && (target > start + 180 || target < start))
    {
      Serial.println("Left");
      robot.rotate(motor1, speed, CCW);
      robot.rotate(motor2, speed, CCW); 
    }
    if (start <= 180 && (target > start) && (target < start + 180))
    {
      Serial.println("Right");
      robot.rotate(motor1, speed, CW);
      robot.rotate(motor2, speed, CW);
      // Serial.println("Left");
      // robot.rotate(motor1, speed, CCW);
      // robot.rotate(motor2, speed, CCW);
    }
    if (start >= 180 && ((target < start - 180) || target > start))
    {
      
      Serial.println("Right");
      robot.rotate(motor1, speed, CW);
      robot.rotate(motor2, speed, CW);
    }
    if (start = 180 && ((target > start - 180) && (target < start)))
    {
      Serial.println("Left");
      robot.rotate(motor1, speed, CCW);
      robot.rotate(motor2, speed, CCW);
     
    }
    
    }
   
    
}


void roverGoto(int x, int y){
  
    if(!withinFive(rover.X, x) || !withinFive(rover.Y, y)){
      int angle;
      
        if ((y - rover.Y )!= 0){ 
        double ratio = (double(x-rover.X))/(y-rover.Y);
        double ATAN = (-atan(ratio))*180/3.14159;
        angle = ATAN;
        }

      else if((y - rover.Y )== 0){
        if (x-rover.X > 0)  {angle = -90;}
        if (x-rover.X < 0)  {angle = 90;}
        
      }
      
      
      
      if (x-rover.X < 0 && y - rover.Y < 0)
        {
          angle+=180;
        }
      if (y-rover.Y < 0 && x-rover.X > 0)
      {
        angle+= 180;
      }
      if (angle < 0)
      {
        angle += 360;
      }
      Serial.print("angle=");
      Serial.println(angle);
      Serial.print("rover.angle=");
      Serial.println(rover.angle);
      roverTurn(angle);
      
      // Rover starts going forwards
      if (rover.angle == angle)
      {
        robot.rotate(motor1, 30, CW); 
        robot.rotate(motor2, 30, CCW);
      }
      
  }

    if(withinFive(rover.X, x) && withinFive(rover.Y, y))
  {
    robot.brake(1);
    robot.brake(2);
  }


}


void USensorSetup(){
  pinMode(TRIG_PIN, OUTPUT);     // configure the trigger pin to output mode
  pinMode(ECHO_PIN, INPUT);      // configure the echo pin to input mode
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
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = 0.017 * duration_us;

  int xlocation = RoverX - distance_cm * sin(RoverAngle*3.14159/180); //minus due to how the coords work
  int ylocation = RoverY + distance_cm * cos(RoverAngle*3.14159/180);

  rover.walldetection = 0;

  Serial.println(distance_cm);

  if (xlocation >500 || ylocation > 300 || xlocation < 0 || ylocation < 0)
  {
    Serial.println("Wall found");
    rover.detection = 0;
    rover.walldetection = 1;
  }

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
  String mode = centralCommand["mode"];
  if(centralCommand["mode"] == 3){
    {rover.autoMode = true;
    Serial.println("AutoMode enabled");}
  }
  else{ 
    {rover.autoMode = false;
    Serial.println("AutoMode disabled");}
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

void halt() //stops the rover
  {robot.brake(1);
  robot.brake(2);}

int sweep(int step)
{
  if (step != 0)
  {
  int x = 10;
  int y = 10;

  if (step % 2 == 0)  {y += 10*(step);} //check if even step. y target increases.

  else {y = rover.Y;}

  if (step % 2 == 1){
    if (step-1 % 4 == 0)  {x += 280;}}

  else {x = rover.X;}

  if (y > 200)  {step = 0;}

  if (step != 0)
  {roverGoto(x, y);}

  if (withinFive(x, rover.X) && withinFive(y, rover.Y))
  {
    step++;
  }
  if (y > 200) {step = 0;
  robot.brake(1); 
  robot.brake(2);} //stops the sweep mode.
  }
  return step;

}

void deleteElements() //Deletes an element if the rover is within range of it.
{
  for (int i = 0; i < CoordinateVector.size(); i++)
  {
    if (withinFive(rover.X, CoordinateVector[i].x) && withinFive(rover.Y,CoordinateVector[i].y))
    {
      CoordinateVector.erase(CoordinateVector.begin()+i);
    }
  }
}

//searches the recieved coordinates for the one closest to the rover presently
int returnClosestElement() //rover.x and rover.y
{
  int currentDistance;
  int smallestElement;
  if (CoordinateVector.size() != 0)
  {
  for (int i = 0; i < CoordinateVector.size(); i++)
  {
    int compareDistance = sqrt((rover.X - CoordinateVector[i].x)^2 + (rover.Y-CoordinateVector[i].y)^2);
    if (i == 0)
    {
      currentDistance = compareDistance;
      smallestElement = 0;
    }
    else if (currentDistance < compareDistance)
    {
      currentDistance = compareDistance;
      smallestElement = i;
    }
  }
  }
  return smallestElement;
}

void getCoordinates() //Grabs coordinates from the MQTT server and adds them to the vector
{
  //anatomy of this is {"coords":"+030-0201"} 
  /*String command = "";//2345678901234567890
  /for (int i = 2; i < 8; i++){
    command += roverCommand[i];
  }
  Serial.println(command);*/
  if (centralCommand["mode"] == 2)
  {
    Serial.println("Recieved Command");
    //String xcoords = "";
    //String ycoords = "";
    int x;
    int y;
    /*for (int i = 12; i < 15; i++)
    {
      xcoords+=roverCommand[i];
    }*/
    //Serial.println(xcoords);
    /*for (int i = 16; i < 19; i++)
    {
      ycoords+=roverCommand[i];
    }*/
    //Serial.println(ycoords);
    if (centralCommand["mode"] == 2)
    {
      rover.coordinateMode = 1;
    }
    //Serial.println(roverCommand[20] );
    if (centralCommand["mode"] !=2)
    {
      rover.coordinateMode = 0;
    }

    x = coordinates["xcoord"];//(int(xcoords[0])*100) + (int(xcoords[1])*10) + int(xcoords[2]);
    y = coordinates["ycoord"];//(int(ycoords[0])*100) + (int(ycoords[1])*10) + int(ycoords[2]);
    Coordinate newCoord;
    //if (roverCommand[11] == '-')
    //{x = -x;}
    //if (roverCommand[15] == '-')
    //{y = -y;}
    newCoord.x = x;
    newCoord.y = y;

    CoordinateVector.push_back(newCoord);
  }
}

int next = 0;
int targetX;
int targetY;
int targetAngle;

void roverAvoid()
{
Serial.println("Targets: ");
Serial.println(targetX);
Serial.println(targetY);

  if (rover.avoidStep == 1)
  {
    targetX = rover.X + 5* sin(3.14159*(rover.angle+90)/180);
    targetY = rover.Y + 5* cos(3.14159*(rover.angle+90)/180);
    rover.avoidStep++;
  }
  else if (rover.avoidStep == 2)
  {
    roverGoto(targetX, targetY);
    if (withinFive(rover.X, targetX) && withinFive(rover.Y, targetY))
    {rover.avoidStep++;}
  }
  else if (rover.avoidStep == 3)
  {
    targetX = rover.X + 5* sin(3.14159*(rover.angle-90)/180);
    targetY = rover.Y + 5* cos(3.14159*(rover.angle-90)/180);
    rover.avoidStep++;
  }
  else if (rover.avoidStep == 4)
  {
    roverGoto(targetX, targetY);
    if (withinFive(rover.X, targetX) && withinFive(rover.Y, targetY))
    {rover.avoidStep++;}
  }
  else if (rover.avoidStep == 5)
  {
    targetX = rover.X + 5* sin(3.14159*(rover.angle-90)/180);
    targetY = rover.Y + 5* cos(3.14159*(rover.angle-90)/180);
    rover.avoidStep++;
  }
  else if (rover.avoidStep == 6)
  {
    roverGoto(targetX, targetY);
    if (withinFive(rover.X, targetX) && withinFive(rover.Y, targetY))
    {rover.avoidStep = 0;
    rover.avoiding = 0;
    halt();}
  }
}

void setup() {
   Serial.begin(115200);
   //CONNECTION SETUP
  mfrc522.PCD_Init();
  initWifi();
  mqttConnect();
  sub("#");
    
  //COMPONENT SETUP
  mousecam_init(); //OPTIC SENSOR
  opticSetup();
  gyroSetup();
  robot.begin();
  USensorSetup();
  
  //FPGA SETUP STUFF
  // pinMode(PIN_SS,OUTPUT);
  // pinMode(PIN_MISO,INPUT);
  // pinMode(PIN_MOSI,OUTPUT);
  // pinMode(PIN_SCK,OUTPUT);
  // SPI.begin();
  // SPI.setClockDivider(SPI_CLOCK_DIV32);
  // SPI.setDataMode(SPI_MODE0);
  // SPI.setBitOrder(MSBFIRST); //Setting up SPI bus

  //setting up rover initials
  rover.X = 0;
  rover.Y = 0; 
  rover.angle = 0;
  rover.fpga_detection = 0;
  rover.autoMode = true;
  rover.detection = 1;


  //DEBUG FOR COORD MODE:
  // Coordinate ega;
  // ega.x = 10;
  // ega.y = 10;
  // CoordinateVector.push_back(ega);
  // Coordinate egb;
  // egb.x = -10;
  // egb.y = -10;
  // CoordinateVector.push_back(egb);
}


void detect() //All the operations of the rover to do with information, detection.
{
      // angleCalc();
      // digitalWrite(PIN_SS_FPGA, LOW); //activate FPGA
      // digitalWrite(PIN_MOUSECAM_CS, HIGH); //disable the mousecam
      // SPI.setDataMode(SPI_MODE0) //for fpga
      // delay(1); //tiny delay to let everything settle
      // recievedData = SPI.transfer16(0xff); //Get data from the FPGA
      // analyseData(recievedData);
      // digitalWrite(PIN_SS_FPGA, HIGH);
      // SPI.setDataMode(SPI_MODE3) //for optic sensor
      // digitalWrite(PIN_MOUSECAM_CS, LOW); //activate the mousecam
      // delay(1);
      // opticMain();
      // radarDetection(rover.X, rover.Y, rover.angle);
}

int loopcount = 0;
int stepchecker = 0; //DEBUG FOR ROVERGOTO SEQUENCES
void loop() {
  client.loop();
  wifi_check();
  // roverMovement(); //Wifi connection dependent
  //roverDatatransfaer
 
  
  opticMain();
  USensorFunction(rover.X, rover.Y, rover.angle, rover.fpga_detection);
  //radarDetection(rover.X, rover.Y, rover.angle);

  //Coordinate Mode
  // if (CoordinateVector.size() != 0)
  // roverGoto(CoordinateVector[returnClosestElement()].x, CoordinateVector[returnClosestElement()].y);
  // deleteElements();

  // if (rover.detection && !rover.avoiding) //Replace with Joshua's later
  // {
  //   rover.avoiding = 1;
  //   rover.avoidStep = 1;
  // }

  // if (rover.avoiding)
  // {
  //   roverAvoid();
  // }

  getCoordinates();

  if (rover.coordinateMode == 1)
  {
    if (CoordinateVector.size() != 0)
    {roverGoto(CoordinateVector[returnClosestElement()].x, CoordinateVector[returnClosestElement()].y);}
    deleteElements();
    if (CoordinateVector.size() == 0) {rover.coordinateMode = 0;}
  }

  

  //
// //automaticMode(); //Checks for the automode being activated
  // //detect();
  //analyseData("0000000000100000");
  //angleCalc(rover.angle);
  //printCoordinates();
  //sendObjects();

  delay(50);
}

