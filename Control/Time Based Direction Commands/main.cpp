#include <Arduino.h>

//All of the below are GPIO pin inputs 
#define mot1fwd 3 // to be physically linked to left wheel forward
#define mot1bwd 4 // to be physically linked to left wheel backward
#define mot2fwd 5 // to be physically linked to right wheel forward
#define mot2bwd 6 // to be physically linked to right wheel backward
#define killprocess 38 // button pin

void forward(int time) //forward for time seconds
{
    digitalWrite(mot1fwd, HIGH);
    digitalWrite(mot2fwd, HIGH);
    Serial.println("Moving Forwards for ");
    Serial.println(time);
    Serial.println(" seconds.");
    delay(time * 1000); //delay in ms
    digitalWrite(mot1fwd, LOW); //Reset
    digitalWrite(mot2fwd, LOW);
}

void backwards(int time) //forward for time seconds
{
    digitalWrite(mot1bwd, HIGH);
    digitalWrite(mot2bwd, HIGH);
    Serial.println("Moving backwards for ");
    Serial.println(time);
    Serial.println(" seconds.");
    delay(time * 1000); //delay in ms
    digitalWrite(mot1bwd, LOW); //Reset
    digitalWrite(mot2bwd, LOW);
}

void swivel(int time, int direction) //forward for time seconds 
{
  if (direction == 0)
  {
    digitalWrite(mot1fwd, HIGH);
        digitalWrite(mot2bwd, HIGH);
        Serial.println("Swivelling Left for ");
        Serial.println(time);
        Serial.println(" seconds.");
        delay(time * 1000); //delay in ms
        digitalWrite(mot1fwd, LOW); //Reset
        digitalWrite(mot2bwd, LOW);
  }

  else if (direction == 1)
  {
    digitalWrite(mot2fwd, HIGH);
        digitalWrite(mot1bwd, HIGH);
        Serial.println("Swivelling Right for ");
        Serial.println(time);
        Serial.println(" seconds.");
        delay(time * 1000); //delay in ms
        digitalWrite(mot2fwd, LOW); //Reset
        digitalWrite(mot1bwd, LOW);
  }

  else 
  {
    Serial.println("Invalid Command");
  }

}

void distancefwd(int centimeters)
{
  //empty function for now.
  //specifies a set distance for the rover to travel
  //for example it is given 100 cm to travel.
  //will check current location, and during the process of going forward polls its current location.
  //calculates total distance travelled
  //if total distance is within 10 cm, will slow down (Important to figure out)
  //if distance travelled == specified distance, halt process. 
  //same for backwards but backwards
}

//Note for the swivel function: Angular change is needed for tracking purposes to update location. 


void setup() {

  Serial.begin(115200);

  pinMode (mot1fwd, OUTPUT); // initiates the specified pins as outputs. 
  pinMode (mot1bwd, OUTPUT);
  pinMode (mot2fwd, OUTPUT);
  pinMode (mot2bwd, OUTPUT);
  pinMode (killprocess, INPUT);
  
}

void loop() {
  
    forward(2); //Basic test: goes forward for 2 seconds, turns left, turns right, goes backwards for 2 seconds.
    swivel(1, 0);
    swivel(1, 1);
    backwards(2);

    if (digitalRead(killprocess) == HIGH)
    {
      Serial.println("Process ended.");
      return;
    }
}
