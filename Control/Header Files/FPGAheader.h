#include <string.h> 
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>

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


