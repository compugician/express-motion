                          //brown - GND
const byte STEP_PIN = 12; //black - Input B
const byte DIRECTION_PIN = 11; //white - Input A
const byte ENABLE_PIN = 10; //blue - Enable
const int HLFB_PIN = 2; //green -HLFB //needs to support interrupts (on UNO this means 2 or 3)

const int STEPS_PER_ROTATION = 800; 
const float INCHES_OF_TRAVEL = 24;
const float INCHES_PER_ROTATION = 2.0;
const int MAX_TRAVEL_INCHES = 25; //32 screw - 5 body
const int PULSE_COUNT = (INCHES_OF_TRAVEL/INCHES_PER_ROTATION*STEPS_PER_ROTATION);

//10 uSec total pulse width = 100khz
const int ON_PULSE_WIDTH_MICROS = 5; 
const int OFF_PULSE_WIDTH_MICROS = 5; 

const int DELAY_BETWEEN_DIRECTIONS_MS = 0;

char inData[20]; // Allocate some space for the string
char inChar=-1; // Where to store the character read
byte index = 0; // Index into array; where to store the character

//CW = move towards motor
//CCW = move away from motor
//motor homes away.

void setup() {
  Serial.begin(9600);
  Serial.write("Power On! ");
  Serial.println(PULSE_COUNT);

  pinMode(STEP_PIN,OUTPUT);
  pinMode(DIRECTION_PIN,OUTPUT); 
  pinMode(ENABLE_PIN,OUTPUT);
  pinMode(HLFB_PIN,INPUT_PULLUP);
  
}

boolean isInMotion() {
  return (digitalRead(HLFB_PIN)==HIGH);
}

void waitTillNoMotion() {
  while (isInMotion()) {
    //nop
  }
}

char Comp(char* This) {
    while (Serial.available() > 0) // Don't read unless
                                   // there you know there is data
    {
        if(index < 19) // One less than the size of the array
        {
            inChar = Serial.read(); // Read a character
            inData[index] = inChar; // Store it
            index++; // Increment where to write next
            inData[index] = '\0'; // Null terminate the string
        }
    }

    if (strcmp(inData,This)  == 0) {
        for (int i=0;i<19;i++) {
            inData[i]=0;
        }
        index=0;
        return(0);
    }
    else {
        return(1);
    }
}

bool moveCCW = false;
bool moveEnabled = false;
void loop() {

  if (moveEnabled) {
    if (moveCCW) {
      digitalWrite(DIRECTION_PIN,HIGH);
      Serial.println("MOVING CCW");
    } else {
      digitalWrite(DIRECTION_PIN,LOW);
      Serial.println("MOVING CW");
    } 
    moveCCW = !moveCCW;
    
    for (int i=0; i<PULSE_COUNT; i++) {
      digitalWrite(STEP_PIN,HIGH);
      delayMicroseconds(ON_PULSE_WIDTH_MICROS);
      digitalWrite(STEP_PIN,LOW);
      delayMicroseconds(OFF_PULSE_WIDTH_MICROS);
    }
    while (isInMotion()) {
      //nop;
    }
    delay(DELAY_BETWEEN_DIRECTIONS_MS);    
  } else {
    delay(50);
  }
  processState();

}

void processState() {
  if (Comp("enable")==0) {
    digitalWrite(ENABLE_PIN,HIGH);
    Serial.println("ENABLED!");
  }
  if (Comp("disable")==0) {
    digitalWrite(ENABLE_PIN,LOW);
    Serial.println("DISABLED!");
  }
  if (Comp("start")==0) {
    if (digitalRead(ENABLE_PIN)==LOW) {
      Serial.println("Homing");
      digitalWrite(ENABLE_PIN,HIGH);
      waitTillNoMotion();
      Serial.println("@Home");
    }
    
    moveEnabled = true;
    moveCCW = false;
  }
  if (Comp("stop")==0) {
    Serial.println("Stopped!");
    moveEnabled = false;
  }

}



//void loop()
//{
//    if (Comp("m1 on")==0) {
//        Serial.write("Motor 1 -> Online\n");
//    }
//    if (Comp("m1 off")==0) {
//        Serial.write("Motor 1 -> Offline\n");
//    }
//}
