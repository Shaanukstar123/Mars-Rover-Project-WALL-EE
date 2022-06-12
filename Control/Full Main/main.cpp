#include <Arduino.h>
#include <vector>

//library needed for communication with FPGA and the Optic sensor
#include "SPI.h"
//Servo library needed for Radar
#include <ESP32servo.h>

//Library for the Motors
#include <Robojax_L298N_DC_motor.h>



#define Radar 2
#define BATTERY 36
#define RadarAngle 4
#define FPGA_data 5
#define Motor1F 12  //Fwd
#define Motor2F 13
#define Motor1B 14  //Bwd
#define Motor2B 15
//Add ADNS3080 stuff here (Optic Sensor things)
//copied from the optic sensor test code.

#define PIN_SS        5
#define PIN_MISO      19 
#define PIN_MOSI      23 
#define PIN_SCK       18

#define PIN_MOUSECAM_RESET     12
#define PIN_MOUSECAM_CS        5

#define ADNS3080_PIXELS_X                 30
#define ADNS3080_PIXELS_Y                 30

#define ADNS3080_PRODUCT_ID            0x00
#define ADNS3080_REVISION_ID           0x01
#define ADNS3080_MOTION                0x02
#define ADNS3080_DELTA_X               0x03
#define ADNS3080_DELTA_Y               0x04
#define ADNS3080_SQUAL                 0x05
#define ADNS3080_PIXEL_SUM             0x06
#define ADNS3080_MAXIMUM_PIXEL         0x07
#define ADNS3080_CONFIGURATION_BITS    0x0a
#define ADNS3080_EXTENDED_CONFIG       0x0b
#define ADNS3080_DATA_OUT_LOWER        0x0c
#define ADNS3080_DATA_OUT_UPPER        0x0d
#define ADNS3080_SHUTTER_LOWER         0x0e
#define ADNS3080_SHUTTER_UPPER         0x0f
#define ADNS3080_FRAME_PERIOD_LOWER    0x10
#define ADNS3080_FRAME_PERIOD_UPPER    0x11
#define ADNS3080_MOTION_CLEAR          0x12
#define ADNS3080_FRAME_CAPTURE         0x13
#define ADNS3080_SROM_ENABLE           0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER      0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER      0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER      0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER      0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER           0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER           0x1e
#define ADNS3080_SROM_ID               0x1f
#define ADNS3080_OBSERVATION           0x3d
#define ADNS3080_INVERSE_PRODUCT_ID    0x3f
#define ADNS3080_PIXEL_BURST           0x40
#define ADNS3080_MOTION_BURST          0x50
#define ADNS3080_SROM_LOAD             0x60
#define ADNS3080_PRODUCT_ID_VAL        0x17

//Top level classes

//Servo setup stuff
Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

class RadarProperties
{
    public:
    int angle;
    int CurrentFrequency;
    bool Detection;
    bool Swingdirection; //0 is left, 1 is right
};

RadarProperties RadarObject; // Used for storage of data on the object

class FPGAproperties
{
    public:
    int numberAliens;
    int numberBuildings;
    bool alienPresent; //will send flags for if an alien is present
    bool buildingPresent; //will send flags for if a building is present
};

FPGAproperties FPGAobject;

class locationdata
{
  public: 
  int X;
  int Y;
  int angle;
  int BatteryPercentage;
};

locationdata Rover; //create the rover object

locationdata objects;

class objectlist
{
  public: 
  std::vector<locationdata> AlienList;
  std::vector<locationdata> FanList;
};


//Optic sensor setup functions and variables

int total_x = 0;
int total_y = 0;

int total_x1 = 0;
int total_y1 = 0;

int x=0;
int y=0;

int a=0;
int b=0;

int distance_x=0;
int distance_y=0;

struct MD
{
 byte motion;
 char dx, dy;
 byte squal;
 word shutter;
 byte max_pix;
};

int convTwosComp(int b){
  //Convert from 2's complement
  if(b & 0x80){
    b = -1 * ((b ^ 0xff) + 1);
    }
  return b;
  }

  void mousecam_reset()
{
  digitalWrite(PIN_MOUSECAM_RESET,HIGH); // issue here!
  delay(1); // reset pulse >10us
  digitalWrite(PIN_MOUSECAM_RESET,LOW);
  delay(35); // 35ms from reset to functional
}

int mousecam_init()
{
  pinMode(PIN_MOUSECAM_RESET,OUTPUT); 
  pinMode(PIN_MOUSECAM_CS,OUTPUT);

  digitalWrite(PIN_MOUSECAM_CS,HIGH);

  mousecam_reset(); //issue function

  return 1;
}

void mousecam_write_reg(int reg, int val)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW); 
  SPI.transfer(reg | 0x80);
  SPI.transfer(val);
  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  delayMicroseconds(50);
}

int mousecam_read_reg(int reg)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg);
  delayMicroseconds(75);
  int ret = SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  delayMicroseconds(1);
  return ret;
}

  void mousecam_read_motion(struct MD *p)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(ADNS3080_MOTION_BURST);
  delayMicroseconds(75);
  p->motion =  SPI.transfer(0xff);
  p->dx =  SPI.transfer(0xff);
  p->dy =  SPI.transfer(0xff);
  p->squal =  SPI.transfer(0xff);
  p->shutter =  SPI.transfer(0xff)<<8;
  p->shutter |=  SPI.transfer(0xff);
  p->max_pix =  SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  delayMicroseconds(5);
}

void opticSensorFunc()
{

  int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
  MD md;
  mousecam_read_motion(&md);
  //un-needed?
  for(int i=0; i<md.squal/4; i++)
    Serial.print('*');
  Serial.print(' ');
  Serial.print((val*100)/351);
  Serial.print(' ');
  Serial.print(md.shutter); Serial.print(" (");
  Serial.print((int)md.dx); Serial.print(',');
  Serial.print((int)md.dy); Serial.println(')');

    distance_x = convTwosComp(md.dx);
    distance_y = convTwosComp(md.dy);

total_x1 = total_x1 + distance_x; 
total_y1 = total_y1 + distance_y;

// total_x = total_x1/157;
// total_y = total_y1/157;

total_x = total_x1/25;
total_y = total_y1/25;

//new issue here: we need a manipulation of the coordinates in order to accurately
//map them into the map in command.

//for example: fwd + 90 degree turn + fwd is registered only as moving fwd twice in the same direction
//hence we need a method of checking the direction, and then manipulating the coordinates. 

//interconnectivity: 
//using the motors commands, we will send the total degrees turned to a manipulation function
//function will take the current rover angle (starting at 0), and establish the change in coordinates
//from going fwds and backwards at that angle.
//i.e: angle 0 -> x = 0 (only y)
//angle 45: y = x?


Serial.print('\n');


Serial.println("Distance_x = " + String(total_x));

Serial.println("Distance_y = " + String(total_y));
Serial.print('\n');

Rover.X = total_x;
Rover.Y = total_y;


}



//Motor Setup stuff here:




 
//Place Radar Code here


int ServoFunctionality()
{
  //servo function: go through whole function once. 
  if (RadarObject.angle <= 0)
  {
      RadarObject.Swingdirection = 1;
  }

  if (RadarObject.angle >= 180)
  {
      RadarObject.Swingdirection = 0;
  }

  if (RadarObject.Swingdirection == 0)
  {
      RadarObject.angle--;
      //servoupdate
  }

  if (RadarObject.Swingdirection == 1)
  {
      RadarObject.angle++; 
  }

    myservo.write(RadarObject.angle); //update the servoposition

    if (digitalRead(Radar) == HIGH)
    {
        RadarObject.Detection = 1;
    }
} 

void WiFiSetup() //Setup connection
{
  //put all wifi related stuff here. 
  //Not here at this time as it is not needed and breaks code sometimes. 
}

void WiFiCheck() //Check wifi connection in loop()
{

}



void commandConnect() //shaanuka's section applicable here
{

}

int BatteryLevel()
{
  
  int discharge = analogRead(BATTERY);

  int level = (discharge*1000)/40960;

if (Rover.BatteryPercentage != level)
{
  Serial.print("Battery Percentage: ");
  Serial.print(level);
  Serial.println("% Power");
} // To avoid needless reprinting.

  Rover.BatteryPercentage = level;
  return level;
}

void RadarDetection() // May be a redundant function. 
{
  
  if (digitalRead(Radar))
  {
      //Debugging stuff
    // int angle = analogRead(RadarAngle);
    // Serial.println("Radar has detected fan!");
    // Serial.print("Detected at ");
    // Serial.print(angle);
    // Serial.println("Degrees from Rover Forward!");
    // Serial.print("Frequency is: ");
    // Serial.println("Test Frequency Here");
   
  }

  //approximate location of fan
  //anaglogRead for Frequency
  //send to command detection flag
  //send to command fan information (approximate x coordinate)

}

void DriveCommands()
{
    String command; // = Incoming command...
    //command = sub(DriveCommands)
    //check for incoming commands:
    if (command == "Forward")
    {
        //set all other pins low
        //set forwardpins high
    }

    if (command == "Backward")
    {
        //set backward pins high
    }

    if (command == "Left")
    {
        //set all other pins low
        //set left pins high
    }

    if (command == "Right")
    {
        //set all other pins low
        //set right pins high
    }

    if (command = "None")
    {
        //set all pins LOW;
    }

}

void FPGA_Detection() //Do with Shaheen
{
  //read analogue, convert to binary

  //read bitwise output of FPGA
  //from bitstream: 16 bits, 
  //3 bits for colour, 
  //8 bits for distance,
  //5 bits for...

  //Determine colour, distance of alien

  //buildingflag:

  //alienflag:

  //send flag to controller, 
  //send alien data after a delay with a while wait for confirmation from controller. 
}

void locationupdate(double time, int action) 
{
  //Old locations in x and y update to new locations in x and y. 
  //To-do!
}

void locationsetup()
{
  Rover.X = 0;
  Rover.Y = 0;
  Rover.angle = 0; 
  
}

void sendData()
{
    int prevX, prevY; // previous coordinates of Rover
    //Radar detection
    if (RadarObject.Detection)
    {
        //send fan flag to command
        RadarObject.Detection = 0; // reset the value to 0. Has been detected now. 
       
    }

    //Battery Tracking
    if (BatteryLevel()!= Rover.BatteryPercentage)
    {
        int currentlevel = BatteryLevel();
        //send the current battery level to command:
    }

    //FPGA detection (aliens)
    if (FPGAobject.alienPresent)
    {
        //send colour, approximate distance and approximate coordinate
    }

    if (FPGAobject.buildingPresent)
    {
        //send approximate distance and approximate coordinate
    }

    //send total number of aliens found

    //Location Data

    if (Rover.X != prevX && Rover.Y != prevY)
    {
        //send Rover.X and Rover.Y to command.
        prevX = Rover.X;
        prevY = Rover.Y;
    }
}

void setup()
{
  pinMode(PIN_SS,OUTPUT);
  pinMode(PIN_MISO,INPUT);
  pinMode(PIN_MOSI,OUTPUT);
  pinMode(PIN_SCK,OUTPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);


  Serial.begin(115200);

  pinMode(Radar, INPUT);
  //Input pin for FPGA data:
  pinMode(FPGA_data, INPUT); 
  pinMode(BATTERY, INPUT);
  //FPGA data will come in as an analog signal.
  // We then need to convert the data to bitwise. 
  
  pinMode(4, OUTPUT);

  //Initialise X, Y coord. 
  locationsetup();

  //SERVO, RADAR

    RadarObject.angle = 0; // initialises the angle of the servo to 0 degrees.
    RadarObject.Detection = 0; //The variable storing whether a detection has been made
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

  myservo.setPeriodHertz(50);
  myservo.attach(12);  // attaches the servo on pin 12 to the servo object
  //Servo is controlled by adjusting the position.

  //Optic sensor stuff:
  mousecam_init(); //Sets up the optic sensor.

}

void loop()
{ 

    opticSensorFunc();
    delay(1000);

}