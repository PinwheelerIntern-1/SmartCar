// Simple program to control an RGB LED light using the Neopixel library
// Adapted from NeoPixel Ring simple sketch (c) 2013 Shae Erisson under the GPLv3

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
int LEDPIN = A5;

// How many NeoPixels are attached to the Arduino?
int NUMPIXELS = 32;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

int delayval = 1000; // delay for half a second

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {
  /*Red();
 delay(1000);
  Green();
 delay(1000);
  Blue();
 delay(1000);*/
  BlueRunning();

 GreenRunning();
for(int i=0;i<5;i++)
{
 YellowRunning();
 delay(50);
}

for(int i=0;i<5;i++)
{
 Dancing();
 delay(50);
}
for(int i=0;i<5;i++)
{
 Kwinking();
 delay(50);
}

 

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
 // pixels.setPixelColor(0, pixels.Color(150,0,0)); // red at about 60% of full brightness
  //pixels.setPixelColor(1, pixels.Color(0,150,0)); // green at about 60% of full brightness
  //pixels.setPixelColor(2, pixels.Color(0,0,150)); // blue at about 60% of full brightness
 // pixels.setPixelColor(3, pixels.Color(50,50,50)); // white at about 20% of full brightness
//  pixels.setPixelColor(4, pixels.Color(75,0,75)); // off
//  pixels.setPixelColor(5, pixels.Color(25,25,25)); // magenta
//  pixels.setPixelColor(6, pixels.Color(90,50,170)); // 
//  pixels.setPixelColor(7, pixels.Color(150,50,0)); // orange
//  pixels.setPixelColor(8, pixels.Color(75,75,0)); // red at about 60% of full brightness
//  pixels.setPixelColor(9, pixels.Color(100,100,100)); // green at about 60% of full brightness
 // pixels.setPixelColor(10, pixels.Color(150,0,150)); // blue at about 60% of full brightness
 // pixels.setPixelColor(11,0x661133); // white at about 20% of full brightness
 // pixels.setPixelColor(12, pixels.Color(0,0,0)); // off
 // pixels.setPixelColor(13, 0x054411); // magenta
 // pixels.setPixelColor(14, pixels.Color(20,100,100)); // 
  //pixels.setPixelColor(15, pixels.Color(75,25,25)); // orange

 // pixels.show(); // This sends the updated pixel color to the hardware.

 // delay(delayval); // Delay for a period of time (in milliseconds).
  
// / pixels.setPixelColor(0, pixels.Color(75,0,75)); // magenta at about 60% of full brightness
 // pixels.setPixelColor(1, pixels.Color(75,75,0)); // yellow at about 60% of full brightness
//  pixels.setPixelColor(2, 0,  75, 75); // cyan using shorter RGB option at about 60% of full brightness
 // pixels.setPixelColor(3, 100,100,100); // white using shorter RGB notation at 100% brightness
 // pixels.setPixelColor(4, 0x661133); // color using shorter HEX color code
 // pixels.setPixelColor(5, 0x054411); // color using shorter HEX color code
 // pixels.setPixelColor(6, pixels.ColorHSV(20,100,100)); // color using Hue/Saturation/Value
 // pixels.setPixelColor(7, pixels.ColorHSV(75,25,25)); // color using Hue/Saturation/Value
//  pixels.setPixelColor(8, pixels.Color(75,0,75)); // magenta at about 60% of full brightness
 // pixels.setPixelColor(9, pixels.Color(75,75,0)); // yellow at about 60% of full brightness
 // pixels.setPixelColor(10, 0,  75, 75); // cyan using shorter RGB option at about 60% of full brightness
 // pixels.setPixelColor(11, 100,100,100); // white using shorter RGB notation at 100% brightness
 // pixels.setPixelColor(12, 0x661133); // color using shorter HEX color code
//  pixels.setPixelColor(13, 0x054411); // color using shorter HEX color code
//  pixels.setPixelColor(14, pixels.ColorHSV(20,100,100)); // color using Hue/Saturation/Value
  //pixels.setPixelColor(15, pixels.ColorHSV(75,25,25)); // color using Hue/Saturation/Value

  //pixels.show(); // This sends the updated pixel color to the hardware.

//  delay(delayval); // Delay for a period of time (in milliseconds).

}
void SolidRed()
{
  for (int i=0;i<NUMPIXELS;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(50,0,0));
  
  }
   pixels.show();
  
}
void SolidGreen()
{
  for (int i=0;i<NUMPIXELS;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(0,50,0));
 
  }
   pixels.show();
}
void SolidBlue()
{
  for (int i=0;i<NUMPIXELS;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(0,0,50));
 
  }
   pixels.show();
}
void BlueRunning()
{
   for (int i=0;i<NUMPIXELS/2;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(0,0,50));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,50));
   delay(100);
    pixels.show();
   pixels.setPixelColor(i,  pixels.Color(0,0,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,0));
    delay(100);
     pixels.show();
  }
  
}
void GreenRunning()
{
   for (int i=0;i<NUMPIXELS/2;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(0,50,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,50));
   delay(10);
    pixels.show();
   pixels.setPixelColor(i,  pixels.Color(0,0,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,0));
    delay(10);
     pixels.show();
  }
  
}
void YellowRunning()
{
   for (int i=0;i<NUMPIXELS/2;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(200,200,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(200,200,0));
   delay(10);
    pixels.show();
   pixels.setPixelColor(i,  pixels.Color(0,0,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,0));
    delay(10);
     pixels.show();
  }
  
}

void Dancing()
{
   for (int i=0;i<NUMPIXELS/2;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(200,200,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(200,200,0));
   delay(10);
    pixels.show();
   pixels.setPixelColor(i,  pixels.Color(0,0,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,0));
    delay(10);
     pixels.show();
  }
   for (int i=0;i<NUMPIXELS/2;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(0,200,200));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,200,200));
   delay(10);
    pixels.show();
   pixels.setPixelColor(i,  pixels.Color(0,0,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,0));
    delay(10);
     pixels.show();
  }
  for (int i=0;i<NUMPIXELS/2;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(200,0,200));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(200,0,200));
   delay(10);
    pixels.show();
   pixels.setPixelColor(i,  pixels.Color(0,0,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,0));
    delay(10);
     pixels.show();
  }
 
  
}

void Kwinking()
{
   for (int i=0;i<NUMPIXELS/4;i++)
  {
   pixels.setPixelColor(i,  pixels.Color(200,200,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(200,200,0));
 
   
  }
     pixels.show();
     delay(1000);
     for (int i=0;i<NUMPIXELS/4;i++)
  {
  
  
   pixels.setPixelColor(i,  pixels.Color(0,0,0));
    pixels.setPixelColor((NUMPIXELS/2)+i,  pixels.Color(0,0,0));
   
  }
   pixels.show();
     delay(1000);
}
