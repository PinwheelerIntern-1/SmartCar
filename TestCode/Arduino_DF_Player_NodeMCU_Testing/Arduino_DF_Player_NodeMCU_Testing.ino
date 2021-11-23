
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 8; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 9; // Connects to module's TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);
SoftwareSerial nodeMCUSerial(6,7) ; //(Rx,Tx)

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
  nodeMCUSerial.begin(115200);

}

void loop() 
{
    if (nodeMCUSerial.available() > 0)  
    {
      char data1 = nodeMCUSerial.read();
      Serial.println(data1);
       if (data1 == 'B')
        player.play(1);
       else if (data1 == 'H')
        player.play(2);
       else if (data1 == 'N')
        player.play(3);
       else if(data1 == 'R')
        player.play(4);
        else if( data1 == 'S')
         player.play(5);
         else if( data1 == 'U')
           player.play(6);
          else if(data1 == 'V')
            player.play(6);
           
     
    }
    delay(500);
      
    }
 
 
