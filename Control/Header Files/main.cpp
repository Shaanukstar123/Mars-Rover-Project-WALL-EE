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
//#include "radar.h"

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

#define PIN_SS_FPGA 4
char buffer[256];
char alien_buffer [256];
char battery_buffer [256];

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
std::vector<Coordinate> CoordinateSequence;

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
  int totalTime;
  //FLAGS
  bool fpga_detection;
 
  bool detection; //1 if yes, 0 if no
  bool walldetection;
  bool avoiding;
  bool turning; //flag for the rover turning. Used to diable detection of obstacles

  //Control modes:
  bool remoteControl;
  bool coordinateMode;
  bool autoMode;

  //step tracker for avoid/automatic mode
  int avoidStep; //Tracks the steps of avoidance;
  int autoStep;
  bool sthdetection;

};

//quality of life

void getbattery() {
  //std::cout << "Run-time in seconds: ";
  //std::cin >> rt;
 
 
  // full run-time of rover = (6000 / current usage per hour), i.e. 6000mAh/bmAh
  // battery can provide b mAh for 6000/b hours i.e. b mAh for (6000/b)/(60^2) seconds
  // hence, after the rover has run for (6000/b)/(60^2), the battery is depleted
  // rover uses b/(3600) mA per second
  double  seconds = millis()/1000.0;
  double tmp = 6000.0/120; //Time battery can run for in hours
  tmp = (6000/120)*60*60; //Time battery can run for in seconds
  int batteryCharge = int(((tmp - seconds)/tmp)*100); //Amount of time battery can still run for divided by the total time it can run for
  battery["percentage"] = batteryCharge;
}

int angleConvert(int angle)
{
  if (angle < 0)
  {
    angle += 360;
  }
  if (angle >= 360)
  {
    angle -= 360;
  }
  return angle;
}

bool withinFive(int coordinate1, int coordinate2) //Checks if a coordinate is within radius 2 of the original
{
  for (int i = -3; i < 4; i++)
  {
    if (angleConvert(coordinate1+i) == angleConvert(coordinate2))
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

  aliens["xcoord"] = alienX;
  aliens["ycoord"] = alienY;
  aliens["color"] = colour;

  char* topic = "aliens";
  serializeJson(aliens,alien_buffer);

  pub(alien_buffer, topic);
}


int analyseData(String x)
{
  //Serial.println(recievedData);
  //Identification
  String colour;
  bool detectionMade = false;
  String alienBin;
  int distance = 0;
  for (int y = 0; y < 3; y++) //extracting the distance
    {alienBin += x[y];}

  if (alienBin != "000")
  {
    detectionMade == true;
    if (alienBin == "001")
    {
      Serial.println("Red Alien detected.");
      colour = "Red";
    }

    if (alienBin == "010")
    {
      //green
      Serial.println("Green Alien detected.");
      colour = "Green";
    }

    if (alienBin == "011")
    {
      //blue
      Serial.println("Blue Alien detected.");
      colour = "Blue";
    }

    if (alienBin == "100")
    {
      //orange 
      Serial.println("Yellow Alien detected.");
      colour = "Yellow";
    }

    if (alienBin == "101")
    {
      //pink
      Serial.println("Pink Alien detected.");
      colour = "Pink";
    }

    if (alienBin == "110")
    {
      //grey
      Serial.println("Light Green Alien detected.");
      colour = "Light Green";
    }
    if (alienBin == "111")
    {
      //building
      Serial.println("Building detected.");
      colour = "building";
    }
    //Distance
    String distanceBin = "";
  
  for (int y = 3; y < 11; y++) { //extracting the distance
      distanceBin += x[y];
    }
    distanceBin += "0"; //Needs a trailing zero as the data from the FPGA removes the LSB
    Serial.println(distanceBin);
    int pixelWidth = toInteger(distanceBin);
    distance = int(distanceCalc(pixelWidth));
    Serial.print("Approximate distance from Rover : ");
    Serial.print(distance);
    Serial.println(" centimeters.");
    //angle

  String angleBin;

  for (int y = 11; y < 16; y++) //extracting the angle
    {angleBin += x[y];}
  
  int angleRaw = toInteger(angleBin);
  int angle = (angleRaw*2.2)-30;

 if (angleRaw == 0) {
  Serial.print("Erroneous Angle data!");
 } else {
    Serial.print("Approximate angle from Rover : ");
    Serial.print(angle);
    Serial.println(" degrees.");
 }
  

  }

  //Distance

  // String distanceBin;

  // for (int y = 3; y < 11; y++) //extracting the distance
  //   {distanceBin += x[y];}

  // int distance = toInteger(distanceBin);

  // if (detectionMade)
  // {
  //   Serial.print("Approximate distance from Rover : ");
  //   Serial.println(distance + " centimeters.");
  // }

  String angleBin;

  for (int y = 11; y < 16; y++) //extracting the angle
    {angleBin += x[y];}
  
  int angle = toInteger(angleBin);
  angle = (70*angle)/32;
  if (detectionMade)
  {
    Serial.print("Approximate angle from Rover : ");
    Serial.println(angle + " degrees.");
  }
  approximateLocation(distance, angle, colour, rover.X, rover.Y, rover.angle);
  return distance;

}

void roverMovement() //DONE
{

    int speed = 50;
    //Serial.println(roverCommand);
    if (rControl["directionMove"] == "F") // bwd
    {
    //Serial.println("backwards!");
    rover.turning = 0;
    robot.rotate(motor1, speed, CCW);
    robot.rotate(motor2, speed, CW);
    }

    if (rControl["directionMove"] == "B") //fwd
    {
    //Serial.println("forwards!");
    rover.turning = 0;
    robot.rotate(motor1, speed, CW);
    robot.rotate(motor2, speed, CCW);
    }

    if (rControl["directionMove"] == "R") //cw
    {
    //Serial.println("turn Clockwise!");
    rover.turning = 1;
    robot.rotate(motor1, speed, CW);
    robot.rotate(motor2, speed, CW);
    }

    if (rControl["directionMove"] == "L")  //ccw
    {
    //Serial.println("turning counter clockwise!");
    rover.turning = 1;
    robot.rotate(motor1, speed, CCW);
    robot.rotate(motor2, speed, CCW);
    }

    if (rControl["directionMove"] == "S") 
    {
    //Serial.println("brake!");
    robot.brake(motor1);
    robot.brake(motor2);
    }
}

uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;
String recievedData;
byte incomingByte = 0;

const int buzzer = 25;
const int TONE_PWM_CHANNEL = 0; 
long startTime;
int lastUsed;
SPISettings settings(128000, MSBFIRST, SPI_MODE1);
//For each obstacle

int grabBallData(byte ballCode) {
  int distance = 0;
  //Ask for the two packets ball data
  SPI.beginTransaction(settings);
  digitalWrite(PIN_SS_FPGA, LOW);
  byte syncPacket = SPI.transfer(0b10101010); //Packet required for sync
  byte packet1 = SPI.transfer(ballCode);
  //SPI.endTransaction();
  //digitalWrite(PIN_SS, HIGH);
  //SPI.beginTransaction(settings);
  //digitalWrite(PIN_SS, LOW);
  byte packet2 = SPI.transfer(ballCode+1);
  digitalWrite(PIN_SS_FPGA, HIGH);
  SPI.endTransaction();
  //convert to binary
  int dataIn = (packet1 << 8) + (packet2);
  if (dataIn != 0 && dataIn != 65535) {
    recievedData = toBinary(dataIn);
    Serial.print(ballCode);
    Serial.print(" : ");
    Serial.println(recievedData);
    distance = analyseData(recievedData);
  }
  return distance;
}


//OPTIC SENSOR STUFF

char asciiart(int k)
{
  static char foo[] = "WX86*3I>!;~:,`. ";
  return foo[k>>4];
}

byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];



void angleConversion() { //Convert angles to 360 degree format
  while (rover.angle < 0)
  {rover.angle+=360;}
  while (rover.angle >= 360)
  {rover.angle =rover.angle - 360;}  }


double previoustime;

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

void angleCalc(){
  mpu.getEvent(&acc, &g, &temp);
  double radianspersec = g.gyro.z;
  double current = millis();
  double difference = current-previoustime;

  if (radianspersec > 0.1 || radianspersec < -0.1){
    double degrees = difference*radianspersec*180/3141.9;
    rover.angleDoub = rover.angleDoub - degrees;
    rover.angle = rover.angleDoub;
    angleConversion();
    }
  // if (abs(differenceFunction(rover.angle, rover.anglePrev)) > 30)
  // {
  //     rover.angle = rover.anglePrev;
  // }
  rover.anglePrev = rover.angle;
  previoustime = current;
}

void roverCoordUpdate(int dist)
{
  rover.Xdoub += dist*sin(rover.angle*3.14159/180);
  rover.Ydoub += dist*cos(rover.angle*3.14159/180);
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
  // Serial.print(' ');
  // Serial.print((val*100)/351);
  // Serial.print(' ');
  // Serial.print(md.shutter); Serial.print(" (");
  // Serial.print((int)md.dx); Serial.print(',');
  // Serial.print((int)md.dy); Serial.println(')');

  delay(20);

    //distance_x = convTwosComp(md.dx);
    distance_y = convTwosComp(md.dy);
    // Serial.println("Distance_y : " + String(distance_y));
angleCalc();
roverCoordUpdate(distance_y/35); //modified by /100 to approximate to 1cm per cm moved. //closer possible
// printCoordinates();
}


void roverTurn(int angle){

    int diff= differenceFunction(rover.angle, angle);
    int speed = 35;
    
    // if (diff > 90)
    // {speed = 50;}
    // if (diff < 90)
    // {speed = 50;}
    // if (diff < 45)
    // {speed = 40;}
    // if (diff < 30)
    // {speed = 30;}
    // else {diff = 20;}
    
    int start = rover.angle;
    int target = angle;

    //start < 180
    //if (target!= start)
    if (!withinFive(target, start))
    {
      rover.turning = 1;

    if (start <= 180 && (target > start + 180 || target < start))
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
    }
    if (start >= 180 && ((target < start - 180) || target > start))
    {
      
      Serial.println("Right");
      robot.rotate(motor1, speed, CW);
      robot.rotate(motor2, speed, CW);
    }
    if (start >= 180 && ((target > start - 180) && (target < start)))
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
        double ATAN = (atan(ratio))*180/3.14159;
        angle = ATAN;
        }

      else if((y - rover.Y )== 0){
        if (x-rover.X > 0)  {angle = 90;}
        if (x-rover.X < 0)  {angle = -90;}  
      }

      Serial.println("YTarget" + String(y));
      Serial.println("XTarget" + String(x));

      // if (y-rover.Y > 0 && x - rover.X > 0)
      // {
      //   Serial.println("+270");
      //   angle+=270;
      // }
     
       if (x-rover.X < 0 && y - rover.Y < 0)
        {
          Serial.println("+90");
          angle+=90;
        }
      else if (y-rover.Y < 0 && x-rover.X > 0)
      {
        Serial.println("+180");
        angle+=180;
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

      int distance = sqrt((rover.X-x)^2 + (rover.Y-y)^2);
      int threshold = 5;

      //if (rover.angle == angle)
      if(withinFive(rover.angle, angle))
      {
        rover.turning = 0;
        robot.rotate(motor1, 45, CCW); 
        robot.rotate(motor2, 45, CW);
      }
      // Rover starts going forwards
      //if (rover.angle == angle)
  }

    if(withinFive(rover.X, x) && withinFive(rover.Y, y))
  {
    robot.brake(1);
    robot.brake(2);
    rover.turning = 0;
  }


}
void USensorSetup(){
  pinMode(TRIG_PIN, OUTPUT);     // configure the trigger pin to output mode
  pinMode(ECHO_PIN, INPUT);      // configure the echo pin to input mode
}
 //creates an array that tracks object location.
 //repeat values rejected by having a 5cm diameter of detection per object.
// void mapObjects(int x_coord, int y_coord)
// {
//   Serial.println("Mapping Object");
  
//   for (int i = 0; i < 10; i++) //up to ten objects can be mapped
//   {
//     if (Objects[i].used == false)
//     {
//       Serial.println("Success!");
//       Objects[i].x = x_coord;
//       Objects[i].y = y_coord;
//       Objects[i].used = true;
//     }
//     // && withinFive(x_coord, Objects[i].x)
//     //  && withinFive(y_coord, Objects[i].y
//   }
// }

void sendObjects()
{
  for (int i = 0; i < 10; i++)
  {
    if (Objects[i].sent == false && Objects[i].used == true) //sends used object slots
    {
      location["obstacle"] = 1;
      Serial.println("Object detected and sent");
      // String JSON = "{\n\t\"Obstacle\" : \"Unknown\"\n\tX-Coordinate: " 
      //               + String(Objects[i].x) + "\n\tY-Coordinate : " 
      //               + String(Objects[i].y) + "\n}";
      // char* topic = "Obstacles";
      // pub(JSON, topic);
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

  if (xlocation >300 || ylocation > 200 || xlocation < 0 || ylocation < 0)
  {
    Serial.println("Wall found");
    rover.detection = 0;
    rover.walldetection = 1;
  }

  if (distance_cm < 20 && !(xlocation >300 || ylocation > 200 || xlocation < 0 || ylocation < 0))
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
  //Serial.println("X: " + String(xlocation));
  //Serial.println("Y: " + String(ylocation));

  rover.sthdetection = rover.walldetection||rover.detection;
}

// void automaticMode()
// {
//   String mode = roverCommand;
//   if(mode == "{\n\"mode\" : 1\n}")
//   {rover.autoMode = true;
//   Serial.println("AutoMode enabled");}
//   else 
//   {rover.autoMode = false;
//   Serial.println("AutoMode disabled");}
// }

int batterytime = millis();

void roverDataTransfer()
{
    char* topic = "location";
    char* topic_battery = "battery";
    // String JSON = "{\n\txcoord:" + String(rover.X) + ",\n\tycoord : " + String(rover.Y) + 
    // ",\n\tobstacle : " + String(rover.angle) + "\n}";
    // pub(JSON, topic);

    if (millis()-batterytime > 20000)
    {
      batterytime = millis();
      getbattery();
    }
    
   

    location["xcoord"] = rover.X;
    location["ycoord"] = rover.Y;
    location["angle"] = rover.angle;
    location["avoidstep"] = rover.avoidStep;
    if (CoordinateSequence.size() != 0)
    {
    location["xtarget"] = CoordinateSequence[(CoordinateSequence.size() -1)].x;
    location["ytarget"] = CoordinateSequence[(CoordinateSequence.size() -1)].y;
    }
    else 
    {
    location["xtarget"] = 0;
    location["ytarget"] = 0;
    }
    
    serializeJson(location,buffer);
    serializeJson(battery,battery_buffer);
    pub(battery_buffer,topic);
    pub(buffer,topic_battery);
    location["obstacle"] = 0;
}

void halt() //stops the rover
  {robot.brake(1);
  robot.brake(2);}

bool targetMet = false;
int sweepx = 30;
int sweepy = 0;
int sweep(int& step)
{
  // if (step != 0)
  // {
  // int x = 10;
  // int y = 10;

  // if (step % 2 == 0)  {y += 10*(step);} //check if even step. y target increases.

  // else {y = rover.Y;}

  // if (step % 2 == 1){
  //   if (step-1 % 4 == 0)  {x += 20;}}//+=280

  // else {x = rover.X;}

  // if (y > 200)  {step = 0;}

  // if (step != 0)
  // {roverGoto(x, y);}

  // if (withinFive(x, rover.X) && withinFive(y, rover.Y))
  // {
  //   step++;
  // }
  // if (y > 200) {step = 0;
  // halt();} //stops the sweep mode.
  // }
  // return step;
  // if (step == 1)
  // {
  // int x = 30;
  // int y = 0;
  // }

  if (step != 0)
  {
    if (!targetMet) {
      roverGoto(sweepx, sweepy);
    }
  }
  if (withinFive(sweepx, rover.X) && withinFive(sweepy, rover.Y))
  {
    step++;
    targetMet = true;
  }
  if ((step % 2 == 0) && (targetMet))  {
    sweepy += 10;
    targetMet = false;
    } //check if even step. y target increases.

  //else {y = rover.Y;}

  if ((step % 2 == 1) && (targetMet)){
    if (sweepx<5){
      sweepx =30;
      targetMet = false;
    }
    else{
      sweepx=0;
      targetMet = false;
    }
   }//=290

  //else {x = rover.X;}
  if (sweepy > 50) {
    step = 0;
    halt(); //stops the sweep mode.
  }
  return step;

}



void deleteElementsAvoid()
{
 for (int i = 0; i < CoordinateSequence.size(); i++)
  {
    if (withinFive(rover.X, CoordinateSequence[i].x) && withinFive(rover.Y,CoordinateSequence[i].y))
    {
      CoordinateSequence.erase(CoordinateSequence.begin()+i);
    }
  }
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

  if (rover.coordinateMode == 1 || centralCommand["mode"] == 2)
  {
    Serial.println("Recieved Command");
    
    rover.coordinateMode = 1;
    
    char msg[128];
    serializeJson(coordinates,msg);
    Serial.println(msg);

    int x = coordinates["xcoord"];
    int y = coordinates["ycoord"];
    
    Serial.print("X: ");
    Serial.println(x);
    Serial.print("Y: ");
    Serial.println(y);

    Coordinate newCoord;
  
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
int distance = 20;
  if (rover.avoidStep == 1)
  {
    targetX = rover.X + distance* sin(3.14159*angleConvert(rover.angle+90)/180); //r
    targetY = rover.Y + distance* cos(3.14159*angleConvert(rover.angle+90)/180);
    rover.avoidStep++;
  }
  else if (rover.avoidStep == 2)
  {
    roverGoto(targetX, targetY);
    if (withinFive(rover.X, targetX) && withinFive(rover.Y, targetY))
    {halt();
      rover.avoidStep++;}
  }
  else if (rover.avoidStep == 3)
  {
    targetX = rover.X + distance* sin(3.14159*angleConvert(rover.angle-90)/180);
    targetY = rover.Y + distance* cos(3.14159*angleConvert(rover.angle-90)/180);
    rover.avoidStep++;
  }
  else if (rover.avoidStep == 4)
  {
    roverGoto(targetX, targetY);
    if (withinFive(rover.X, targetX) && withinFive(rover.Y, targetY))
    {halt();
      rover.avoidStep++;}
  }
  else if (rover.avoidStep == 5)
  {
    targetX = rover.X + distance* sin(3.14159*angleConvert(rover.angle-90)/180);
    targetY = rover.Y + distance* cos(3.14159*angleConvert(rover.angle-90)/180);
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
  // Joshua's
  // targetX = rover.X + distance* sin(3.14159*angleConvert(rover.angle+90)/180); //r
  // targetY = rover.Y + distance* cos(3.14159*angleConvert(rover.angle+90)/180);
  // while(!(rover.sthdetection&&(withinFive(rover.X, targetX) && withinFive(rover.Y, targetY)))){
  //   roverGoto(targetX, targetY);
  //   USensorFunction(rover.X, rover.Y, rover.angle, );
  // }
  // targetX = rover.X + distance* sin(3.14159*angleConvert(rover.angle-90)/180);
  // targetY = rover.Y + distance* cos(3.14159*angleConvert(rover.angle-90)/180);
  // while(!(rover.sthdetection&&(withinFive(rover.X, targetX) && withinFive(rover.Y, targetY)))){
  //   roverGoto(targetX, targetY);
  //   USensorFunction();
  // }
  // targetX = rover.X + distance* sin(3.14159*angleConvert(rover.angle-90)/180);
  // targetY = rover.Y + distance* cos(3.14159*angleConvert(rover.angle-90)/180);
  // while(!(rover.sthdetection&&(withinFive(rover.X, targetX) && withinFive(rover.Y, targetY)))){
  //   roverGoto(targetX, targetY);
  //   USensorFunction();
  // }
}

void setup() {
  Serial.begin(115200);

  // Setup SPI stuff
  pinMode(PIN_SS, OUTPUT);
  //SPI.begin();
   spi_returnval = 0;
  // Serial.println("Start");
  ledcAttachPin(buzzer, TONE_PWM_CHANNEL);
  startTime = millis();
  lastUsed = 0;
  // location["obstacle"] = 0;
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

  //rover.avoidStep = 1;

  //roverTurn(270);
  
  //FPGA SETUP STUFF
  pinMode(PIN_SS_FPGA,OUTPUT);
  pinMode(PIN_MOUSECAM_CS, OUTPUT);
  // // SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST); //Setting up SPI bus

  //setting up rover initials
  rover.X = 0;
  rover.Y = 0; 
  rover.angle = 0;
  rover.fpga_detection = 0;
  rover.autoMode = false;
  rover.detection = 0;
  rover.coordinateMode = 0;
  rover.avoidStep = 0;
  rover.autoStep = 0;
  rover.avoiding = 0;

  
  rover.coordinateMode = 1;
  Coordinate ega;
  ega.x = 0;
  ega.y = -10;
  CoordinateVector.push_back(ega);
  
  Coordinate egb;
  egb.x = 10;
  egb.y = 10;
  CoordinateVector.push_back(egb);
}



void FPGAdetect()
{
  // for (byte x = 1; x < 13; x+= 2)
  // {
  // delay(20);
  // grabBallData(x);
  // }
  // delay(20);
  // grabBallData(14);

  //FPGA
  int minDistance = 100;
  int distance = 0;
  //Ask for data for each ball
  for (byte x = 1; x < 13; x = x + 2) {
    delay(20);
    distance = grabBallData(x);
    if ((distance != 0) && (distance < minDistance)) {
      minDistance = distance;
    }
  }
  //Building
  delay(20);
  distance = grabBallData(14);
  if ((distance != 0) && (distance < minDistance)) {
      minDistance = distance;
    }
  //Buzzer sound
  if ((millis()-startTime) > minDistance*20) {
    startTime = millis();
    if (lastUsed == 0) {
      ledcWriteTone(TONE_PWM_CHANNEL, 200);
      lastUsed = 1;
    }
  } else {
    if (lastUsed == 1) {
      ledcWriteTone(TONE_PWM_CHANNEL, 0);
      lastUsed = 0;
    }
  }

}

int loopcount = 0;
int stepchecker = 0; //DEBUG FOR ROVERGOTO SEQUENCES
int timeTracker = 0; //Using for the sending of data at regular and non epilepsy inducing speeds
int previousTime = 0;
bool tester = 1;

void modeswitch()
{
  if (centralCommand["mode"] == 1)
  {
    rover.remoteControl = 1;
    rover.coordinateMode = 0;
    rover.autoMode = 0;
  }
  if (centralCommand["mode"] == 2)
  {
    rover.remoteControl = 0;
    rover.coordinateMode = 1;
    rover.autoMode = 0;
  }
  if (centralCommand["mode"] == 3)
  {
    rover.remoteControl = 0;
    rover.coordinateMode = 0;
    rover.autoMode = 1;
  }
}

void loop() {
  
  client.loop();
  wifi_check();
  modeswitch();
  rover.totalTime = millis();
  timeTracker += (rover.totalTime - previousTime);
  //FPGA, OPTIC STUFF
  FPGAdetect();
  digitalWrite(PIN_SS_FPGA, HIGH);
  delay(1);
  SPI.setDataMode(SPI_MODE3);
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  delay(1);
  opticMain();
  delay(1);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  USensorFunction(rover.X, rover.Y, rover.angle, rover.fpga_detection);




  if (rover.remoteControl)
  {
    roverMovement();
  }


  if (rover.coordinateMode)
  {
    getCoordinates();
    if (CoordinateVector.size() != 0)
    {roverGoto(CoordinateVector[returnClosestElement()].x, CoordinateVector[returnClosestElement()].y);}
    else {halt();}
    deleteElements();
  }
  
  if (timeTracker > 1000)
  {roverDataTransfer();}
  

  printCoordinates();
  angleConversion();

  if (rover.autoMode)
  {

  }

  
  

  // if (rover.detection && !rover.turning && tester)
  // {
  //   Coordinate target;
  //   rover.avoiding = 1;
  //   int distance = 20;
  //   int pythag = sqrt(2*(distance^2));
  //   //Right
  //   target.x = rover.X + distance*sin(3.14159*angleConvert(rover.angle)/180); //Last in the sequence
  //   target.y = rover.Y + distance*cos(3.14159*angleConvert(rover.angle)/180); //third here
  //   CoordinateSequence.push_back(target);
  //   target.x = rover.X + pythag*sin(3.14159*angleConvert(rover.angle + 45)/180); //Second in the sequence
  //   target.y = rover.Y + pythag*cos(3.14159*angleConvert(rover.angle + 45)/180); //Second here
  //   CoordinateSequence.push_back(target);
  //   target.x = rover.X + distance*sin(3.14159*angleConvert(rover.angle + 90)/180); //first in the sequence
  //   target.y = rover.Y + distance*cos(3.14159*angleConvert(rover.angle + 90)/180); //Will go here first
  //   CoordinateSequence.push_back(target);
  //   tester = 0;
    
  // }
  
  // if (rover.avoiding == 1)
  // {
  // if (CoordinateSequence.size() != 0)
  //   {
  //     int max = CoordinateSequence.size()-1;
  //     roverGoto(CoordinateSequence[max].x, CoordinateSequence[max].y);
  //     }
  //     deleteElementsAvoid();
  //     if (CoordinateSequence.size() == 0)
  //     {
  //       rover.avoiding = 0;
  //     }
  // }

// if (rover.autoStep >= 1)
// {
//   sweep(rover.autoStep);
// }
  delay(50);
  previousTime = rover.totalTime;

}