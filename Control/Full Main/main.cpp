#include <Arduino.h>
#include <vector>
//Servo library needed for Radar
#include <ESP32servo.h>


#define Radar 2
#define BATTERY 36
#define RadarAngle 4
#define FPGA_data 5
#define Motor1F 12  //Fwd
#define Motor2F 13
#define Motor1B 14  //Bwd
#define Motor2B 15
//Add ADNS3080 Input ports here. 

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

void setup()
{
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

}

void loop()
{ 
    delay(100); //Do everything every 100 ms?



    //BatteryLevel(2000);



    Serial.println("loop Executed");

}
