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
#define MUSIC_AUDIO_ID 4
#define GOING_FORWARD_AUDIO_ID 5
#define GOING_REVERSE_AUDIO_ID 6
#define TURN_LEFT_AUDIO_ID 7
#define TURN_RIGHT_AUDIO_ID 8
#define SPIN_LEFT_AUDIO_ID 9
#define SPIN_RIGHT_AUDIO_ID 10
#define WELCOME_AUDIO_ID 11
#define NETWORK_CONNECTED_AUDIO 12
#define NETWORK_DISCONNECTED_AUDIO 13

//Constant Setup Values
short leftMotorSpeed = 600;
short rightMotorSpeed = 600;
short spinSpeed = 800;

uint8_t LeftMotorSlaveId = 1;   //Slave ID of LEFT Drive
uint8_t RightMotorSlaveId = 1;   //Slave ID of RIGHT Drive
// How many NeoPixels are attached to the Arduino?
int NeoPixelLedCount = 32;

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
bool isObstacle=false;
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
  DFPlayerObject.volume(15);  //Set volume value. From 0 to 30  
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 0);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 0); 
  
}

void loop() 
{
  NeoPixelDark();
  isObstacle=digitalRead(OBSTACLE_SENSOR_NANDGATE);
  if(NodeMCUSerial.available())
  {
    Serial.println("Got Data From NodeMCU");
    data_NodeMCU=NodeMCUSerial.read();
    Serial.println(data_NodeMCU);
  }
  if(LineSensorSerial.available())
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
      NeoPixelSolidBlue();
      delay(500);
      data_NodeMCU='0';
      break;
      
      case 'F':
      NeoPixelSolidGreen();
      ForwardTheBot(); 
      break;

      case 'B':
      NeoPixelSolidBlue();
      ReverseTheBot(); 
      break;
      
      case 'R': 
      SpinRightTheBot(); 
      break;
      
      case 'L': 
      SpinLeftTheBot(); 
      break;
      
      case 'D':
      if(isAudioPlaying)
      {
        DFPlayerObject.stop();
      }
      DFPlayerObject.play(GOING_REVERSE_AUDIO_ID);
      break;
      
      case 'A': //add code below to turn Neopixel LED on
      NeoPixelDancing();
      delay(500);
      break;
      
      case 'Z'://add code below to turn neopixel LED off
      NeoPixelDark();
      delay(500);
      break;
      
      case 'S': 
      StopTheBot();
      NeoPixelEyeBlink();
      delay(500);
      break;

      case 'c': 
      DFPlayerObject.play(GREET_AUDIO_ID);
      NeoPixelEyeBlink();
      delay(500);
      break;

      case 'd': 
      DFPlayerObject.play(MUSIC_AUDIO_ID);
      NeoPixelEyeBlink();
      delay(500);
      break;
      
      default: 
      data_NodeMCU='0'; 
      break;
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
void ForwardTheBot()
{
  Serial.println("Forward");
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
  modbusMasterLeftMotorNode.Run_Motor(LeftMotorSlaveId, 513); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  modbusMasterRightMotorNode.Run_Motor(RightMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
  delay(1);
  
}
void ReverseTheBot()
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
     delay(1000);
}
