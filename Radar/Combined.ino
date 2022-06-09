
//LIBRARIES

#include <TimerOne.h>
#include <Servo.h>

//RADAR

const int inPin = 3;
unsigned volatile long inPinCounter = 0; //volatile makes it possible to change the variable even inside an interrupt - takes more memory but necessary
float frequency;
int timerinterval = 1000; //timer interval in milliseconds for radar

//SERVO

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
int t = 18; //time to sweep 180 degrees (seconds)

//LOOP Parameter

int n=0;
int delay_loop = 500;

//SETUP

void setup() {
  //SERIAL OUTPUT
  
  Serial.begin(9600);
  Serial.println();
  
  //SERVO
  
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  //RADAR aka Pulse Counter
  
  attachInterrupt(digitalPinToInterrupt (inPin), countFallingPulse, FALLING); //prioritizes doing the countFallingPulse on the falling edge of the signal
  
}

//FUNCTION FOR RADAR

void countFallingPulse(){ //whenever the inPin goes from Low to high, it will count on the falling edge of the signal
  inPinCounter++; //just increases the value of the volatile variable by 1
}

//MAIN LOOP

void loop() {

// main loop is the servo rotating by 1 degree (every  100ms), within that we want a loop every 1000ms to read the radar
  
  for (pos = 0; pos <= 180; pos += 1) { // goes from -90 degrees to +90 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    Serial.print(", Angle: ");
    Serial.print(-pos+90); //positive is right, negative is left
    Serial.println();
    
    if (n==5){
      n=0;
      Serial.print(inPinCounter); //prints the total number of pulses in the interval
      Serial.print(", Frequency: ");
      frequency =  inPinCounter*1000/delay_loop;
      Serial.print(frequency);
      Serial.print(" Hz,");
      Serial.println();
      inPinCounter = 0;
    
      if (frequency > 60){
        Serial.println();
        Serial.print("Fan detected at angle ");
        Serial.print(-pos+90);
        Serial.println();
        Serial.println();
      }
    }
    
    n++;
    delay(delay_loop/5); //delay until it moves to the next degree
  }

  
  for (pos = 180; pos >= 0; pos -= 1) { // goes from +90 degrees to -90 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    Serial.print(", Angle: ");
    Serial.print(-pos+90);
    Serial.println();

    if (n==5){
      n=0;
      Serial.print(inPinCounter); //prints the total number of pulses in the interval
      Serial.print(", Frequency: ");
      frequency =  inPinCounter*1000/delay_loop;
      Serial.print(frequency);
      Serial.print(" Hz,");
      Serial.println();
      inPinCounter = 0;
    
      if (frequency > 60){
        Serial.println();
        Serial.print("Fan detected at angle ");
        Serial.print(-pos+90);
        Serial.println();
        Serial.println();
      }
    }
    
    n++;
    delay(delay_loop/5); //delay until it moves to the next degree
  }

}
