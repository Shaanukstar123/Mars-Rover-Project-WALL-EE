#include <Wire.h>
#include <INA219_WE.h>
#include <SPI.h>
#include <SD.h>

INA219_WE ina219; // this is the instantiation of the library for the current sensor

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10; //hardwired chip select for the SD card
unsigned int loop_trigger;
unsigned int int_count = 0; // a variables to count the interrupts. Used for program debugging.
float Ts = 0.001; //1 kHz control frequency.
float current_measure;
float pwm_out;
float V_in, V, P, a, b,V_max;
boolean input_switch;
int state_num=0,next_state;
String dataString;

void setup() {
  //Some General Setup Stuff

  Wire.begin(); // We need this for the i2c comms for the current sensor
  Wire.setClock(700000); // set the comms speed for i2c
  ina219.init(); // this initiates the current sensor
  Serial.begin(9600); // USB Communications


  //Check for the SD Card
  Serial.println("\nInitializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("* is a card inserted?");
    while (true) {} //It will stick here FOREVER if no SD is in on boot
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  if (SD.exists("SD_Test.csv")) { // Wipe the datalog when starting
    SD.remove("SD_Test.csv");
  }

  
  noInterrupts(); //disable all interrupts
  analogReference(EXTERNAL); // We are using an external analogue reference for the ADC

  //SMPS Pins
  pinMode(13, OUTPUT); // Using the LED on Pin D13 to indicate status
  pinMode(2, INPUT_PULLUP); // Pin 2 is the input from the CL/OL switch
  pinMode(6, OUTPUT); // This is the PWM Pin
  
  //LEDs on pin 7 and 8
  pinMode(7, OUTPUT); //error led
  pinMode(8, OUTPUT); //some other digital out

  //Analogue input, the battery voltage (also port B voltage)
  pinMode(A0, INPUT);

  // TimerA0 initialization for 1kHz control-loop interrupt.
  TCA0.SINGLE.PER = 999; //
  TCA0.SINGLE.CMP1 = 999; //
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm; //16 prescaler, 1M.
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm;

  // TimerB0 initialization for PWM output
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm; //62.5kHz

  interrupts();  //enable interrupts.
  analogWrite(6, 120); //just a default state to start with

  //Output for relay module
  pinMode(D4, OUTPUT);

  //Power output of Buck SMPS
  pinMode(A2, INPUT);
}

void loop() {
  vb = analogRead(A0)*4.096; //Reads output voltage of boost SMPS


  //Relay module
  if((vb > 4.5) && (vb < 5.2)){ //Execute 'if' statement when output voltage is within desired range
    digitalWrite(5, LOW); //Relay module switch is on, i.e. an active low switch, when the voltage is within the desired range
  }else{
    digitalWrite(5, HIGH); //Relay module switches off if voltage is not within desired range
  }

  //Finding corresponding voltage for battery
  //Assume power for input of the battery is given by P = a*V + b, where P is power, V is voltage exiting the boost and a/b are constants
  // V is given by (P-b)/a. We know the power being outputted by the buck. Hence, to determine the corresponding voltage for the battery to achieve the same power, we merely insert P
  P = analogRead(A2); //Reads power value from buck
  V = (P-b)/a; //Determines corresponding voltage for battery, i.e. within the range of 4.5 and 5.2
  pwm_modulate(V/V_max); //V will be a fraction of the maximum output of the boost, i.e. when the duty cycle is 1
  
}

void pwm_modulate(float pwm_input){ // PWM function
  analogWrite(6,(int)(255-pwm_input*255)); 
}
