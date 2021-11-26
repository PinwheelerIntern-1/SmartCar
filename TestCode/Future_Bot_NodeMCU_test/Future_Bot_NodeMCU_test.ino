#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
//RX Pin GPIO-14/D5
//TX Pin GPIO-12/D6
SoftwareSerial mSerial(2,14);
#define Bot_Name "_H"
//======================================WIFI CREDENTIALS for Hotspot==================================
const char* ssid = "belkin.3ea6";
const char* pwd = "d4f786c9";
//====================================================================================================
String URL;
int httpCode;
String payload;
String ID;
String state;


//====================================================================================================
String k;
//=============================================define the objects======================================
ESP8266WebServer server(80);
HTTPClient http;
//=========================================SETUP PART==================================================
void setup() {
  Serial.begin(9600);
  mSerial.begin(9600);
  WiFi.disconnect();
  pinMode(5, OUTPUT); //Pin D1 on NodeMCU
  pinMode(4, OUTPUT);  //Pin D2 on NodeMCU
  Serial.print("Connecting to WIFI:");
  WiFi.begin( ssid, pwd);
  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("I am Connected, ");
  Serial.print("My Local IP is : ");
  Serial.println((WiFi.localIP()));
  digitalWrite(5, 1);    //LED on pin D1 of node MCU turns on
  
  server.on("/", handleroot);

  server.on("/ispinwheel", Handshake);
  server.on("/TurnOn", Connected);
  server.on("/TurnOff", Disconnected);
  server.on("/Battery", Battery);
  server.on("/Left", Left);
  server.on("/Right", Right);
  server.on("/Forward", Forward);
  server.on("/Backward", Backward);
  server.on("/LEDOn", LEDOn);
  server.on("/LEDOff", LEDOff);
  server.on("/Stop", Stop);
  server.on("/1", Audio1);
  server.on("/2", Audio2);
  server.on("/3", Audio3);
  server.on("/4", Audio4);
  server.on("/5", Audio5);
  server.on("/6", Audio6);
  server.on("/7", Audio7);
  server.on("/8", Audio8);

  server.begin();
}
//=====================================handle root page================================================
void handleroot() {
  server.send(200, "text/plain", "Welcome TO Future");
}
//===================================AutoBot control page===========================================

void Audio1() {
  server.send(200, "text/plain", "Audio1");
  mSerial.write('1');
  Serial.println("1");
}

void Audio2() {
  server.send(200, "text/plain", "Audio2");
  mSerial.write('2');
  Serial.println("2");
}


void Audio3() {
  server.send(200, "text/plain", "Audio3");
  mSerial.write('3');
  Serial.println("3");
}


void Audio4() {
  server.send(200, "text/plain", "Audio4");
  mSerial.write('4');
  Serial.println("4");
}


void Audio5() {
  server.send(200, "text/plain", "Audio5");
  mSerial.write('5');
  Serial.println("5");
}
void Audio6() {
  server.send(200, "text/plain", "Audio6");
  mSerial.write('6');
  Serial.println("6");
}


void Audio7() {
  server.send(200, "text/plain", "Audio7");
  mSerial.write('7');
  Serial.println("7");
}


void Audio8() {
  server.send(200, "text/plain", "Audio8");
  mSerial.write('8');
  Serial.println("8");
}


void Handshake()
{
 server.send(200, "text/plain", String("yes")+String(Bot_Name));
  Serial.println(String("yes")+String(Bot_Name));
}
void Connected()
{
  digitalWrite(4, 1); 
   mSerial.write('c');
  Serial.println("Connected");
}
void Disconnected()
{
  digitalWrite(4, 0); 
   mSerial.write('d');
  Serial.println("Disconnected");
}

void Stop() {
  server.send(200, "text/plain", "Stop");
  mSerial.write('S');
  Serial.println("S");
}

void Left() {
  server.send(200, "text/plain", "LEFT");
  mSerial.write('L');
  Serial.println("L");
}
void Right() {
  server.send(200, "text/plain", "Right");
  mSerial.write('R');
  Serial.println("R");
}
void Forward() {
  server.send(200, "text/plain", "Forward");
  mSerial.write('F');
  Serial.println("F");
}
void Backward() {
  server.send(200, "text/plain", "Backward");
  mSerial.write('B');
  Serial.println("B");
}
void LEDOn() {
  server.send(200, "text/plain", "LEDOn");
  mSerial.write('A');
  Serial.println("A");
}
void LEDOff() {
  server.send(200, "text/plain", "LEDOff");
  mSerial.write('Z');
  Serial.println("Z");
}
void Battery() {
  mSerial.write('X');
  Serial.println("X");
  k = "";
  while (mSerial.available()) {
    char c = mSerial.read();
    k += c;
  }
  server.send(200, "text/plain", String(k));
  
}
//==============================================LOOP===================================================

void loop () 
{

  server.handleClient();
//k = "";
  //if (mSerial.available()) {
    //String myString = String(mSerial.read());
//server.send(200, "text/plain",myString);
//}

}
