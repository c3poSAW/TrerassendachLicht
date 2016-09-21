#include "TimerOne.h"  
  int iModi = 0;

  
// READ Poti ////////////////////////////////////////////////////////////////////////
  const int numReadings = 10;
  int readings[numReadings];      // the readings from the analog input
  int readIndex = 0;              // the index of the current reading
  int total = 0;                  // the running total
  int PotiAvr = 0;                // the Avarage Value of Poti
// READ Poti ////////////////////////////////////////////////////////////////////////  

// Debounce Pins// Include the Bounce2 library found here :///////////////////////////
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring
#include <Bounce2.h>
Bounce BU02 = Bounce(); 
Bounce BU03 = Bounce(); 
Bounce BU04 = Bounce(); 
// Debounce Pins// Include the Bounce2 library found here :///////////////////////////

    
// PWM Pins //////////////////////////////////////////////////////////////////////  
// typedef for properties of each sw pwm pin
typedef struct pwmPins {
  int pin;
  int pwmValue;
  bool pinState;
  int pwmTickCount;
} pwmPin;
 
// create the sw pwm pins
// these can be any I/O pin
// that can be set to output!
const int pinCount = 7; //Total used PWM Pins
const byte pins[pinCount] = {12,11,10,9,8,7,6}; // Used PWM Pins
//byte lPins[pinCount]  = {12,11,10,9,8,7,6}; // Used for Lauflicht
pwmPin myPWMpins[pinCount];
int iMaxSteps = 32; //Maximum of singel Timeintervalls
int iCounter = 0;
bool PWMActive = true;
// PWM Pins //////////////////////////////////////////////////////////////////////  

byte akPin = 0; 
bool StepBegin = false;
bool StepEnd = false;
     

     
void setup()   {
      // needed for PWM   ////////////////////////////
                   Timer1.initialize(500);
                   Timer1.attachInterrupt(fucPWM);
      // needed for PWM   ////////////////////////////  
    
    Serial.begin(9600);

   setupPWMpins();
    pinMode(13, OUTPUT);
    pinMode(5, OUTPUT);
 
    digitalWrite(5, HIGH); // Allways on

    pinMode(2,INPUT_PULLUP);
    BU02.attach(2);
    BU02.interval(15);
  
    pinMode(3,INPUT_PULLUP);
    BU03.attach(3);
    BU03.interval(15);
  
    pinMode(4,INPUT_PULLUP);
    BU04.attach(4);
    BU04.interval(15);
}
     
void loop()    {
    BU02.update();
    BU03.update();
    BU04.update();

 Serial.print("    ");
 Serial.print("    ");
 Serial.println(iModi);

  switch (iModi) {
    case 0:  
      //startup
      for (int iDurchlauf=0; iDurchlauf < 1; iDurchlauf++) {
         for (int index=0; index < pinCount; index++) {
           myPWMpins[index].pwmValue   = iMaxSteps;
           delay(250);
        }

        for (int index=pinCount; index >= 0; index--) {
           myPWMpins[index].pwmValue   = 0;
           delay(250);
        }
      }
      iModi = 1 ;
      break;
    case 1:
      //Dimming by Analog Input
      PWMActive = true;
      setAllpinsToOneValue(PotiAvr/4);
      break;
    case 2:
      //Maximum on with PWM
      PWMActive = true;
      setAllpinsToOneValue(iMaxSteps);
      //delay(15000);
      break;
    case 3:
      //Maximum on without PWM
      PWMActive = false;
      SetAllPins(true);
      break;
    case 4:
      PWMActive = false;
       for (int index=0; index < pinCount; index++) {
         digitalWrite(myPWMpins[index].pin, HIGH);
         delay(400);
         digitalWrite(myPWMpins[index + 1].pin, HIGH);
         delay(400);
         digitalWrite(myPWMpins[index].pin, LOW);
         delay(400*2);
      }
      for (int index=pinCount; index > 0; index--) {
         digitalWrite(myPWMpins[index].pin, HIGH);
         delay(400);
         digitalWrite(myPWMpins[index - 1].pin, HIGH);
         delay(400);
         digitalWrite(myPWMpins[index].pin, LOW);
         delay(400*2);
      }
    break;
    case 5:
      //Zoom
      PWMActive = true;
      for (int index=0; index <= iMaxSteps; index++) {
        setAllpinsToOneValue(index);
        delay(1000);
      }
      break;


  
    default: 
      // if nothing else matches, do the default
      // default is optional
      setAllpinsToOneValue(iMaxSteps);
  }

    if (BU04.fell()) {StepEnd = true;}
    
    if (StepBegin){ 
         StepBegin = false;
         SetAllPins(false);
         }
    
    if (StepEnd){ 
         iModi = iModi + 1;
         StepEnd = false;
         StepBegin = true;
         }
    
    if (iModi == 6) {iModi = 1;}
 
  delay(200);

 //Create a flashing on LED to see that the Programm is running
 digitalWrite(13, millis()/1000 % 2);


 
ReadPoti();
    
    }

void fucPWM() {
  if (PWMActive) {
    if (iCounter == iMaxSteps) {iCounter = 0;} 
    iCounter = iCounter +1;
    for (int index=0; index < pinCount; index++) {  //für jeden PIN
        if (iCounter <= myPWMpins[index].pwmValue){ //as long Counter is smaller switch on
          if (myPWMpins[index].pinState == 0){   //Switch it only on if it is off
                         digitalWrite(myPWMpins[index].pin, 1);   // sets the pin as 1
                         myPWMpins[index].pinState = 1;
          }
        }
        else
        {
           if (myPWMpins[index].pinState == 1){   //Switch it only off if it is on
                         digitalWrite(myPWMpins[index].pin, 0);   // sets the pin as 1
                         myPWMpins[index].pinState = 0;
         }
       }
    }
  }
}
   

void ReadPoti()
{

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(A7);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  if (2 < abs(PotiAvr - (total / numReadings))){
    
    // calculate the PotiAvr:
  PotiAvr = total / numReadings;
  // send it to the computer as ASCII digits
 }
}
////////////////////////////////////////////////////////////////////    

void setupPWMpins() {
  for (int index=0; index < pinCount; index++) {
    myPWMpins[index].pin = pins[index];
    myPWMpins[index].pinState = 0;
    pinMode(pins[index], OUTPUT);
  }
}

void setAllpinsToOneValue(int iValue) {
  for (int index=0; index < pinCount; index++) {
    myPWMpins[index].pwmValue = iValue;
  }
}

void SetAllPins(bool bValue) {
  for (int index=0; index < pinCount; index++) {
        digitalWrite(myPWMpins[index].pin, bValue);   
        myPWMpins[index].pinState = bValue;
  }
}

void Lauflicht (){


  
}


//////////////////////////////////////////////////////////////////////////////// 


