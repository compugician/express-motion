                          //brown - GND
const byte STEP_PIN = 12; //black - Input B
const byte DIRECTION_PIN = 11; //white - Input A
const byte ENABLE_PIN = 10; //blue - Enable
const int HLFB_PIN = 2; //green -HLFB //needs to support interrupts (on UNO this means 2 or 3)

const int STEPS_PER_ROTATION = 800; 
const float INCHES_OF_TRAVEL = 24.25;
const float INCHES_PER_ROTATION = 2.0;
const int MAX_PULSE_COUNT = (INCHES_OF_TRAVEL/INCHES_PER_ROTATION*STEPS_PER_ROTATION);

//10 uSec total pulse width = 100khz
const int ON_PULSE_WIDTH_MICROS = 5; 
const int OFF_PULSE_WIDTH_MICROS = 5; 

//CW = move towards motor
//CCW = move away from motor
//motor homes away.

//vars
int destination = 0;
int queuedDestination = 0;
bool newMoveAvailable = false;


void setup() {
  Serial.begin(9600);
  Serial.write("Power On! ");
  Serial.println(MAX_PULSE_COUNT);

  pinMode(STEP_PIN,OUTPUT);
  pinMode(DIRECTION_PIN,OUTPUT); 
  pinMode(ENABLE_PIN,OUTPUT);
  pinMode(HLFB_PIN,INPUT_PULLUP);

  goHome();
}

void goHome() {
  
  Serial.println("Homing");
  destination = 0;
  digitalWrite(ENABLE_PIN,LOW);
  delay(100);
  digitalWrite(ENABLE_PIN,HIGH);
  delay(10);
  while (isInMotion()) {
    //nop
  }
  Serial.println("@home");  
}

boolean isInMotion() {
  return (digitalRead(HLFB_PIN)==HIGH);
}


void setDir(bool cw) {
  if (cw) {
    digitalWrite(DIRECTION_PIN,LOW);
    Serial.println("Setting Direction CW");
  } else {
    digitalWrite(DIRECTION_PIN,HIGH);
    Serial.println("Setting Direction CCW");
  } 
}

void makeAMove() {
  newMoveAvailable = false;
  int pos = destination;    
  destination = queuedDestination;  
  int delta = destination - pos; 
  int pulse_count = abs(delta); 
  
  if (pulse_count>0) {
    Serial.println(pulse_count);
    setDir(delta>0); //if delta is positive, move CW
    for (int i=0; i<pulse_count; i++) {
      digitalWrite(STEP_PIN,HIGH);
      delayMicroseconds(ON_PULSE_WIDTH_MICROS);
      digitalWrite(STEP_PIN,LOW);
      delayMicroseconds(OFF_PULSE_WIDTH_MICROS);
    }  
  }
}

void loop() {

  if (Serial.available() > 0) {
    // look for the next valid integer in the incoming serial stream:
    int input = Serial.parseInt();
    if (input>=0 && input<=MAX_PULSE_COUNT) {
      queuedDestination = input;
      newMoveAvailable = true;
      Serial.print("moving to: ");
      Serial.println(queuedDestination);
    } else {
      Serial.println("Out of bounds.");
      goHome();
    }
  }

  if (!isInMotion() && newMoveAvailable) {
    makeAMove();
  }  
}
