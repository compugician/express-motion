const byte STEP_PIN = 12;
const byte DIRECTION_PIN = 11;
const int STEPS_PER_ROTATION = 800;
const float INCHES_OF_TRAVEL = 26.5;
const float INCHES_PER_ROTATION = 2.0;
const int MAX_TRAVEL_INCHES = 27; //32 screw - 5 body

//10 uSec total pulse width = 100khz
const int ON_PULSE_WIDTH_MICROS = 5; 
const int OFF_PULSE_WIDTH_MICROS = 5; 

const int DELAY_BETWEEN_DIRECTIONS_MS = 600;

char inData[20]; // Allocate some space for the string
char inChar=-1; // Where to store the character read
byte index = 0; // Index into array; where to store the character

//CW = move towards motor
//CCW = move away from motor
//motor homes away.

void setup() {
  Serial.begin(9600);
  Serial.write("Power On");

  pinMode(STEP_PIN,OUTPUT);
  pinMode(DIRECTION_PIN,OUTPUT); 
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
bool enabled = false;
void loop() {

  if (enabled) {
    if (moveCCW) {
      digitalWrite(DIRECTION_PIN,HIGH);
    } else {
      digitalWrite(DIRECTION_PIN,LOW);
    } 
    moveCCW = !moveCCW;
    
    for (int i=0; i<(INCHES_OF_TRAVEL/INCHES_PER_ROTATION*STEPS_PER_ROTATION); i++) {
      digitalWrite(STEP_PIN,HIGH);
      delayMicroseconds(ON_PULSE_WIDTH_MICROS);
      digitalWrite(STEP_PIN,LOW);
      delayMicroseconds(OFF_PULSE_WIDTH_MICROS);
    }
    delay(DELAY_BETWEEN_DIRECTIONS_MS);    
    if (Comp("stop")==0) {
      enabled = false;
    }
  } else {
    if (Comp("start")==0) {
      enabled = true;
      moveCCW = false;
    }
    delay(50);
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
