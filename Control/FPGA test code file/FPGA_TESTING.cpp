#include <Arduino.h>
#include <SPI.h>
#include <string.h>
#include <math.h>

#define Testinput 2

#define PIN_SS        5
#define PIN_MISO      19
#define PIN_MOSI      23
#define PIN_SCK       18

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

double distanceCalc(int width)
{
  double a = 68.23;
  double b = 0.9923;

  double powerVar = pow(b, width);

  return a*powerVar;
}


void analyseData(String x)
{
  //Identification
  bool detectionMade = false;
  String alienBin;
  for (int y = 0; y < 3; y++) //extracting the distance
    {alienBin += x[y];}

  if (alienBin != "000")
  {
    detectionMade == true;

    if (alienBin == "001")
    {
      Serial.println("Red Alien detected.");
    }

    if (alienBin == "010")
    {
      //green
      Serial.println("Green Alien detected.");
    }

    if (alienBin == "011")
    {
      //blue
      Serial.println("Blue Alien detected.");
    }

    if (alienBin == "100")
    {
      //orange 
      Serial.println("Orange Alien detected.");
    }

    if (alienBin == "101")
    {
      //pink
      Serial.println("Pink Alien detected.");
    }

    if (alienBin == "110")
    {
      //grey
      Serial.println("Grey Alien detected.");
    }

    //Distance
    String distanceBin;
  
  for (int y = 3; y < 11; y++) //extracting the distance
    {distanceBin += x[y];}
    //Serial.println(distanceBin);

    int pixelWidth = toInteger(distanceBin);

    int distance = distanceCalc(pixelWidth);
    Serial.print("Approximate distance from Rover : ");
    Serial.print(distance);
    Serial.println(" centimeters.");
    //angle

  String angleBin;

  for (int y = 11; y < 16; y++) //extracting the angle
    {angleBin += x[y];}


  
    int angle = toInteger(angleBin);
    double ratio = 2.1875;

    angle *=2.1875;

    Serial.print("Approximate angle from Rover : ");
    Serial.print(angle);
    Serial.println(" degrees.");


  }
}

void setup()
{ 

  pinMode(PIN_SS,OUTPUT);
  pinMode(PIN_MISO,INPUT);
  pinMode(PIN_MOSI,OUTPUT);
  pinMode(PIN_SCK,OUTPUT);

  Serial.begin(115200);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST); //Setting up SPI bus
}

void loop()
{

  digitalWrite(SS, LOW);

  int example = SPI.transfer16(0xff);
  //convert to binary
  recievedData = toBinary(example);

  digitalWrite(SS, HIGH);

  //testing input for checking deconversion
  //recievedData = "1001000000110011";


  Serial.println(recievedData);
  delay(1000);
  Serial.print("Processing data");

  for (int x = 0; x < 5; x++)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  analyseData(recievedData);

  //while(1);
}