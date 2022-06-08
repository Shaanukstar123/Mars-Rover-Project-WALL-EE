#include <Arduino.h>
#include <vector>


#define Radar 2
#define BATTERY 36
#define RadarAngle 4
#define FPGA_data 5
#define Motor1F 12  //Fwd
#define Motor2F 13
#define Motor1B 14  //Bwd
#define Motor2B 15
//Add ADNS3080 Input ports here. 

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

// int degree = 0; externally defined.
int RadarFunctionality()
{
  //send to the radar: rotate by 1 degree.
  //degree++
  //if nothing detected, do nothing.
  //if (analogRead(FrequencyPin > 60))
  //  {
  //    return analogRead(FrequencyPin);
  //    if not 0 something will happen...
  //  }
  //    else 
  //  {
  //    return 0; // if 0 nothing will happen
  //  }

} //If all of the radar stuff needs to be locally hosted we will put the entire code here. 

void WiFiSetup() //Setup connection
{
  //put all wifi related stuff here. 
  //Not here at this time as it is not needed and breaks code sometimes. 
}

void WiFiCheck() //Check wifi connection in loop()
{

}

void commandCommunicate() //shaanuka's section applicable here
{

}

void commandConnect() //shaanuka's section applicable here
{

}

void BatteryLevel(int example)
{
  int discharge = example;
  //int discharge = analogRead(BATTERY);

  int level = (discharge*1000)/40960;

if (Rover.BatteryPercentage != level)
{
  Serial.print("Battery Percentage: ");
  Serial.print(level);
  Serial.println("% Power");
} // To avoid needless reprinting.

  Rover.BatteryPercentage = level;
}

void RadarDetection()
{
  
  if (digitalRead(Radar))
  {
    int angle = analogRead(RadarAngle);
    Serial.println("Radar has detected fan!");
    Serial.print("Detected at ");
    Serial.print(angle);
    Serial.println("Degrees from Rover Forward!");
    Serial.print("Frequency is: ");
    Serial.println("Test Frequency Here");
  }

  //approximate location of fan
  //anaglogRead for Frequency
  //send to command detection flag
  //send to command fan information (approximate x coordinate)

}

void DriveCommands()
{
    //digitalwrite to the drive module with correct values. 
    //read command from wifi
    //if else block for fwd, bwd, left, right
    //set correct pins HIGH, LOW;
    //if no input, set all inputs to LOW.
    
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
  //idea: buy a compass module in order to calibrate angle?
  //Manual callibration option?
  //Send specific data from command to tell Rover direction information. 
  //i.e, tell Rover it is facing in the 45 degree direction. 
  //so Rover.angle = 45;
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

}

void loop()
{ 
  delay(1000); //Do everything every 100 ms?



  //BatteryLevel(2000);



  Serial.println("loop Executed");

}
