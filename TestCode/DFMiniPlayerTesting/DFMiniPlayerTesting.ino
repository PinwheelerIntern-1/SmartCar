#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 8; // Connects to module's RX 
static const uint8_t PIN_MP3_RX = 9; // Connects to module's TX 
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

// Create the Player object
DFRobotDFPlayerMini player;

void setup() {

  // Init USB serial port for debugging
  Serial.begin(9600);
  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
   Serial.println("OK");

    // Set volume to maximum (0 to 30).
    player.volume(30);
    // Play the first MP3 file on the SD card
    
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void loop() {
  Serial.println("layng");
  player.play(1);
  delay(1000);
  player.play(2);
  delay(1000);
  player.play(3);
  delay(1000);
  player.play(4);
  delay(1000);
  player.play(5);
  delay(1000);
  player.play(6);
  delay(1000);
}
