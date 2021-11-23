#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

#define BLACK_LINE 0
#define WHITE_LINE 1

//Which Color Line You want to follow BLACK/DARK or WHITE/LIGHT
bool lineColor = WHITE_LINE;
//Protocol Commands
//B- BatteryVoltage
//R- Reset
//H-Home
//N-Next
//S-Stop
//U-T1
//V-T2
//W-T3
//X-T4
//Y-T5
//Z-T6
//M-Pause
//L-Resume
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

SoftwareSerial Max485LeftMotorSerial(2, 3);   //Serial Port Connected with MAX 485 Module on Left Motor
SoftwareSerial Max485RightMotorSerial(4, 5);  //Serial Port Connected with MAX 485 Module on Right Motor
SoftwareSerial dfPlayerSerial(8, 9);// 8-TX(mp3), 9-RX(mp3), audio player
SoftwareSerial NodeMCUSerial(6,7); // RX, TX
SoftwareSerial LineSensorSerial(11,12);

#define MODBUS_DATA_TRANSACTION_PIN_LEFT A6   //DE & DE Pin of MAX485
#define MODBUS_DATA_TRANSACTION_PIN_RIGHT A2

//Battery voltage reader pin
#define VOLTAGE_READING_PIN A0
#define OBSTACLE_SENSOR  A3
//STATUS LEDs
#define HEADLIGHT 12

ModbusMaster modbusMasterLeftMotorNode;                      //object for ModbusMaster
ModbusMaster modbusMasterRightMotorNode;

uint8_t leftMotorSlaveId = 1;   //Slave ID of LDrive
uint8_t rightMotorSlaveId = 1;   //Slave ID of Drive
int rpm = 0;

//STATES
#define STATE_LINE 0
#define STATE_NO_LINE 1
#define STATE_HOME 2
#define STATE_PAUSE 3

#define LEFT 1
#define RIGHT 2

char data = 0;
int sensorWeight = 0;
int countOfSensorsOnLine = 0;
int countOfLeftSensorsOnLine = 0;
int countOfRightSensorsOnLine = 0;
int totalNumberOfSensor=8;
int state=2;
int previousState=2;
int noLineConfirmationCounter=10;


short minSpeed = 0;
short maxSpeed = 1000;
short spinSpeed = 800;
int spinDirection=LEFT;
float speedFactor = .3;
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

float Kp = 0.045;
float Ki = 0;
float Kd = 5;
float error = 0, P = 0, I = 0, D = 0, PIDvalue = 0;
float previousError = 0, previousI = 0;

boolean audioPlaying = false;
boolean isObstacle=false;

float voltage = 0;
float dsp = 1;
void setup()
{
  pinMode(MODBUS_DATA_TRANSACTION_PIN_LEFT, OUTPUT);
  pinMode(MODBUS_DATA_TRANSACTION_PIN_RIGHT, OUTPUT);
  pinMode(HEADLIGHT, OUTPUT);
  pinMode(OBSTACLE_SENSOR, INPUT);
  pinMode(VOLTAGE_READING_PIN,INPUT);
  
  Max485LeftMotorSerial.begin(9600);   //Modbus Baud rate is 9600 8N1
  Max485RightMotorSerial.begin(9600);   //Modbus Baud rate is 9600 8N1
  dfPlayerSerial.begin(9600);   //Modbus Baud rate is 9600 8N1
  myDFPlayer.begin(dfPlayerSerial);
  LineSensorSerial.begin(9600);
  NodeMCUSerial.begin(115200);
  Serial.begin(115200);
  
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 0);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 0);
  myDFPlayer.volume(15);  //Set volume value. From 0 to 30
  modbusMasterLeftMotorNode.begin(Max485LeftMotorSerial);
  modbusMasterLeftMotorNode.preTransmission(preTransmission);
  modbusMasterLeftMotorNode.postTransmission(postTransmission);
  modbusMasterRightMotorNode.begin(Max485RightMotorSerial);
  modbusMasterRightMotorNode.preTransmission(preTransmission);
  modbusMasterRightMotorNode.postTransmission(postTransmission);
  ReadVoltage();
 
}
void(* resetFunc) (void) = 0; //declare reset function @ address 0


void loop() 
{
  isObstacle=digitalRead(OBSTACLE_SENSOR);
  if(isObstacle)
  {
    //Pause the Robot
  }
  if (NodeMCUSerial.available() > 0)
  {
    //Pause the Robot , Look for Command and Proceed
    char data1 = NodeMCUSerial.read();
    
  }
  if(LineSensorSerial.available()>0)
  {
     Run(LineSensorSerial.read());
     Act();
  }
}

void Act()
{
  switch (state)
  {
    case STATE_NO_LINE:
        if(spinDirection==LEFT)
        spinLeft();
        else
        spinRight();
      break;

    case STATE_LINE:
        motor_go(0, 0);
        Forward();
        calculatePID();
        motorPIDcontrol();
      break;

    case STATE_PAUSE:
      //myDFPlayer.play(3);
      break;

    case STATE_HOME:
      //myDFPlayer.play(3);
      break;
    default:
      break;
  }
}
bool confirmNoLine()
{
  //get sensor weight from Serial Data
  if (sensorWeight == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}


void Run(char data)
{
    Serial.print("Incoming Data from Line Sensor Board:");
    Serial.print(data);
    Serial.println();
   
    sensorWeight = 0; //extract sensor weight from data and assign it 
    countOfSensorsOnLine = 0;//extract count of Sensor On Line 
    countOfLeftSensorsOnLine=0;//extract count of Sensor on Line On Left side
    countOfRightSensorsOnLine=0;//extract count of Sensor on Line On Right side
    
    error = (((totalNumberOfSensor+1)*1000)/2) - (sensorWeight / countOfSensorsOnLine);
    previousState = state;
    if(countOfLeftSensorsOnLine>=4)
    {
      Serial.print("Next Turn will be Left");
      Serial.println();
      spinDirection=LEFT;
    }
    if(countOfRightSensorsOnLine>=4)
    {
      Serial.print("Next Turn will be Right");
      Serial.println();
      spinDirection=RIGHT;
    }
    else if (countOfSensorsOnLine > 0 && countOfSensorsOnLine <= 3)
    {
    noLineConfirmationCounter=0;
    state = STATE_LINE;
    digitalWrite(HEADLIGHT, HIGH);
    }
    else if (countOfSensorsOnLine == 0)
    {
    noLineConfirmationCounter++;
    if(noLineConfirmationCounter>=10)
    {
      noLineConfirmationCounter=0;
      state = STATE_NO_LINE;
    }
  }
  else if (countOfSensorsOnLine >= 8)
  {
    noLineConfirmationCounter=0;
    Serial.println("HOME");
    state = STATE_HOME;
  }
}
void slowDown()
{
  if (speedFactor >= 0.2)
    speedFactor -= 0.01;
}

void motorStop()
{
  //Serial.println("Stop");
  modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, 515); // CCW-521, stop-512, brake-515, CW-513
  modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, 515); // CCW-521, stop-512, brake-515, CW-513
  
}

void Forward()
{
  //Serial.println("Forward");
  modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, 513); // CCW-521, stop-512, brake-515, CW-513
  modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
}
void spinRobot(int m, int n)
{
  leftMotorSpeed = spinSpeed;
  rightMotorSpeed = spinSpeed;
  modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, m); // CCW-521, stop-512, brake-515, CW-513
  modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, n); // CCW-521, stop-512, brake-515, CW-513
  motor_go(leftMotorSpeed, rightMotorSpeed);
  //  if (speedFactor < 1)
  //  {
  //    motor_go(leftMotorSpeed, rightMotorSpeed);
  //    speedFactor += 0.01;
  //  }
  //  else
  //  {
  //    motor_go(leftMotorSpeed, rightMotorSpeed);
  //  }

}
void spinRight()
{
   spinRobot(513, 513);
}
void spinLeft()
{
  spinRobot(521, 521);
 
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
void ReadVoltage() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(VOLTAGE_READING_PIN);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 50V):
  voltage = (sensorValue * (5.0 / 1024.0) ) / 0.09;
  voltage = voltage - 2; // add on for calibration only for AB2
  float batteryPercentage = (voltage - 19.2) / 0.074; // full charge 26.6V
  if (voltage < 19.2)
  {
    NodeMCUSerial.print("LOW BATTERY : " + String(voltage) + "V, " + String(batteryPercentage) + "%");
    //Serial.print("LOW BATTERY : " + String(voltage) + "V, " + String(batteryPercentage) + "%");
    myDFPlayer.play(1);

  }
  else {

    NodeMCUSerial.print("Battery : " + String(voltage) + "V, " + String(batteryPercentage) + "%");
    //Serial.print("Battery : " + String(voltage) + "V, " + String(batteryPercentage) + "%");

  }
}
void preTransmission() {
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 1);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 1);
}
void postTransmission() {
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 0);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 0);
}
