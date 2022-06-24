#include <string.h> 
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include<HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
//#include <WebSocketsClient.h>
#include <PubSubClient.h>
#include <Robojax_L298N_DC_motor.h>
#include <math.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <QuickSortLib.h>


//#define RST_PIN 3
//#define SS_PIN 2

#define CHA 0
#define ENA 14 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 12
#define IN2 13 
#define IN3 17
#define IN4 16
#define ENB 26// this pin must be PWM enabled pin if Arduino board is used
#define CHB 1
const int CCW = 2; // do not change
const int CW  = 1; // do not change
#define motor1 1 // do not change
#define motor2 2 // do not change
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);


#define PIN_SS        5
#define PIN_MISO      19 
#define PIN_MOSI      23 
#define PIN_SCK       18

#define PIN_MOUSECAM_RESET     4
#define PIN_MOUSECAM_CS        5

#define ADNS3080_PIXELS_X                 30
#define ADNS3080_PIXELS_Y                 30

#define ADNS3080_PRODUCT_ID            0x00
#define ADNS3080_REVISION_ID           0x01
#define ADNS3080_MOTION                0x02
#define ADNS3080_DELTA_X               0x03
#define ADNS3080_DELTA_Y               0x04
#define ADNS3080_SQUAL                 0x05
#define ADNS3080_PIXEL_SUM             0x06
#define ADNS3080_MAXIMUM_PIXEL         0x07
#define ADNS3080_CONFIGURATION_BITS    0x0a
#define ADNS3080_EXTENDED_CONFIG       0x0b
#define ADNS3080_DATA_OUT_LOWER        0x0c
#define ADNS3080_DATA_OUT_UPPER        0x0d
#define ADNS3080_SHUTTER_LOWER         0x0e
#define ADNS3080_SHUTTER_UPPER         0x0f
#define ADNS3080_FRAME_PERIOD_LOWER    0x10
#define ADNS3080_FRAME_PERIOD_UPPER    0x11
#define ADNS3080_MOTION_CLEAR          0x12
#define ADNS3080_FRAME_CAPTURE         0x13
#define ADNS3080_SROM_ENABLE           0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER      0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER      0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER      0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER      0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER           0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER           0x1e
#define ADNS3080_SROM_ID               0x1f
#define ADNS3080_OBSERVATION           0x3d
#define ADNS3080_INVERSE_PRODUCT_ID    0x3f
#define ADNS3080_PIXEL_BURST           0x40
#define ADNS3080_MOTION_BURST          0x50
#define ADNS3080_SROM_LOAD             0x60
#define ADNS3080_PRODUCT_ID_VAL        0x17





//MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long previousMillis = 0;
unsigned long interval = 30000;
const char* host_ip= "35.176.71.115";
int host_port = 3000;

//MQTT Broker data:
const char *broker = "35.176.71.115";
char *topic = "test";
char *topic2 = "epic";
const char *mqtt_user ="marsrover";
const char *mqtt_pass = "marsrover123";
const int mqtt_port = 1883;

String roverCommand = "";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) { //Data received
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 String command = "";
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
     command+= (char) payload[i];
 }
 roverCommand = command;

 Serial.println();
 Serial.println("-----------------------");
}

void pub(String message,char *topic){
  //convert string to char*
  const char *cstr = message.c_str();
  client.publish(topic, cstr); 
  Serial.println("Message sent");
}

void sub(char *topic){
  Serial.println("Waiting for sub...");
  client.subscribe(topic);
}

void mqttConnect(){
  client.setServer(broker,mqtt_port);
  client.setCallback(callback);

  while(!client.connected()){ //connects if not connected
    String client_id = "esp32-client-";
    client_id+= String(WiFi.macAddress());
    if (client.connect(client_id.c_str(),mqtt_user,mqtt_pass)){
      Serial.println("Connected to MQTT Broker");
    }
    else{
      Serial.println("Failed with state: ");
      Serial.print(client.state());
    }
    pub("hello",topic);
    sub(topic2);
  }
}

void initWifi(){
  WiFi.mode(WIFI_STA); //Connection Mode (Connecting to Access Point Mode)

  int networks = WiFi.scanNetworks();
  Serial.println("Networks: "+networks);
  for (int i=0;i<networks;i++){
    Serial.println("Name: "+WiFi.SSID(i));}

      //**Access Point Details**//
  const char* ssid = "LAPTOP-UU8ERU01 2861";
  const char* password = "-52675Uy";


  WiFi.begin(ssid,password);
  Serial.print(" Connecting to WiFi ...");
  while (WiFi.status()!=WL_CONNECTED){
    Serial.print('.');
    delay(1000);
  }
  Serial.println("Connected Successfully");
  Serial.println(WiFi.localIP());
}

void wifi_check(){
  unsigned long currentMillis = millis();
  if (WiFi.status()!=WL_CONNECTED && (currentMillis - previousMillis >=interval)){
    Serial.println("Reconnecting to Wifi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}


//RoverObject
class locationdata
{
  public: 
  int X;
  int Y;
  double angle;
  double angledisplay;
  double distance;
  int BatteryPercentage;
};

locationdata rover; //create the rover object

//GYROSTUFF
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

void gyroSetup(){
  mpu.begin();
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  rover.angle = 0;
}

void angleConversion() {
  int count = rover.angle/360;
    if (rover.angle < 0){
      for(int i=0; i<count+1;i++){
        rover.angledisplay=360+rover.angle;
        
      }
    }
    else if (rover.angle > 360){
      for(int i=0; i<count;i++){
        rover.angledisplay=rover.angle-360;
        
      }
    }
    else{
      rover.angledisplay=rover.angle;
    }
}


void angleCalc(){
  mpu.getEvent(&a, &g, &temp);
  double radianspersec = g.gyro.z;
    if (abs(radianspersec) > 0.1){
    double degrees = 0.1*radianspersec*180/3.14159;
    rover.angle = rover.angle - degrees;
    angleConversion();
    }
}

void roverCoordUpdate(double dist)
{
  rover.X += dist*sin(rover.angledisplay);
  rover.Y += dist*cos(rover.angledisplay);
}

void printCoordinates(){
  Serial.print("X: ");
  Serial.println(rover.X);
  Serial.print("Y: ");
  Serial.println(rover.Y);
  Serial.println("Angle: " );
  Serial.println(rover.angledisplay);
}

//Camera (sonic) Detection
bool detection(){
  return 0;
}


//Rover Motors
int spd = 30;

void brakeBoth()
{
  robot.brake(1);
  robot.brake(2); 
}

void backward()
{
  robot.rotate(motor1, spd+10, CW);
  robot.rotate(motor2, spd+15, CCW);
  opticSensorFunc();
}

void turnCW()
{
  robot.rotate(motor1, 40, CW);
  robot.rotate(motor2, 35, CW);
}

void turnCCW()
{
  robot.rotate(motor1, 40, CCW);
  robot.rotate(motor2, 35, CCW);
}

void forward()
{
  robot.rotate(motor1, spd+10, CCW);
  robot.rotate(motor2, spd+15, CW);
  opticSensorFunc();
}

void rot(int deg){//rotate clockwise
  //for(double countangle = rover.angle; (rover.angle-countangle)<deg;){
    turnCW(); 
    delay(6660*deg/360);
    brakeBoth();
    angleCalc();
    //delay(100);  
    //}
  //brakeBoth();
  printCoordinates();
}

void rotn(int deg){//rotate anticlockwise
  //for(double countangle = rover.angle; (countangle-rover.angle)<deg;){
    turnCCW();
    delay(6660*deg/360);
    brakeBoth();
    angleCalc();
    //delay(100);
  //} 
  //brakeBoth();
  printCoordinates();
}

void dforward(int period){//forward until detect object or 3 sec elapsed
  uint32_t tStart = millis();
    while(((millis()-tStart) < period)&&detection==0){
      forward();
    } 
}

void sweep(bool rl){
  delay(100);
  dforward(15000);
  if(rl==0){
   rot(90);
   brakeBoth(); 
   dforward(3000);
   brakeBoth();   
   rotn(90);
   brakeBoth();
   if(detection==0){//right around object
      dforward(3000);
      brakeBoth();
      rotn(90);
      brakeBoth();
      dforward(3000);
      brakeBoth();
      rot(90);
      brakeBoth();
    }
    else{//right turn around
      rot(180);
      brakeBoth();
      rl = ~rl;
    }
  }
  else{
   rotn(90);
   brakeBoth(); 
   dforward(3000);
   brakeBoth();   
   rot(90);
   brakeBoth();
   if(detection==0){//left around object
      dforward(3000);
      brakeBoth();
      rot(90);
      brakeBoth();
      dforward(3000);
      brakeBoth();
      rotn(90);
      brakeBoth();
    }
    else{//left turn around
      rotn(180);
      brakeBoth();
      rl = ~rl;
    }
  }
}



void testmovement(){
  delay(3000);
  forward();
  delay(2000);
  brakeBoth();
  delay(1000);
  rot(90);
  brakeBoth(); 
  delay(500);
  forward();
  delay(2000);
  brakeBoth();
  delay(500);   
  rotn(90);
  brakeBoth();
  delay(500);
  forward();
  delay(2000);
  brakeBoth();
  delay(500);
  rotn(90);
  brakeBoth();
  delay(500);
  forward();
  delay(2000);
  brakeBoth();
  delay(500);
  rot(90);
  brakeBoth();
}


//optical sensor
//int total_x = 0;
int total_y = 0;


//int total_x1 = 0;
int total_y1 = 0;


//int x=0;
int y=0;

//int a=0;
int b=0;

//int distance_x=0;
//int distance_y=0;

volatile byte movementflag=0;
volatile int xydat[2];

char asciiart(int k)
{
  static char foo[] = "WX86*3I>!;~:,`. ";
  return foo[k>>4];
}

byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];


int convTwosComp(int b){
  //Convert from 2's complement
  if(b & 0x80){
    b = -1 * ((b ^ 0xff) + 1);
    }
  return b;
  }


int tdistance = 0;


void mousecam_reset()
{
  digitalWrite(PIN_MOUSECAM_RESET,HIGH); // issue here!
  delay(1); // reset pulse >10us
  digitalWrite(PIN_MOUSECAM_RESET,LOW);
  delay(35); // 35ms from reset to functional
}


int mousecam_init()
{
  pinMode(PIN_MOUSECAM_RESET,OUTPUT); 
  pinMode(PIN_MOUSECAM_CS,OUTPUT);

  digitalWrite(PIN_MOUSECAM_CS,HIGH);

  mousecam_reset(); //issue function

  return 1;
}

void mousecam_write_reg(int reg, int val)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW); 
  SPI.transfer(reg | 0x80);
  SPI.transfer(val);
  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  delayMicroseconds(50);
}

int mousecam_read_reg(int reg)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg);
  delayMicroseconds(75);
  int ret = SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  delayMicroseconds(1);
  return ret;
}

struct MD
{
 byte motion;
 char dx, dy;
 byte squal;
 word shutter;
 byte max_pix;
};


void mousecam_read_motion(struct MD *p)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(ADNS3080_MOTION_BURST);
  delayMicroseconds(75);
  p->motion =  SPI.transfer(0xff);
  p->dx =  SPI.transfer(0xff);
  p->dy =  SPI.transfer(0xff);
  p->squal =  SPI.transfer(0xff);
  p->shutter =  SPI.transfer(0xff)<<8;
  p->shutter |=  SPI.transfer(0xff);
  p->max_pix =  SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  delayMicroseconds(5);
}

// pdata must point to an array of size ADNS3080_PIXELS_X x ADNS3080_PIXELS_Y
// you must call mousecam_reset() after this if you want to go back to normal operation
int mousecam_frame_capture(byte *pdata)
{
  mousecam_write_reg(ADNS3080_FRAME_CAPTURE,0x83);

  digitalWrite(PIN_MOUSECAM_CS, LOW);

  SPI.transfer(ADNS3080_PIXEL_BURST);
  delayMicroseconds(50);

  int pix;
  byte started = 0;
  int count;
  int timeout = 0;
  int ret = 0;
  for(count = 0; count < ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y; )
  {
    pix = SPI.transfer(0xff);
    delayMicroseconds(10);
    if(started==0)
    {
      if(pix&0x40)
        started = 1;
      else
      {
        timeout++;
        if(timeout==100)
        {
          ret = -1;
          break;
        }
      }
    }
    if(started==1)
    {
      pdata[count++] = (pix & 0x3f)<<2; // scale to normal grayscale byte range
    }
  }

  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  delayMicroseconds(14);

  return ret;
}

void opticSetup()
{
  //mousecam_init();

    pinMode(PIN_SS,OUTPUT);
    pinMode(PIN_MISO,INPUT);
    pinMode(PIN_MOSI,OUTPUT);
    pinMode(PIN_SCK,OUTPUT);

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV32);
    SPI.setDataMode(SPI_MODE3);
    SPI.setBitOrder(MSBFIRST);
}

void opticSensorFunc()
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

  delay(10);

    //distance_x = convTwosComp(md.dx);
    rover.distance = convTwosComp(md.dy);
    angleCalc();
    roverCoordUpdate(rover.distance/4); //modified by 4 to approximate to 1cm per cm moved. //closer possible
}

void setup() {
  // put your setup code here, to run once:
  //SPI.begin();
  //mfrc522.PCD_Init();
  Serial.begin(115200); //opens serial connection to print to console
  Serial.println("Hello, ESP32!");
  
  gyroSetup();  
  rover.X = 0;
  rover.Y = 0; 
  rover.distance = 0;
  
  robot.begin();

  //initWifi();
  //mqttConnect();
  //initSocket();

  opticSetup();
   
  delay(100);

  //testmovement();
}

bool rl=0;

void loop(){
  //client.loop();
  //wifi_check();
  
  delay(1000);
  //opticSensorFunc();
  //roverCoordUpdate(rover.distance/40);
  printCoordinates();
 
  sweep(rl);
}
