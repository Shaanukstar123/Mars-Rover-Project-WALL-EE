#include <Robojax_L298N_DC_motor.h>
// motor 1 settings
#define CHA 0
#define ENA 19 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 18
#define IN2 5
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
// for two motors with debug information
//Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA, IN3, IN4, ENB, CHB, true);
void setup() {
  Serial.begin(115200);
  robot.begin();
  //L298N DC Motor by Robojax.com
}
void loop() {
// move straight for 3 sec
  robot.rotate(motor1, 70, CW);//run motor1 at 60% speed in CW direction
  robot.rotate(motor2, 70, CCW);//run motor1 at 60% speed in CW direction
  delay(3000);
  robot.brake(1);
  robot.brake(2);  
  delay(3000);
 // rotate for 3 sec
  robot.rotate(motor1, 100, CW);//run motor1 at 60% speed in CW direction
  robot.rotate(motor2, 100, CW);//run motor1 at 60% speed in CW direction
  delay(3000);
  robot.brake(1);
  robot.brake(2);   
  delay(3000);
 // move straight for 3 sec
  robot.rotate(motor1, 70, CW);//run motor1 at 60% speed in CW direction
  robot.rotate(motor2, 70, CCW);//run motor1 at 60% speed in CW direction
  delay(3000);
  robot.brake(1);
  robot.brake(2);  
  delay(3000);
 // rotate for 3 sec in opposite ditection
  robot.rotate(motor1, 100, CCW);//run motor1 at 60% speed in CW direction
  robot.rotate(motor2, 100, CCW);//run motor1 at 60% speed in CW direction
  delay(3000);
  robot.brake(1);
  robot.brake(2);   
  delay(2000);
  for(int i=0; i<=100; i++)
  {
    robot.rotate(motor1, i, CW);// turn motor1 with i% speed in CW direction (whatever is i)
    robot.rotate(motor2, i, CCW);// turn motor1 with i% speed in CW direction (whatever is i) 
    delay(100);
  }
  delay(2000);
  robot.brake(1);
  robot.brake(2);
  delay(2000);  
}
