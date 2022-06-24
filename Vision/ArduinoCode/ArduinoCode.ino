#include <Arduino.h>
#include <SPI.h>
#include <string.h>
#include <math.h>

#define Testinput 2

#define PIN_SS        4
#define PIN_MISO      19
#define PIN_MOSI      23
#define PIN_SCK       18
SPISettings settings(128000, MSBFIRST, SPI_MODE1);

uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;
String recievedData;
byte incomingByte = 0;

//For each ball
void grabBallData(int ballCode) {
  //Ask for the two packets ball data
  SPI.beginTransaction(settings);
  digitalWrite(PIN_SS, LOW);
  byte syncPacket = SPI.transfer(0x0); //Packet required for sync
  byte packet1 = SPI.transfer(ballCode);
  //SPI.endTransaction();
  //digitalWrite(PIN_SS, HIGH);
  //SPI.beginTransaction(settings);
  //digitalWrite(PIN_SS, LOW);
  byte packet2 = SPI.transfer(ballCode+1);
  digitalWrite(PIN_SS, HIGH);
  SPI.endTransaction();
  //convert to binary
  int dataIn = (packet1 << 8) + (packet2);
  if (dataIn != 0 && dataIn != 65535) {
    recievedData = toBinary(dataIn);
    Serial.print(ballCode);
    Serial.print(" : ");
    Serial.println(recievedData);
    analyseData(recievedData);
  }
}

double distanceCalc(int width)
{
  double a = 68.23;
  double b = 0.9923;

  double powerVar = pow(b, width);

  return a*powerVar;
}

//Needs to be 16 bits
String toBinary(int n)
{
//    String r;
//    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
//    sprintf(r, "%16b%);
//    return r;
  int i = 15;
  String r;
  while (i >= 0) {
      if ((n >> i) & 1)
        r += "1";
      else
        r += "0";
      --i;
    }
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


void analyseData(String x)
{
  //Serial.println(recievedData);
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
      Serial.println("Yellow Alien detected.");
    }

    if (alienBin == "101")
    {
      //pink
      Serial.println("Pink Alien detected.");
    }

    if (alienBin == "110")
    {
      //grey
      Serial.println("Light Green Alien detected.");
    }
    if (alienBin == "111")
    {
      //building
      Serial.println("Building detected.");
    }
    //Distance
    String distanceBin = "";
  
  for (int y = 3; y < 11; y++) //extracting the distance
    {distanceBin += x[y];}
    distanceBin += "0"; //Needs a trailing zero as the data from the FPGA removes the LSB
    Serial.println(distanceBin);
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

 
    Serial.print("Approximate angle from Rover : ");
    Serial.print(angle);
    Serial.println(" degrees.");
  
  

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


}

//vspi default pins SCLK = 18, MISO = 19, MOSI = 23, SS = 5 
void setup() {

  Serial.begin(115200);

  // Setup SPI stuff
  pinMode(PIN_SS, OUTPUT);
  SPI.begin();
  spi_returnval = 0;
  Serial.println("Start");
}

void loop()
{
  //Ask for data for each ball
  for (byte x = 1; x < 13; x = x + 2) {
    delay(20);
    grabBallData(x);
  }
  //Building
  delay(20);
  grabBallData(14);
//  if (Serial.available() > 0) {
//    // read the incoming byte:
//    incomingByte = Serial.parseInt();
//    Serial.println(incomingByte);
//    // say what you got:
//    grabBallData(incomingByte);
//  }
}
