 // ATtiny85 Bike Shifter Dual Button with NeoPixel gear indicator
// NeoPixel will indicate Green at start up and bike should then be in Netrual
// Up button - up one gear, Down button - down one gear.  Both buttons the unit
// will go down one gear then back up half a gear to get to netural.  So this should
// work from 1st or 2nd.  Havent yet tried it on a motorcycle.  It will then show
// green to indicate you are in netrual
//   pinMode(1, OUTPUT); //LED on Model A  or Pro

#include <SoftRcPulseOut.h> 
#include <Adafruit_NeoPixel.h>

SoftRcPulseOut myservo;  // create servo object to control a servo 
                        // a maximum of eight servo objects can be created 
                        
#define SERVO_PIN      3
#define REFRESH_PERIOD_MS 3
#define NOW               1
#define PIN            4          // NeoPixel pin number
#define NUMPIXELS      8          // NeoPixel Pixels

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define ACTIVATED LOW

/////////Declare and Initialize Variables//////////////////////////// 
//We need to track how long the momentary pushbutton is held for in order to exectute different commands 
//This value will be recorded in seconds 
float pressLength_milliSeconds = 0; 
// Define the *minimum* length of time, in milli-¬‐seconds, that the button must be pressed for a particular option to occur 
int optionOne_milliSeconds = 25; 
int optionTwo_milliSeconds = 1000;    
// Button Setup
const int buttonUp = 2;           // the number of the pushbutton pin 2
const int buttonDown = 0;         // the number of the pushbutton pin 0
// Servo Setup
const int del = 5;                // servo refresh delay
const int sup = 180;              // shift up degrees
const int dow = 0;                // shift down degree
const int mid = 90;               // rest degree postition
const int net = 135;              // half shift up to get netrual

// variables will change:
int pos = dow;                    // variable for servo position
int gear = 0;                     // variable for gear position

void setup() {
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 3 to the servo 
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonUp, INPUT_PULLUP); 
  pinMode(buttonDown, INPUT_PULLUP);

//  pinMode (1, OUTPUT);
//  digitalWrite(1, HIGH); // sets the digital pin 1 to 5v for Neopixel

  pixels.begin();   
  // clears NeoPixel
  for(int i=0; i<8 ; i +=1)
    {pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.  
    }
  }
  
void loop() {                     // Loop through motion tests
  // read the state of the pushbutton value:

//Record *roughly* the of tenths of seconds the button in being held down 
  while (digitalRead(buttonDown) == LOW ){delay(100);  //if you want more resolution, lower this number  
    pressLength_milliSeconds = pressLength_milliSeconds + 100;    
  }//close while 

//Different if-¬‐else conditions are triggered based on the length of the button press 
//Start with the longest time option first 
 //Option 2 -¬‐ Execute the second option if the button is held for the correct amount of time 
if ((pressLength_milliSeconds >= optionTwo_milliSeconds) && ( gear ==1 )){ 
        netrual();                    // tell servo to go to netrual position
  delay(500);
  } 
//option 1 -¬‐ Execute the first option if the button is held for the correct amount of time 
  else if(pressLength_milliSeconds >= optionOne_milliSeconds){ 
        down();                       // gear shift down
    gear -= 1;                    // decrease gear number by one
       if (gear == -1 ){
        gear = 1;
        pixels.setPixelColor(gear-1, pixels.Color(255,0,0));  //(R,G,B)
        pixels.show();            // This sends the updated pixel color to the hardware.         
       }
       if (gear == 0 ){
        gear = 1;       
        pixels.setPixelColor(gear-1, pixels.Color(255,0,0));  //(R,G,B)
        pixels.show();            // This sends the updated pixel color to the hardware.          
       }
       else {
    pixels.setPixelColor(gear, pixels.Color(0,0,0));          //(R,G,B)
    pixels.show();                // This sends the updated pixel color to the hardware.    
       }

  }//close if options 

   if (digitalRead(buttonUp) == LOW) {  
       up();                         // gear shift up
        if (gear == 0 ) gear = 1;  
        if (gear == 6 ) gear = 5;         
    gear += 1;                    // increase gear number by one
    pixels.setPixelColor(gear-1, pixels.Color(255,0,0));     //(R,G,B)
    pixels.show();                // This sends the updated pixel color to the hardware.
   } 
 
  //every time through the loop, we need to reset the pressLength_Seconds counter 
  pressLength_milliSeconds = 0;  
  //       _debugBlink(gear);
}

// Motion routines for up, down and netrual

// Gear shift up
void up() {                       
   for(pos = mid; pos <= sup; pos += 1)      // goes from 90 degrees to 180 degrees 
     {                                       // in steps of 1 degree 
       myservo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);        // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);    // generates the servo pulse Now
     } 
   for(pos = sup; pos >= mid; pos -= 1)      // goes from 180 degrees to 90 degrees 
     {                                       // in steps of 1 degree 
       myservo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);        // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);    // generates the servo pulse Now
     }           
}

// Gear shift down
void down() {                       
   for(pos = mid; pos>=dow; pos-=1)  
     {                                
       myservo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);        // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);    // generates the servo pulse Now
     }      
   for(pos = dow; pos<=mid; pos+=1)  
     {                                
       myservo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);        // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);    // generates the servo pulse Now
     }
  } 

void centre() {
   for(pos = dow; pos <= mid; pos += 1)    // goes from 90 degrees to 0 degrees 
     {                                     // in steps of 1 degree 
       myservo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);        // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);    // generates the servo pulse Now
     }           
  }
    
void netrual() {
   for(pos = mid; pos <= net; pos += 1)        // goes from 90 degrees to 135 degrees 
     {                                         // in steps of 1 degree 
       myservo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);        // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);    // generates the servo pulse Now
     }           
     delay (500);
   for(pos = net ; pos >= mid; pos -= 1)       // goes from 135 degrees to 90 degrees 
     {                                         // in steps of 1 degree 
       myservo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);        // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);    // generates the servo pulse Now
     } 
     gear = 0;
   for(int i=0; i<7 ; i +=1)
    {pixels.setPixelColor(i, pixels.Color(0,255,0));  //(R,G,B)
    pixels.show();                             // This sends the updated pixel color to the hardware.  
    }
    delay(1000);
    for(int i=0; i<7 ; i +=1)
    {pixels.setPixelColor(i, pixels.Color(0,0,0));  //(R,G,B)
    pixels.show();                             // This sends the updated pixel color to the hardware.  
    }         
}


#define DEBUG 1  // Set to 1 to enable, 0 to disable
 
#if DEBUG
#define DebugPin 1  // Digispark model A onboard LED
#define DebugBlink 100
#define DebugPause 300
#define debugDelay(ms) delay(ms)  // Change if you need a special delay function (e.g. if you use libraries that need regular refresh calls)
 
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
 
#define debugBlink(n) _debugBlink(n)  // Do the blink when DEBUG is set
#define debugSetup() _debugSetup()
#else
#define debugBlink(n)  // Make the calls disappear when DEBUG is 0
#define debugSetup()
#endif
