//https://www.youtube.com/watch?v=jZzpKifPUiE

#include <TimerOne.h>

const int inPin = 3;
unsigned volatile long inPinCounter = 0; //volatile makes it possible to change the variable even inside an interrupt - takes more memory but necessary
float frequency;
int timerinterval = 1000; //timer interval in milliseconds for radar

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println();

  Timer1.initialize(timerinterval); //this is the timer interval in milliseconds
  attachInterrupt(digitalPinToInterrupt (inPin), countFallingPulse, FALLING); //prioritizes doing the countFallingPulse on the falling edge of the signal
}

void countFallingPulse(){ //whenever the inPin goes from Low to high, it will count on the falling edge of the signal
  inPinCounter++; //just increases the value of the volatile variable by 1
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(inPinCounter); //prints the total number of pulses in the interval
  Serial.print(", Frequency: ");
  frequency = inPinCounter / (timerinterval / 1000);
  Serial.print(frequency);
     Serial.print(" Hz,");
     Serial.println();
  inPinCounter = 0;

  if (frequency > 60){
    Serial.println();
    Serial.print("Fan detected at angle XX!");
    Serial.println();
    Serial.println();
  }


  
  delay(timerinterval); //then waits 1 second before looping again

  //overall it samples for 1 second, then waits 1 second, then loops
}
