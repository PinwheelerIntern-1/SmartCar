#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
//RX Pin GPIO-14/D5
//TX Pin GPIO-12/D6
SoftwareSerial mSerial(2, 14);
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
  Serial.begin(115200);
  mSerial.begin(115200);
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
  server.on("/Talk", Talk);

  server.begin();
}
//=====================================handle root page================================================
void handleroot() {
  server.send(200, "text/plain", "Welcome TO Future");
}
//===================================AutoBot control page===========================================

void Talk() {
  server.send(200, "text/plain", "Talking");
  mSerial.println("D");
  Serial.println("D");
}

void Handshake()
{
 server.send(200, "text/plain", String("yes")+String(Bot_Name));
  Serial.println(String("yes")+String(Bot_Name));
}
void Connected()
{
  digitalWrite(4, 1); 
  Serial.println("Connected");
}
void Disconnected()
{
  digitalWrite(4, 0); 
  Serial.println("Disconnected");
}

void Stop() {
  server.send(200, "text/plain", "Stop");
  mSerial.println("S");
  Serial.println("S");
}

void Left() {
  server.send(200, "text/plain", "LEFT");
  mSerial.println("L");
  Serial.println("L");
}
void Right() {
  server.send(200, "text/plain", "Right");
  mSerial.println("R");
  Serial.println("R");
}
void Forward() {
  server.send(200, "text/plain", "Forward");
  mSerial.println("F");
  Serial.println("F");
}
void Backward() {
  server.send(200, "text/plain", "Backward");
  mSerial.println("B");
  Serial.println("B");
}
void LEDOn() {
  server.send(200, "text/plain", "LEDOn");
  mSerial.println("A");
  Serial.println("A");
}
void LEDOff() {
  server.send(200, "text/plain", "LEDOff");
  mSerial.println("Z");
  Serial.println("Z");
}
void Battery() {
  mSerial.println("X");
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
