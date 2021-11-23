#include <ModbusMaster.h>
#include <SoftwareSerial.h>
SoftwareSerial Max485LeftMotorSerial(2, 3);   //Serial Port Connected with MAX 485 Module on Left Motor
SoftwareSerial Max485RightMotorSerial(4, 5);  //Serial Port Connected with MAX 485 Module on Right Motor


#define MODBUS_DATA_TRANSACTION_PIN_LEFT A0   //DE & DE Pin of MAX485
#define MODBUS_DATA_TRANSACTION_PIN_RIGHT A2
ModbusMaster modbusMasterLeftMotorNode;                      //object for ModbusMaster
ModbusMaster modbusMasterRightMotorNode;

uint8_t leftMotorSlaveId = 1;   //Slave ID of LDrive
uint8_t rightMotorSlaveId = 1;   //Slave ID of Drive 


void setup()
{
  pinMode(MODBUS_DATA_TRANSACTION_PIN_LEFT, OUTPUT);
  pinMode(MODBUS_DATA_TRANSACTION_PIN_RIGHT, OUTPUT);
  
  
  Max485LeftMotorSerial.begin(9600);   //Modbus Baud rate is 9600 8N1
  Max485RightMotorSerial.begin(9600);   //Modbus Baud rate is 9600 8N1
  
  Serial.begin(115200);
  
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 0);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 0);
 
  modbusMasterLeftMotorNode.begin(Max485LeftMotorSerial);
  modbusMasterLeftMotorNode.preTransmission(preTransmission);
  modbusMasterLeftMotorNode.postTransmission(postTransmission);
  modbusMasterRightMotorNode.begin(Max485RightMotorSerial);
  modbusMasterRightMotorNode.preTransmission(preTransmission);
  modbusMasterRightMotorNode.postTransmission(postTransmission);
}
void loop() 
{
  //Forward();
  //delay(1000);
  spinRight();
  delay(5000);
  spinLeft();
  //delay(50000);
  //Backward();
  //delay(30000);
  //motorStop();
  delay(5000);
}
void motorStop()
{
  //Serial.println("Stop");
  modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, 515); // CCW-521, stop-512, brake-515, CW-513
  modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, 515); // CCW-521, stop-512, brake-515, CW-513
  
}
void Forward()
{
  Serial.println("Forward");
  int leftMotorSpeed = 900;
  int rightMotorSpeed = 900;
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
  modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, 513); // CCW-521, stop-512, brake-515, CW-513
  delay(10);
  modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
  delay(10);
  
}
void Backward()
{
  Serial.println("Backward");
  int leftMotorSpeed = 900;
  int rightMotorSpeed = 900;
  modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, 521); // CCW-521, stop-512, brake-515, CW-513
  delay(10);
  modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, 513); // CCW-521, stop-512, brake-515, CW-513
  delay(10);
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
}
void spinRobot(int m, int n)
{
  Serial.println("Spin");
  int leftMotorSpeed = 1500;
  int rightMotorSpeed = 1500;
  
  modbusMasterLeftMotorNode.Run_Motor(leftMotorSlaveId, m); // CCW-521, stop-512, brake-515, CW-513
  delay(10);
  modbusMasterRightMotorNode.Run_Motor(rightMotorSlaveId, n); // CCW-521, stop-512, brake-515, CW-513
  delay(10);
  setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
  
 
}
void spinRight()
{
   spinRobot(513, 513);
}
void spinLeft()
{
  spinRobot(521, 521);
 
}

void setMotorSpeed(int leftMotorRPM, int rightMotorRPM)
{
  modbusMasterLeftMotorNode.Set_Speed(leftMotorSlaveId, leftMotorRPM  ); // Set Speed, speed range is (0-1000 RPM)
  delay(10);
  modbusMasterRightMotorNode.Set_Speed(rightMotorSlaveId, rightMotorRPM ); // Set Speed, speed range is (0-1000 RPM)
   delay(10);
  
}
void preTransmission() {
   
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 1);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 1);
  //delay(10);
}
void postTransmission() {
   //delay(10);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_LEFT, 0);
  digitalWrite(MODBUS_DATA_TRANSACTION_PIN_RIGHT, 0);
}
