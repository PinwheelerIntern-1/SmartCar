// constants won't change. They're used here to set pin numbers:
const int NandGatePin = A4;    
bool isObstacle=false;



void setup() {
  pinMode(NandGatePin, INPUT);
  Serial.begin(9600);
}

void loop() 
{
  // read the state of the pushbutton value:
  isObstacle = digitalRead(NandGatePin);
  Serial.println(isObstacle);
  delay(100);
  

  
}
