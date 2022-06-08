/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>
#include <TimerOne.h>


Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int time = 15; //seconds for it to sweep 180 degrees
double delayservo = time*1000/180; //delay for the servo in ms
double timerinterval = 1000; //timer interval in milliseconds
int i=0;


const int inPin = 3; //pin in of radar
unsigned volatile long inPinCounter = 0; //volatile makes it possible to change the variable even inside an interrupt - takes more memory but necessary
float frequency;

void setup() {
  Serial.begin(9600);
  Serial.println();
  
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  Timer1.initialize(timerinterval); //this is the timer interval in milliseconds
  
  attachInterrupt(digitalPinToInterrupt (inPin), countFallingPulse, FALLING); //prioritizes doing the countFallingPulse on the falling edge of the signal
}

void countFallingPulse(){ //whenever the inPin goes from Low to high, it will count on the falling edge of the signal
  inPinCounter++; //just increases the value of the volatile variable by 1
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from -90 degrees to +90 degrees
    
    if(i==12){
      Serial.print(inPinCounter); //prints the total number of pulses in the interval
      Serial.print(", Frequency: ");
      frequency = inPinCounter / (timerinterval / 1000);
      Serial.print(frequency);
      Serial.print(" Hz,");
      Serial.println();
      inPinCounter = 0;
      i=0;

      if (frequency > 60){
        Serial.println();
        Serial.print("Fan detected at angle ");
        Serial.print(pos);
        Serial.println();
        Serial.println();
      }
    }
    
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    Serial.print("i=");
    Serial.print(i);
    Serial.print(", Angle: ");
    Serial.print(-pos+90); //positive is right, negative is left
    Serial.println();
    i++;
    delay(delayservo); //delay until it moves to the next degree

  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from +90 degrees to -90 degrees

     if(i==12){
      Serial.print(inPinCounter); //prints the total number of pulses in the interval
      Serial.print(", Frequency: ");
      frequency = inPinCounter / (timerinterval / 1000);
      Serial.print(frequency);
      Serial.print(" Hz,");
      Serial.println();
      inPinCounter = 0;
      i=0;

      if (frequency > 60){
        Serial.println();
        Serial.print("Fan detected at angle ");
        Serial.print(pos);
        Serial.println();
        Serial.println();
      }
    }

    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    Serial.print("i=");
    Serial.print(i);
    Serial.print(", Angle: ");
    Serial.print(-pos+90);
    Serial.println();
    i++;
    delay(time*5.405);

  }
}
