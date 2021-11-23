

  //(Rx, Tx), connect Tx of sensor board to pin 11 and Rx of sensor board to pin 12
String data="";
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

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
