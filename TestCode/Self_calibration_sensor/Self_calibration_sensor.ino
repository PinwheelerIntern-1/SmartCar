#include <EEPROM.h>

#define Num_Sensors 8

int countOfColor=0;
int Time_count=0;
int Buffer[Num_Sensors];
int i;
int val;
int reading[8];
byte Lookup[8]= {0, 4, 2, 6, 1, 5, 3, 7};
bool Calibration_flag=false;
bool White=true;
bool a=0;   //common flag variable
int temp_buff[8];
int button_press=0;
unsigned long t;
String data=""; 
int sensorWeight;

void setup() {
  // put your setup code here, to run once:
  DDRD|=B11111000; //enabling pins on port D
    
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(9, OUTPUT);

//Refer interrupts section in ATMEGA328P datasheet
//Enabling Interrupt0 on pin 2 to trigger on falling edge.
//Setting prescalar of timer 2(16 bit) and setting mode to normal mode.
cli();
EIMSK = B00000001;
EICRA = B00000010;
TCCR1A = B00000000;
TCCR1B = B00000100;
TIMSK1 = B00000000;
sei();

//retrieve sensor sample readings from EEPROM
for (int i=0; i<Num_Sensors; i++){
      reading[i]=(EEPROM.read(i*2)<<8) | EEPROM.read((i*2)+1);
      //Serial.println(reading[i]);
    }
}

//interrupt subrouting for calibration button
ISR (INT0_vect){
  //this if condition is for debouncing
  if(millis()-t>200){
  digitalWrite(9, HIGH);

  if(!TIMSK1) { TIMSK1= B00000001; TCNT1H=0; TCNT1L=0;} //resetting and enabling the timer
    button_press++;
  //Serial.println(button_press);

  }
      t=millis();
}

ISR(TIMER1_OVF_vect){

 //Serial.println(button_press);
     
 if (button_press==1) {  button_press++; }

 //This represents one button press, if another button press doesnt occur in next 1 second, calibration starts
 else if (button_press==2){ 
  Calibration_flag=true;  
  a=!a; digitalWrite(9, a);
  Time_count++; } 
  
 //If second button press occurs, it will flip the color mode from detecting light line to dark line or vice-versa                                                                                            
 else if(button_press==3) {Calibration_flag=false; White=!White;  button_press=0;  TIMSK1 = B00000000; digitalWrite(9, LOW);}
 else {button_press=0;}
}

void loop() {

 //Calibration subroutine
  if (Calibration_flag){


//set all values of temporary buffer to maximum value of ADC
    for (int i=0; i<Num_Sensors; i++){
    temp_buff[i]=1023;}
    
  //run the loop for taking samples for 5 seconds
   while(Time_count<5){
   //iterating through sensors
    for (i=0; i<Num_Sensors; i++){
      
    PORTD = (i<<3) | (1<<PORTD2);    //writing the adddress for MUX using port manipulation
    val=(i<4)? analogRead(A1) : analogRead(A0);
     //Store value of sensor reading id it is less then 20+the lowest reading so far.
      if (temp_buff[i]+20>val){
        temp_buff[i]=val;
      }
      delay(1); //Note: this delay is necessary for sampling.
     }
     
    }
 //reseting all parameters
 Time_count=0; 
 TIMSK1 = B00000000; 
 button_press=0;
 data="0";

 //Stoting the sample for future reference, these will be retreived by void setup()
for (i=0; i<Num_Sensors; i++){
    reading[i]=temp_buff[i]+30;
  EEPROM.update(i*2, reading[i]>>8);
  EEPROM.update((i*2)+1, reading[i]);
  }
     digitalWrite(9, LOW);
     Calibration_flag=false;    
  }
  
 //for loop to recheck the reading if no line is detected. The loop breaks if any sensor detects line color
for (int j=0; j<2; j++){
  // for loop to go through the array of sensors
  for (i=0; i<Num_Sensors; i++){
    //writing the address for the the MUX using port manipulation.
    PORTD = (i<<3) | (1<<PORTD2);
    //val=(i<4)? analogRead(A1) : analogRead(A0);

    //the following lines check if a sensor detects the line color or not
    //White indicates if light color is to be detected or dark color; it can be adjusted using the calibration feature.
    //Only 8 sensors are soldered, so first 4 sensors are read using first MUX, next four are read using second MUX.
    //If all 16 sensors are soldered, then A1 and A0 can be read simultaniously and i will be the address for both sensors.
    if (i<4){
     if (analogRead(A1) <=reading[i]){ 
      digitalWrite(7, White); 
      countOfColor+=White; 
      data+=White; 
      sensorWeight += (i + 1) * 1000 * White; 
      } 
    else{ 
      digitalWrite(7, !White); 
      countOfColor+=!White; 
      data+=!White;
      sensorWeight += (i + 1) * 1000 * !White;       
      }
    }
    
    else {  
      if (analogRead(A0) <=reading[i]){
        digitalWrite(6, White); 
        countOfColor+=White; 
        data+=White;
        sensorWeight += (i + 1) * 1000 * White; 
        } 
      else{ 
        digitalWrite(6, !White); 
        countOfColor+=!White; 
        data+=!White;
        sensorWeight += (i + 1) * 1000 * !White;         
        }
      }
    }
    digitalWrite(6, LOW);

   //the following lines check if the sensor weight is actually zero, it it is zero, then it will recheck, else it will break out of the loop.
    if (sensorWeight==0 && j==0){ continue;}
    else if(j==1 && sensorWeight==0) { countOfColor=1; break;}
    else if(sensorWeight!=0){ break;}
     // delay(1);
      
  }



//Serial.print(data);  //<<this line is for sending array of 1s and 0s as readings, uncomment if needed
Serial.print(sensorWeight/countOfColor);  //<< this line sends the weight calculated, uncomment if needed
//Serial.println(countOfColor);
data="";
countOfColor=0;
sensorWeight=0; 
}
