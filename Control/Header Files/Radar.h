#include <ESP32servo.h>
#include <Arduino.h>
#include "connection.h"
#include <math.h>
#include <string.h>

#define RADAR_INPUT 30 //Input of High/low signal //30 for testing
#define SERVO_CONTROL 33 //output 


#define RADAR_HEIGHT 20 //cm
#define RADAR_ANGLE 30 // degrees

Servo myservo;
int theta = 0;
bool direction = 0; //0 is left, 1 is right
bool DEBUGMODE = false;

void servoSetup()
{
    // pinMode(RADAR_INPUT, INPUT);
    // pinMode(SERVO_CONTROL, OUTPUT);

    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

  myservo.setPeriodHertz(50);
  myservo.attach(SERVO_CONTROL);  
}

void radarDetection(int roverX, int roverY, int roverAngle)
{
    myservo.write(theta);

    if (digitalRead(RADAR_INPUT) == HIGH)
    {
        //angle calculation will be: roverangle + (theta - 90): theta is from 0 to 180.
        int detectionAngle = roverAngle + (theta-90);
        int distance = RADAR_HEIGHT * tan(RADAR_ANGLE);
        int xcoord = distance*sin(detectionAngle);
        int ycoord = distance*cos(detectionAngle);

        String JSON = "{\"Obstacle\" : \"Fan\"\nX : " + String(xcoord) + "\nY : " + String(ycoord) + "\n}";
        char* name = "Obstacle";
        pub(JSON, name);
    }

    if (direction == 1)
    {theta+=2;}
    if (direction == 0)
    {theta-=2;}
    if (theta >= 180)
    {direction = 0;}
    if (theta <= 0)
    {direction = 1;}
    
}
