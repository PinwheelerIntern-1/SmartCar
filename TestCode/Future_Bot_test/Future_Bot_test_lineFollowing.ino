#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include <Adafruit_NeoPixel.h>
 
#define MODBUS_DATA_TRANSACTION_PIN_LEFT A0   //DE & DE Pin of MAX485
#define MODBUS_DATA_TRANSACTION_PIN_RIGHT A2
#define VOLTAGE_READING_PIN A3
#define OBSTACLE_SENSOR_NANDGATE  A4
#define RING_LED_NEOPIXEL A5

#define MOVE_ASIDE_AUDIO_ID 1
#define BATTERY_LOW_AUDIO_ID 2
#define GREET_AUDIO_ID 3
#define GOING_FORWARD_AUDIO_ID 4
#define GOING_REVERSE_AUDIO_ID 5
#define TURN_LEFT_AUDIO_ID 6
#define TURN_RIGHT_AUDIO_ID 7
#define SPIN_LEFT_AUDIO_ID 8
#define SPIN_RIGHT_AUDIO_ID 9
#define WELCOME_AUDIO_ID 10
#define NETWORK_CONNECTED_AUDIO 11
#define NETWORK_DISCONNECTED_AUDIO 12
#define MUSIC_JINGLE_AUDIO_ID 13
#define MUSIC_CARAVAN_AUDIO_ID 14
#define LEFT 1
#define RIGHT 0

#define AUDIO_FILE1 14
#define AUDIO_FILE2 15
#define AUDIO_FILE3 16
#define AUDIO_FILE4 19
#define AUDIO_FILE5 20
#define AUDIO_FILE6 29
#define AUDIO_FILE7 30
#define AUDIO_FILE8 31



//Constant Setup Values
short leftMotorSpeed = 800;
short rightMotorSpeed = 800;
short spinSpeed = 800;
short batteryLow=30;
int data;
uint8_t LeftMotorSlaveId = 1;   //Slave ID of LEFT Drive
uint8_t RightMotorSlaveId = 1;   //Slave ID of RIGHT Drive
// How many NeoPixels are attached to the Arduino?
int NeoPixelLedCount = 32;
//char data = 0;
int sensorWeight = 0;
int countofLineColor = 0;
int countOfLeftLineColor = 0;
int countOfRightLineColor = 0;
int totalNumberOfSensor=8;
short int markerCount;
short int desiredCount;
int noLineConfirmationCounter=10;


short minSpeed = 0;
short maxSpeed = 1000;

int spinDirection=LEFT;
float speedFactor = .3;


float Kp = 0.045;
float Ki = 0;
float Kd = 5;
float error = 0, P = 0, I = 0, D = 0, PIDvalue = 0;
float previousError = 0, previousI = 0;

boolean audioPlaying = false;
boolean isObstacle=false;
bool pause=false;
bool PreviousStateMarker

float voltage = 0;
float dsp = 1;
void setup()
DFRobotDFPlayerMini DFPlayerObject;
ModbusMaster modbusMasterLeftMotorNode;                     
ModbusMaster modbusMasterRightMotorNode;
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel ringLedNeoPixels = Adafruit_NeoPixel(NeoPixelLedCount, RING_LED_NEOPIXEL, NEO_GRB + NEO_KHZ800);

SoftwareSerial Max485LeftMotorSerial(2, 3);   //Serial Port(Rx,Tx) Connected with MAX 485 Module on Left Motor
SoftwareSerial Max485RightMotorSerial(4, 5);  //Serial Port(Rx,Tx) Connected with MAX 485 Module on Right Motor
SoftwareSerial DFPlayerSerial(9, 8);// (Rx,Tx)
SoftwareSerial NodeMCUSerial(6,7); // (Rx,Tx)
SoftwareSerial LineSensorSerial(11,12);//(Rx,Tx)

//Working Variables
int runningSpeed = 0;
char data_NodeMCU;
char data_LineSensor;

bool isAudioPlaying=false;


void setup() 
{
  // put your setup code here, to run once:
  pinMode(MODBUS_DATA_TRANSACTION_PIN_LEFT, OUTPUT);
  pinMode(MODBUS_DATA_TRANSACTION_PIN_RIGHT, OUTPUT);
  pinMode(RING_LED_NEOPIXEL, OUTPUT);
  pinMode(OBSTACLE_SENSOR_NANDGATE, INPUT);
  pinMode(VOLTAGE_READING_PIN,INPUT); 
  
  //Setting BaudRate 
  Max485LeftMotorSerial.begin(9600);   //Modbus Baud rate is 9600 8N1
  Max485RightMotorSerial.begin(9600);   //Modbus Baud rate is 9600 8N1
  
  DFPlayerSerial.begin(9600);   //Modbus Baud rate is 9600 8N1
  
  LineSensorSerial.begin(9600);
  
  NodeMCUSerial.begin(9600);
  
  Serial.begin(9600);

  //Calling Initialize Function from The Object
  ringLedNeoPixels.begin(); // This initializes the NeoPixel library.
  DFPlayerObject.begin(DFPlayerSerial);
  
  modbusMasterLeftMotorNode.begin(Max485LeftMotorSerial);
  modbusMasterLeftMotorNode.preTransmission(preTransmission);
  modbusMasterLeftMotorNode.postTransmission(postTransmission);
  
  modbusMasterRightMotorNode.begin(Max485RightMotorSerial);
  modbusMasterRightMotorNode.preTransmission(preTransmission);
  modbusMasterRightMotorNode.postTransmission(postTransmission);

 //Setting Initial Values
  DFPlayerObject.volume(30);  //Set volume value. From 0 to 30  
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 0);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 0); 
  DFPlayerObject.play(WELCOME_AUDIO_ID);
  delay(2000);
}

void loop() 
{
  NeoPixelDark();
  isObstacle=digitalRead(OBSTACLE_SENSOR_NANDGATE);
  
  //NodeMCUSerial.listen();
  while(NodeMCUSerial.available()>0)
  {
    Serial.println("Got Data From NodeMCU");
    data_NodeMCU=NodeMCUSerial.read();
    Serial.println(data_NodeMCU);
  }
  //LineSensorSerial.listen();
  while(LineSensorSerial.available()>0)
  {
    Serial.println("Got Data From LineSensor");
    data_LineSensor=LineSensorSerial.read();
    Serial.println(data_LineSensor);
  }
  
  if(isObstacle)
  {
    //Call What Need to happen when obstacle is there 
    StopTheBot();
    if(!isAudioPlaying)
    {
    DFPlayerObject.loop(MOVE_ASIDE_AUDIO_ID);
    isAudioPlaying=true;
    }
    NeoPixelSolidRed();
  }
  else
  {
    if(isAudioPlaying)
    {
      DFPlayerObject.stop();
      isAudioPlaying=false;
    }
    switch (data_NodeMCU)
    {
      case 'X'://Read Battery Level
      if(GetBatteryVoltage())
      NeoPixelSolidBlue();
     
      else
      {
        DFPlayerObject.play(BATTERY_LOW_AUDIO_ID);
      }
     
     
      break;
      
      case 'F':
      DFPlayerObject.play(GOING_FORWARD_AUDIO_ID);
      NeoPixelSolidGreen();
      ForwardTheBot(); 
      desiredCount++;
      
      break;

      case 'B':
      DFPlayerObject.play(GOING_REVERSE_AUDIO_ID);
      NeoPixelSolidBlue();
      ReverseTheBot(); 
      break;
      
      case 'R': 
      DFPlayerObject.play(TURN_RIGHT_AUDIO_ID);
      NeoPixelRightWhite();
      SpinRightTheBot(); 
      break;
      
      case 'L': 
      DFPlayerObject.play(TURN_LEFT_AUDIO_ID);
      NeoPixelLeftWhite();
      SpinLeftTheBot(); 
      break;
      
      case 'D':
      if(isAudioPlaying)
      {
        DFPlayerObject.stop();
      }
      DFPlayerObject.play(MUSIC_JINGLE_AUDIO_ID);
      break;
      
      case 'A': //add code below to turn Neopixel LED on
      DFPlayerObject.play(MUSIC_JINGLE_AUDIO_ID);
      for(int i=0;i<10;i++)
      {
      NeoPixelDancing();
      }
     
      break;
      
      case 'Z'://add code below to turn neopixel LED off
      DFPlayerObject.stop();
      NeoPixelDark();
      
      break;
      
      case 'S': 
      StopTheBot();
      NeoPixelEyeBlink();
    
      break;

      case 'c': 
      DFPlayerObject.play(NETWORK_CONNECTED_AUDIO);
      NeoPixelEyeBlink();
   
      break;

      case 'd': 
      DFPlayerObject.play(NETWORK_DISCONNECTED_AUDIO);
      NeoPixelEyeBlink();
     
      break;
   
    case '1': 
      DFPlayerObject.play(AUDIO_FILE1);
      NeoPixelEyeBlink();
      break;
    case '2': 
      DFPlayerObject.play(AUDIO_FILE2);
      NeoPixelEyeBlink();
      break;
    case '3': 
      DFPlayerObject.play(AUDIO_FILE3);
      NeoPixelEyeBlink();
      break;
    case '4': 
      DFPlayerObject.play(AUDIO_FILE4);
      NeoPixelEyeBlink();
      break;
    case '5': 
      DFPlayerObject.play(AUDIO_FILE5);
      NeoPixelEyeBlink();
      break;
    case '6': 
      DFPlayerObject.play(AUDIO_FILE6);
      NeoPixelEyeBlink();
      break;
    case '7': 
      DFPlayerObject.play(AUDIO_FILE7);
      NeoPixelEyeBlink();
      break;
    case '8': 
      DFPlayerObject.play(AUDIO_FILE8);
      NeoPixelEyeBlink();
      break;
      
      default: 
      data_NodeMCU='0'; 
      break;
      
  }
   data_NodeMCU='0'; 
   delay(10);
 }
 if (markerCount<desiredCount){
  readSensorData();
  act();
 }
 else if(markerCount==desiredCount){
  //
    markerCount=0;
    desiredCount=0;
    StopTheBot();
    
 }
}

void readSensorData(){
  if(LineSensorSerial.available()){
    data=LineSensorSerial.parseInt(); //data=(sensorWeight / countofLineColor);
    //Serial.println(data);
    
  }
  error = (((totalNumberOfSensor+1)*1000)/2) - data;
}

void act(){

  if (data==0){
    spinRobot(521,521);
  }
  else if(data>0){
    if(PreviousStateMarker){
      PreviousStateMarker=false;
      markerCount++;
    }
        motor_go(0, 0);
        ForwardTheBot();
        calculatePID();
        motorPIDcontrol();
  }
 else if(data==36000){
  PreviousStateMarker=true;
 }
 else if(pause){   //pause if a boolean which can be set using data from ESP or whatever means
  StopTheBot();
 }
 data=-1;
}

void motor_go(int leftMotorRPM, int rightMotorRPM)
{
  modbusMasterLeftMotorNode.Set_Speed(leftMotorSlaveId, leftMotorRPM * speedFactor  ); // Set Speed, speed range is (0-1000 RPM)
  modbusMasterRightMotorNode.Set_Speed(rightMotorSlaveId, rightMotorRPM * speedFactor ); // Set Speed, speed range is (0-1000 RPM)
  //Serial.print(rpm1);
  //Serial.print(" : ");
  //Serial.println(rpm);
}
void calculatePID()
{
  P = error;
  I = I + error;
  D = error - previousError;

  PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
  previousError = error;
}

void motorPIDcontrol()
{
  //on negative PID Turn Right
  int leftMotorSpeed = (maxSpeed / 2) - PIDvalue;
  int rightMotorSpeed = (maxSpeed / 2) + PIDvalue;

  leftMotorSpeed = constrain(leftMotorSpeed, minSpeed, maxSpeed);
  rightMotorSpeed = constrain(rightMotorSpeed, minSpeed, maxSpeed);
  float tpd = .8;
  if (error >= 4300)
  {
    modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, 515); // CCW-521, stop-512, brake-515, CW-513BRK
    modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
    motor_go(leftMotorSpeed, rightMotorSpeed * tpd);
  }
  else if (error <= -4300)
  {
    modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, 513); // CCW-521, stop-512, brake-515, CW-513
    modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, 515); // CCW-521, stop-512, brake-515, CW-513BRK
    motor_go(leftMotorSpeed * tpd, rightMotorSpeed);
  }
  else
  {
    if (speedFactor < 1)
    {
      motor_go(leftMotorSpeed, rightMotorSpeed);
      speedFactor += 0.01;
    }
    else
    {
      motor_go(leftMotorSpeed, rightMotorSpeed);
    }

  }
}


void StopTheBot()
{
  //Serial.println("Stop");
  /*for(int i=leftMotorSpeed;i>0;i--)
  {
    
  }*/
  modbusMasterLeftMotorNode.Run_Motor(LeftMotorSlaveId, 515); // CCW-521, stop-512, brake-515, CW-513
  modbusMasterRightMotorNode.Run_Motor(RightMotorSlaveId, 515); // CCW-521, stop-512, brake-515, CW-513
  
}
void ReverseTheBot()
{
  Serial.println("Forward");
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
  modbusMasterLeftMotorNode.Run_Motor(LeftMotorSlaveId, 513); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  modbusMasterRightMotorNode.Run_Motor(RightMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  
}
void ForwardTheBot()
{
  Serial.println("Backward");
  modbusMasterLeftMotorNode.Run_Motor(LeftMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  modbusMasterRightMotorNode.Run_Motor(RightMotorSlaveId, 513); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
}
void TurnRightTheBot()
{
  Serial.println("TurnRight");
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
  modbusMasterLeftMotorNode.Run_Motor(LeftMotorSlaveId, 512); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  modbusMasterRightMotorNode.Run_Motor(RightMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  
}
void TurnLeftTheBot()
{
  Serial.println("TurnLeft");
  modbusMasterLeftMotorNode.Run_Motor(LeftMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  modbusMasterRightMotorNode.Run_Motor(RightMotorSlaveId, 512); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
}
void spinRobot(int m, int n)
{
  Serial.println("Spin");
  modbusMasterLeftMotorNode.Run_Motor(LeftMotorSlaveId, m); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  modbusMasterRightMotorNode.Run_Motor(RightMotorSlaveId, n); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
}
void SpinRightTheBot()
{
   Serial.println("SpinRight");
   spinRobot(513, 513);
}
void SpinLeftTheBot()
{
  Serial.println("SpinLeft");
  spinRobot(521, 521); 
}


void setMotorSpeed(int leftMotorRPM, int rightMotorRPM)
{
  modbusMasterLeftMotorNode.Set_Speed(LeftMotorSlaveId, leftMotorSpeed  ); // Set Speed, speed range is (0-1000 RPM)
  delay(1);
  modbusMasterRightMotorNode.Set_Speed(RightMotorSlaveId, rightMotorSpeed ); // Set Speed, speed range is (0-1000 RPM)
   delay(1);
  
}

void preTransmission() {
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 1);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 1);
}
void postTransmission() {
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 0);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 0);
}

void NeoPixelSolidRed()
{
  for (int i=0;i<NeoPixelLedCount;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(50,0,0));
  }
   ringLedNeoPixels.show();
}

void NeoPixelRightWhite()
{
  for (int i=0;i<NeoPixelLedCount/2;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(200,200,200));
  }
   ringLedNeoPixels.show();
}
void NeoPixelLeftWhite()
{
  for (int i=NeoPixelLedCount/2;i<NeoPixelLedCount;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(200,200,200));
  }
   ringLedNeoPixels.show();
}
void NeoPixelSolidGreen()
{
  for (int i=0;i<NeoPixelLedCount;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,50,0));
  }
   ringLedNeoPixels.show();
}
void NeoPixelSolidBlue()
{
  for (int i=0;i<NeoPixelLedCount;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,50));
  }
   ringLedNeoPixels.show();
}

void NeoPixelRunningLight()
{
  for (int i=0;i<NeoPixelLedCount;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,50));
  }
   ringLedNeoPixels.show();
}
void NeoPixelDark()
{
  for (int i=0;i<NeoPixelLedCount;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
  }
   ringLedNeoPixels.show();
}
void NeoPixelWhiteBlink()
{
  for (int i=0;i<NeoPixelLedCount;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,50));
  }
   ringLedNeoPixels.show();
   delay(500);
   for (int i=0;i<NeoPixelLedCount;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
  }
   ringLedNeoPixels.show();
}
void NeoPixelBlueRunning()
{
   for (int i=0;i<NeoPixelLedCount/2;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,50));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,50));
   delay(100);
    ringLedNeoPixels.show();
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,0));
    delay(100);
     ringLedNeoPixels.show();
  }
  
}
void NeoPixelGreenRunning()
{
   for (int i=0;i<NeoPixelLedCount/2;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,50,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,50));
   delay(10);
    ringLedNeoPixels.show();
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,0));
    delay(10);
     ringLedNeoPixels.show();
  }
  
}
void NeoPixelYellowRunning()
{
   for (int i=0;i<NeoPixelLedCount/2;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(200,200,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(200,200,0));
   delay(10);
    ringLedNeoPixels.show();
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,0));
    delay(10);
     ringLedNeoPixels.show();
  }
  
}

void NeoPixelDancing()
{
   for (int i=0;i<NeoPixelLedCount/2;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(200,200,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(200,200,0));
   delay(10);
    ringLedNeoPixels.show();
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,0));
    delay(10);
     ringLedNeoPixels.show();
  }
   for (int i=0;i<NeoPixelLedCount/2;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,200,200));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,200,200));
   delay(10);
    ringLedNeoPixels.show();
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,0));
    delay(10);
     ringLedNeoPixels.show();
  }
  for (int i=0;i<NeoPixelLedCount/2;i++)
  {
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(200,0,200));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(200,0,200));
   delay(10);
    ringLedNeoPixels.show();
   ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,0));
    delay(10);
     ringLedNeoPixels.show();
  }
 
  
}

void NeoPixelEyeBlink()
{
  for(int k=0;k<2;k++)
  {
    for (int i=0;i<NeoPixelLedCount/4;i++)
    {
     ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(200,200,0));
    ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(200,200,0));
    }
     ringLedNeoPixels.show();
     delay(500);
     for (int i=0;i<NeoPixelLedCount/4;i++)
    {
     ringLedNeoPixels.setPixelColor(i,  ringLedNeoPixels.Color(0,0,0));
     ringLedNeoPixels.setPixelColor((NeoPixelLedCount/2)+i,  ringLedNeoPixels.Color(0,0,0));
     
    }
     ringLedNeoPixels.show();
     delay(500);
  }
}

bool GetBatteryVoltage() {
  // read the input on analog pin 0:
  float voltage;
  int voltageDigital = analogRead(VOLTAGE_READING_PIN);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5.0V):
  voltage = (voltageDigital * (5.0 / 1024.0) ) / 0.09;
  voltage = voltage - 2; // add on for calibration only for AB2
  float batteryPercentage = (voltage - 19.2) / 0.074; // full charge 26.6V
  if (voltage < 19.2)
  {
    NodeMCUSerial.print("LOW BATTERY : " + String(voltage) + "V, " + String(batteryPercentage) + "%");
    return false;
    //Serial.print("LOW BATTERY : " + String(voltage) + "V, " + String(batteryPercentage) + "%");
  }
  else 
  {
    NodeMCUSerial.print("Battery : " + String(voltage) + "V, " + String(batteryPercentage) + "%");
    return true;
    //Serial.print("Battery : " + String(voltage) + "V, " + String(batteryPercentage) + "%");
  }
}
