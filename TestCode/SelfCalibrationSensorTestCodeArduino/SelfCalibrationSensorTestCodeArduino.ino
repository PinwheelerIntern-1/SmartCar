
#include <SoftwareSerial.h>

#define Sensor_Tx  //to any pin on arduino which will serve as Rx pin 
#define Sensor_Rx  //to any pin on arduino which will serve as Tx pin
#define Sensor_calibration //tp the digital pin which will be used to calibrate sensor
SoftwareSerial Sensor(Sensor_Tx, Sensor_Rx), //connect Tx of sensor board to Rx pin of arduino and Rx of sensor board to Tx pin of arduino
String data="";
void setup() {
  pinMode(Sensor_calibration, OUTPUT);
  digitalWrite(Sensor_calibration, HIGH);
  // put your setup code here, to run once:
Serial.begin(9600);
Sensor.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available(){
    data=Sensor.readString();
//data=Sensor.readStringUntil('|');
 //  String data1=Sensor.readStringUntil('|');
 //  .
 //  .
 //  .
// Serial.println(data);
  }

Serial.println(data);
}
//call this function to calibrate
void Calibrate(){
  digitalWrite(Sensor_calibration, LOW);
  delay(100);
  digitalWrite(Sensor_calibration, HIGH);
}

//call this function to change mode
void ChangeLineMode(){
  digitalWrite(Sensor_calibration, LOW);
  delay(100);
  digitalWrite(Sensor_calibration, HIGH);
  delay(100);
  digitalWrite(Sensor_calibration, LOW);
  delay(100);
  digitalWrite(Sensor_calibration, HIGH);
}
}
