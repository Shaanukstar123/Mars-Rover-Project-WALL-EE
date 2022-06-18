#include <Arduino.h>
#include "SPI.h"

#include <math.h>

//GyroScope stuff
#include <OpticSensor.h>


class roverData
{
  public:
  int X;
  int Y;
  int angle;
};

roverData rover;

void setup()
{

  opticSetup();

  rover.angle = 0;
  rover.X = 0;
  rover.Y = 0;

  Serial.begin(115200);

  mpu.begin();

  if(mousecam_init()==-1)
  {
    Serial.println("Mouse cam failed to init");
    while(1);
  }
}

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
  
    if (abs(radianspersec) > 0.1){
    double degrees = 1*radianspersec*180/3.14159;
    rover.angle = rover.angle + degrees;
    angleConversion();
    }
}

void roverCoordUpdate(int dist)
{
  rover.X += dist*sin(rover.angle);
  rover.Y += dist*cos(rover.angle);
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

roverCoordUpdate(distance_y/25); //modified by /100 to approximate to 1cm per cm moved. //closer possible
angleCalc();
Serial.print("X: ");
Serial.println(rover.X);
Serial.print("Y: ");
Serial.println(rover.Y);
Serial.println("Angle: " );
Serial.println(rover.angle);
}

void loop()
{

opticMain(); //We've replaces everything here.
 
 
  delay(1000);

}