#include <Arduino.h>

//All of the below are GPIO pin inputs 
#define mot1fwd 3 // to be physically linked to left wheel forward
#define mot1bwd 4 // to be physically linked to left wheel backward
#define mot2fwd 5 // to be physically linked to right wheel forward
#define mot2bwd 6 // to be physically linked to right wheel backward
#define command1 7 // Input for forward, 
#define command2 8 // left, 
#define command3 9 // right,
#define command4 10 // backwards

void setup() {

  //wifi setup... (tbd!)
  //This is where the commands will actually come from later on. 

  Serial.begin(115200);

  //Format of the following pins and commands is subject to change
  
  pinMode (mot1fwd, OUTPUT); // initiates the specified pins as outputs. 
  pinMode (mot1bwd, OUTPUT);
  pinMode (mot2fwd, OUTPUT);
  pinMode (mot2bwd, OUTPUT);
  pinMode (command1, INPUT); 
  pinMode (command2, INPUT);
  pinMode (command3, INPUT);
  pinMode (command4, INPUT);
}

void loop() {
  
  //code for moving the rover: 
  //Note : signals need to be sent to an intermediary piece of equipment 
  //Motor Control IC
  //The following code is tester code and subject to change. The idea
  //is that inputs will be sent to the ESP32 via the above specified pins
  //to test its ability to output correctly. If possible, a combination 
  //test with the motor can also be done. 


  int c1 = digitalRead(command1); //Binary read from GPIO pins.
  int c2 = digitalRead(command2); 
  int c3 = digitalRead(command3); 
  int c4 = digitalRead(command4); 
  
  if (c1 == HIGH)
  { digitalWrite(mot1fwd, HIGH);
    digitalWrite(mot2fwd, HIGH);

    while (c1 == HIGH)
    {
      delay(1000);
      Serial.println("Moving Forwards");
    }
    
  }

  if (c2 == HIGH) 
  { digitalWrite(mot1fwd, HIGH);
    digitalWrite(mot2bwd, HIGH); 

    while (c2 == HIGH)
    {
      delay(1000);
      Serial.println("Swivelling Left");
    }
  }

  if (c3 == HIGH)  
  { digitalWrite(mot1bwd, HIGH);
    digitalWrite(mot2fwd, HIGH);

    while (c3 == HIGH)
    {
      delay(1000);
      Serial.println("Swivelling Right");
    }
  }
    
  if (c4 == HIGH) 
  { digitalWrite(mot1bwd, HIGH);
    digitalWrite(mot2bwd, HIGH);

    while (c4 == HIGH)
    {
      delay(1000);
      Serial.println("Going Backwards");
    }
  }

  digitalWrite(mot1fwd, LOW); // reset it all to 0
  digitalWrite(mot2fwd, LOW);
  digitalWrite(mot1bwd, LOW);
  digitalWrite(mot2bwd, LOW);

}

  /*
  
  -------------------------------------- PREFACE --------------------------------------
  We want a few main things. 
  Firstly we want a persisent detection (hence being in the loop), for incoming commands.
  These can include commands to turn, stop, start, reverse, which will come from Command. 

  Next we want to relay information that is incoming from specific ports. 

  For example if we get the information from port x that
  ALIEN IS LOCATED AT COORDINATE X,Y, RELATIVE ANGLE POSITION IS...
  We would ideally like to send this information up the chain
  via wifi to the application for sending commands. 

  Another major component is the nexus aspect of the ESP system. Because the ESP and 
  FPGA are meant to workin tandem, the FPGA can recieve inputs and send information to 
  the ESP. This will simply take place via the various ports, and presumably via physical 
  connections TBD at a later time. The goal of this is also unclear. 

  Code Skeleton: 
  startup: 
  initiate wifi connection
  initiate pins needed

  loop: 

  send commands from command to the wheels (signals via the I/O ports)
  Structure: 

  FWD
    (
      while (fwdinput == high)
      setHigh(fwd1, fwd2, fwd3, fwd4);
    )
  REVERSE
    (
      while (revinput == high)
      setHigh(rev1, rev2, rev3, rev4);
    )
  LEFT
    (
      while (leftinput == high)
      sethigh (rightwheelfwd, leftwheelrev);

    )
  RIGHT
    (
      while (leftinput == high)
      sethigh (rightwheelrev, leftwheelfwd);

    )

    HALT
    (All stops)

    wireless communications: 
    -Send status updates:
      Location information
      Video feed
      Vehicle status (testing idea)
      Discovery logs? (Aliens found, alien buildings found?) May not be necessary or practical

  */
