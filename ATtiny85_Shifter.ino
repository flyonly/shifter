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
const int buttonUp = 2;           // the number of the pushbutton pin 2
const int buttonDown = 0;         // the number of the pushbutton pin 0
const int del = 5;                // servo refresh delay
const int sup = 180;              // shift up degrees
const int dow = 0;                // shift down degree
const int mid = 90;               // rest degree postition
const int net = 135;              // half shift up to get netrual

// variables will change:
int buttonStateUp = 0;            // variable for reading the pushbutton status
int buttonStateDown = 0;          // variable for reading the pushbutton status
int pos = dow;                    // variable for servo position
int gear = 0;                     // variable for gear position

void setup() {
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 3 to the servo 
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonUp, INPUT);
  digitalWrite(buttonUp,HIGH);
  pinMode(buttonDown, INPUT);
  digitalWrite(buttonDown,HIGH);

  pixels.begin();
  // initialize start position netrual
    down();                         // go down one gear shift 
    delay(500);
    netrual(); 
    delay(500);
    
  // clears NeoPixel
  for(int i=0; i<8 ; i +=1)
    {pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.  
    }
  }
  
void loop() {                     // Loop through motion tests
  // read the state of the pushbutton value:
  buttonStateUp = digitalRead(buttonUp);
  buttonStateDown = digitalRead(buttonDown);

  // check if both pushbutton are pressed. to do the netrual shift 
    while ((buttonStateUp == LOW) && (buttonStateDown == LOW)){
    down();                       // down to first
    netrual();                    // tell servo to go to netrual position
    delay(1000);
    buttonStateUp = HIGH;
    buttonStateDown = HIGH;
   } 
  
  // check if the pushbutton is pressed. If it is, the buttonStateUp is HIGH:
  if (buttonStateUp == ACTIVATED) {
    up();                         // gear shift up
        if (gear == 0 ) gear = 1;  
        if (gear == 6 ) gear = 5;         
    gear += 1;                    // increase gear number by one
    pixels.setPixelColor(gear-1, pixels.Color(255,0,0));     //(R,G,B)
    pixels.show();                // This sends the updated pixel color to the hardware.
    }
    
  if (buttonStateDown == ACTIVATED) {
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
  } 
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
