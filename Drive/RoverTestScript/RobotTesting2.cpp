#include <Arduino.h>
#include <math.h>
#include <Robojax_L298N_DC_motor.h>

#define CHA 0
#define ENA 19 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 18
#define IN2 5 //will not use this in the final version as it clashes with optic sensor. 
//All seem to clash with optic sensor in some way but can be changed.
// motor 2 settings
#define IN3 17
#define IN4 16
#define ENB 4// this pin must be PWM enabled pin if Arduino board is used
#define CHB 1
const int CCW = 2; // do not change
const int CW  = 1; // do not change
#define motor1 1 // do not change
#define motor2 2 // do not change
// for two motors without debug information // Watch video instruciton for this line: https://youtu.be/2JTMqURJTwg
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);

void brakeBoth()
{
  robot.brake(1);
  robot.brake(2); 
}

void setup() {
  Serial.begin(115200);
  robot.begin();
  //L298N DC Motor by Robojax.com
}

void loop()
{
  delay(2000);
  robot.rotate(motor1, 50, CW);
  robot.rotate(motor2, 50, CW); //Forward
  delay(5000);
  brakeBoth();
  delay(1000);
  robot.rotate(motor1, 50, CW);
  robot.rotate(motor2, 50, CCW);//turns
  delay(5000);
  brakeBoth();
  delay(1000);
  robot.rotate(motor1, 50, CCW);
  robot.rotate(motor2, 50, CW); //turns opposite to before
  delay(5000);
  brakeBoth();
  delay(1000);
  robot.rotate(motor1, 50, CCW);
  robot.rotate(motor2, 50, CCW);
  delay(5000);
  brakeBoth();
  delay(1000);

    //What should happen:
    // Forward for 5 seconds,
    // Brake 1s,
    // Right/Left for 5 seconds
    // Brake 1s
    // Left/Right for 5 seconds 
    // Brake 1s
    // Backwards for 5 seconds, 
    // Brake 1s

}