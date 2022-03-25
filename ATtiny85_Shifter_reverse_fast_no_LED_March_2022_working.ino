// ATtiny85 Bike Shifter Dual Button
// Up button - up one gear, Down button - down one gear.  Long push on down button when in
// first gear it will half shift up a gear to get to netural.
// pinMode(1, OUTPUT); //LED on Model A  or Pro

#include <SoftRcPulseOut.h> 

SoftRcPulseOut myservo;                                     // Create servo object to control a servo 
                        
#define SERVO_PIN          3                                // Attiny 85 pin output 
#define REFRESH_PERIOD_MS 10                                // Servo refresh
#define NOW                1                                // Servo pulse
#define SPEED              10                               // Move servo fast till this many degrees before limit

/////////Declare and Initialize Variables//////////////////////////// 
//We need to track how long the momentary pushbutton is held for in order to exectute different commands 
//This value will be recorded in seconds 
int pressLength_milliSeconds = 0; 

// Define the *minimum* length of time, in milli-¬‐seconds, that the button must be pressed for a particular option to occur 
byte optionOne_milliSeconds = 25; 
int optionTwo_milliSeconds = 1000;    

// Button Setup
byte buttonUp = 2;                                          // Attiny85 pushbuttonUp pin
byte buttonDown = 0;                                        // Attiny85 pushbuttonDown pin

// Setting initial variables or variable type
byte del = 50;                                              // servo pause because the Attiny is faster than the servo
byte dow =10;                                                  // shift down degree
byte sup =200;                                                  // shift up degrees
byte net = 130;                                                  // half shift up to get netrual
byte correction = 10;                                        // adjustment for backlash
byte mid = 95;                                              // rest degree postition
byte address = 0;                                           // eeprom address location 
byte pos = mid;                                             // variable for servo position
int gear = 0;                                               // variable for gear position
byte degree = 5;                                            // degree of movement per cycle
byte wait = 400;
int milliSecPress = millis();
 
void setup() {
  myservo.attach(SERVO_PIN);                                // Attaches the servo on pin 3 to the servo 
  
// Initialize the pushbutton pin as an input:
  pinMode(buttonUp, INPUT_PULLUP); 
  pinMode(buttonDown, INPUT_PULLUP);
 
  delay(100);                                               // starts to quick, delay to allow the timer to start  
}

void loop() {     
                                           
// Read the state of the down pushbutton value:
//Record *roughly* the of tenths of seconds the button in being held down 

  while (digitalRead(buttonDown) == LOW ){delay(100);       // if you want more resolution, lower this number  
    pressLength_milliSeconds = pressLength_milliSeconds + 100;    
  }

//Different if-¬‐else conditions are triggered based on the length of the button press 
//Start with the longest time option first 

//Option 2 -¬‐ Execute the second option if the button is held for the correct amount of time 
  if ((pressLength_milliSeconds >= optionTwo_milliSeconds) && ( gear ==1 )){ 
    netrual();                                              // tell servo to go to netrual sequence 
  pressLength_milliSeconds = 0;                             //Every time through the loop, we need to reset the pressLength_Seconds counter
  }
    
//Option 1 -¬‐ Execute the first option if the button is held for the correct amount of time 
  else if(pressLength_milliSeconds >= optionOne_milliSeconds){ 
   if (gear >1){
      gear -= 1;                                            // decrease gear number by one
      down();                                               // gear shift down                                     // Show on Neoplixel  
   }
    else if (gear == 0 ){
      gear = 1;
      down();                                               // gear shift down
      }                                                     // If gear = 1 then do nothing
  pressLength_milliSeconds = 0;  
  }                                                         //close if options 

// Read the state of the down pushbutton value:
                                                            //Record *roughly* the of tenths of seconds the button in being held down 
  while (digitalRead(buttonUp) == LOW ){delay(100);         //if you want more resolution, lower this number  
    pressLength_milliSeconds = pressLength_milliSeconds + 100;    
  }
   if(pressLength_milliSeconds >= optionOne_milliSeconds){ 
    if (gear == 0 ){
      gear = 2; 
      up();                                                 // gear shift up  
    }
    else if (gear < 7){
      gear += 1;                                            // increase gear number by one
      up();                                                 // gear shift up
      if (gear == 7){                                       // allow the gears to go above 6 to allow for missing an up gear to allow it to get to 6th
        gear =6;
        } 
      }
  pressLength_milliSeconds = 0;                             //Every time through the loop, we need to reset the pressLength_Seconds counter 
  } 
}

// Motion routines for up, down and netrual

// Gear shift up
void up() {                 
  for(pos =mid; pos <= sup; pos += degree)                  // goes from mid to up 
  {                                                         // in steps of degree set in variables 
    if((sup-pos)>SPEED) pos++;                              // move faster if not near end point
    motion(pos);
  } 
  for(pos = sup; pos >= mid; pos -= degree)                 // goes from up to up to mid 
    {                                                       // in steps of degree set in variables  
    if((pos-mid)>SPEED) pos--;                              // move faster if not near end point
    motion(pos);
    }
   delay(wait);                                             // debounce button push
}

// Gear shift down
void down(){          
  for(pos = mid; pos>=dow; pos-=degree)                     // goes from mid to down   
  {                                                         // in steps of 1 degree                                 
    if((pos-dow)>SPEED) pos--;                              // move faster if not near end point
    motion(pos);
  }      
  for(pos = dow; pos<=mid; pos+=degree)                     // goes from down to mid  
  {                                                         // in steps of 1 degree     
    if((mid-pos)>SPEED) pos++;                              // move faster if not near end point
    motion(pos);
  }    
  delay(wait);                                              // debounce button push
}

void netrual() {                                            //
  for(pos = mid; pos <= net; pos += 1)                      // goes from mid to netrual 
    {                                                       // in steps of 1 degree so moves slow 
      motion(pos);
    }           
  delay (500);
  for(pos = net ; pos >= (mid-correction); pos -= 1)        // goes netrual to mid with backlash allowance 
    {                                                       // in steps of 1 degree 
      motion(pos);
    } 
  gear = 0;
}

void motion(uint8_t p){ 
       myservo.write(p);                                    // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);                            // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);                        // generates the servo pulse Now  
}
      
// routines to use for debugging.  Can use it to blink different counts at different parts of the script 
// with onboard led
#define DEBUG 1                                             // Set to 1 to enable, 0 to disable
 
#if DEBUG
#define DebugPin 1                                          // Digispark model A onboard LED
#define DebugBlink 100
#define DebugPause 300
#define debugDelay(ms) delay(ms)                            // Change if you need a special delay function (e.g. if you use libraries that need regular refresh calls)
 
void _debugBlink(int n) {
  for ( int i = 0 ; i < n ; i++ ) {
    digitalWrite(DebugPin, HIGH);
    debugDelay(DebugBlink);
    digitalWrite(DebugPin, LOW);
    debugDelay(DebugBlink);
  }
  debugDelay(DebugPause);
}
 
void _debugSetup() {
  pinMode(DebugPin, OUTPUT);
}
#define debugBlink(n) _debugBlink(n)                        // Do the blink when DEBUG is set
#define debugSetup() _debugSetup()
#else
#define debugBlink(n)                                       // Make the calls disappear when DEBUG is 0
#define debugSetup()
#endif
