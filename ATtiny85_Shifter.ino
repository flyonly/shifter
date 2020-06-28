// ATtiny85 Bike Shifter Dual Button with NeoPixel gear indicator
// White NeoPixel is white if you long press on a button it enters throw tuning
// Down, Netural then Up.  A blue Neopixel will show status.  Up or down will adjust by
// 1 degree.  After 5 sec no button then it moves to the next tuning.  It will clear 
// Neopixel and goto operation mode.  Settings are written to EEprom
// Up button - up one gear, Down button - down one gear.  Long push on down button when in
// first gear it will half shift up a gear to get to netural.
// No led to indicate you are in netrual
// pinMode(1, OUTPUT); //LED on Model A  or Pro

#include <SoftRcPulseOut.h> 
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

SoftRcPulseOut myservo;  // create servo object to control a servo 
                        
#define SERVO_PIN      3          // attiny 85 pin output 
#define PIN            4          // NeoPixel pin number
#define NUMPIXELS      8          // NeoPixel Pixels
#define REFRESH_PERIOD_MS 3       // servo refresh
#define NOW            1          // servo pulse

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

/////////Declare and Initialize Variables//////////////////////////// 
//We need to track how long the momentary pushbutton is held for in order to exectute different commands 
//This value will be recorded in seconds 
int pressLength_milliSeconds = 0; 
// Define the *minimum* length of time, in milli-¬‐seconds, that the button must be pressed for a particular option to occur 
byte optionOne_milliSeconds = 25; 
int optionTwo_milliSeconds = 1000;    
// Button Setup
byte buttonUp = 2;                // Attiny85 pushbuttonUp pin
byte buttonDown = 0;              // Attiny85 pushbuttonDown pin

// Setting initial variables or variable type
byte del = 50;                    // servo pause because the Attiny is faster than the servo
byte dow ;                        // shift down degree
byte sup ;                        // shift up degrees
byte net ;                        // half shift up to get netrual
byte mid = 90;                    // rest degree postition
byte address = 0;                 // eeprom address location 
int pos = mid;                    // variable for servo position
int gear = 0;                     // variable for gear position
int milliSecPress = millis();

void setup() {
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 3 to the servo 
  
// Initialize the pushbutton pin as an input:
  pinMode(buttonUp, INPUT_PULLUP); 
  pinMode(buttonDown, INPUT_PULLUP);

// Read previous position settings from EEprom 
  dow = EEPROM.read(address);
  sup = EEPROM.read(address+1);    
  net = EEPROM.read(address+2);   
// Uncomment to reset inital positions
/* 
  EEPROM.write(address, 20);  //dow 
  EEPROM.write(address+1, 160); //sup 
  EEPROM.write(address+2, 135); //net  
*/  
  pixels.begin();       // Turn on Neopixels
  colorWipe(pixels.Color(255, 0, 0), 50); // Red
  colorWipe(pixels.Color(0, 255, 0), 50); // Green
  colorWipe(pixels.Color(0, 0, 255), 50); // Blue
  nopixels();           // Write (0,0,0) to all Neopixels

// Read the state of the down pushbutton value:
//Record *roughly* the of tenths of seconds the button in being held down 
  while (digitalRead(buttonDown) == LOW ||digitalRead(buttonUp) == LOW ){
    delay(100);  //if you want more resolution, lower this number  
    pressLength_milliSeconds = pressLength_milliSeconds + 100;    
  } 
// If button press is greater than 1000 mills then enter setting movement positions
  if ((pressLength_milliSeconds >= optionTwo_milliSeconds) ){ 
    centre ();                                              // Start in mid position
  
// Adjust Down Position
  pixels.setPixelColor(0, pixels.Color(0,0,155));           // (R,G,B)
  pixels.show();                                            // Initialize all pixels
  milliSecPress = millis();                                 // Reset last button press time to equal now
  for(pos = mid; pos >= dow; pos -= 1)                      // Move from 90 degrees to 0 degrees
    {                                
    motion(pos);
    } 
  while (millis()-milliSecPress<5000){                      // Do until button is not pressed in 5000 mills
    if (digitalRead(buttonDown) == LOW) {                   // Move down 1 degree
      dow -= 1;
    if (dow == 0){
      dow = 1;
      }
    milliSecPress = millis();
    } 
    else{
      if (digitalRead(buttonUp) == LOW) {                   // Move up 1 degree
        dow += 1;      
        milliSecPress = millis();
        }
    }
    motion(dow);
    delay(50);
    } 
  nopixels();                                               // Clear all Neopixels 
  for(pos = net; pos >= mid; pos -= 1)                      // Move from 90 degrees to 135 degrees
    {                                
    motion(pos);
    }
  
//Set Net Position
  pixels.setPixelColor(3, pixels.Color(0,0,155));           // (R,G,B)
  pixels.show();                                            // Show Neopixels
  milliSecPress = millis();                                 // Reset last button press time to equal now
  while (millis()-milliSecPress<5000){                      // Do until button is not pressed in 5000 mills
        if (digitalRead(buttonDown) == LOW) {
           net -= 1;                                        // Move down 1 degree
           milliSecPress = millis();                        // Reset last button press time to equal now
        }
      if (digitalRead(buttonUp) == LOW) {                   // Move up 1 degree
          net += 1;      
         milliSecPress = millis();
        }
      motion(net);
      delay(50); 
      }
    nopixels();
       for(pos = net; pos <= sup; pos += 1)                 // Moves from 135 degrees to 180 degrees
     {                                
      motion(pos);
     }   

//Set Up Position
    pixels.setPixelColor(6, pixels.Color(0,0,155));         //(R,G,B)
    pixels.show();                                          // Show Neopixels
    milliSecPress = millis();                               // Reset last button press time to equal now
    while (millis()-milliSecPress<5000){                    // Do until button is not pressed in 5000 mills
      if (digitalRead(buttonDown) == LOW) {
        sup -= 1;                                           // Move down 1 degree
        milliSecPress = millis();
      }
      if (digitalRead(buttonUp) == LOW) {
        sup += 1;                                           // Move up 1 degree      
        milliSecPress = millis();
      }
      motion(sup);
      delay(50); 
    } 
        centre();                                               // Set position to midpoint
        nopixels();
  }

    pressLength_milliSeconds = 0;
    EEPROM.update(address, dow);                            // Write new positions to EEprom if they have changed
    EEPROM.update(address+1, sup);
    EEPROM.update(address+2, net); 
}

void loop() {                                              
// Read the state of the down pushbutton value:
//Record *roughly* the of tenths of seconds the button in being held down 
  while (digitalRead(buttonDown) == LOW ){delay(100);       //if you want more resolution, lower this number  
    pressLength_milliSeconds = pressLength_milliSeconds + 100;    
  }

//Different if-¬‐else conditions are triggered based on the length of the button press 
//Start with the longest time option first 
//Option 2 -¬‐ Execute the second option if the button is held for the correct amount of time 
  if ((pressLength_milliSeconds >= optionTwo_milliSeconds) && ( gear ==1 )){ 
    netrual();                                              // tell servo to go to netrual sequence
  } 
//Option 1 -¬‐ Execute the first option if the button is held for the correct amount of time 
  else if(pressLength_milliSeconds >= optionOne_milliSeconds){ 
    down();                                                 // gear shift down
    gear -= 1;                                              // decrease gear number by one
    if (gear == -1 ){
      gear = 1;
      showgear(gear-1);                                     // Show on Neoplixel       
      }
      if (gear == 0 ){
        gear = 1;       
        showgear(gear-1);         
      }
      else {
        hidegear(gear);
        showgear(gear-1);
      }
  }//close if options 

  if (digitalRead(buttonUp) == LOW) {  
    up();                                                   // gear shift up
    if (gear == 0 ){
      gear = 1; 
      showgear(gear-1);  
    }
    if (gear == 6 ) gear = 5;         
      gear += 1;                                            // increase gear number by one
      showgear(gear-1);
  } 
 
//Every time through the loop, we need to reset the pressLength_Seconds counter 
  pressLength_milliSeconds = 0;  
}

// Motion routines for up, down and netrual
// Gear shift up
void up() {                 
  for(pos =mid; pos <= sup; pos += 1)                      // goes from 90 degrees to 180 degrees 
  {                                                         // in steps of 1 degree 
    motion(pos);
  } 
  for(pos = sup; pos >= mid; pos -= 1)                      // goes from 180 degrees to 90 degrees 
    {                                                       // in steps of 1 degree 
    motion(pos);
    }            
}

// Gear shift down
void down() {                    
  for(pos = mid; pos>=dow; pos-=1)                          // goes from 90 degrees to 0 degrees   
  {                                                         // in steps of 1 degree                                 
    motion(pos);
    if (pos<5) delay(del);                                  // slow down at close to full through so servo can keep up
  }      
  for(pos = dow; pos<=mid; pos+=1)                          // goes from 0 degrees to 90 degrees  
  {                                                         // in steps of 1 degree                                 
    motion(pos);
  }
}

void centre() {
  for(pos = sup; pos >= mid; pos -= 1)                      // goes from 180 degrees to 90 degrees 
  {                                                         // in steps of 1 degree 
    motion(pos);
  }           
}
   
void netrual() {
  for(pos = mid; pos <= net; pos += 1)                     // goes from 90 degrees to 135 degrees 
    {                                                      // in steps of 1 degree 
      motion(pos);
    }           
  delay (500);
  for(pos = net ; pos >= mid; pos -= 1)                    // goes from 135 degrees to 90 degrees 
    {                                                      // in steps of 1 degree 
      motion(pos);
    } 
  gear = 0;
  nopixels();
}

void motion(uint8_t p){ 
       myservo.write(p);                                   // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);                           // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);                       // generates the servo pulse Now  
}
      
void showgear(uint8_t g){
    pixels.setPixelColor(g, pixels.Color(155,0,0));        //(R,G,B)
    pixels.show();                                         // This sends the updated pixel color to the hardware.
}

void hidegear(uint8_t g){
    pixels.setPixelColor(g, pixels.Color(0,0,0));          //(R,G,B)
    pixels.show();                                         // This sends the updated pixel color to the hardware.
}
      
void nopixels(){
  for(uint8_t i=0; i<8 ; i +=1){
    pixels.setPixelColor(i, pixels.Color(0,0,0));          //(R,G,B)
    pixels.show();                                         // This sends the updated pixel color to the hardware.         
  }
}
 
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, c);
      pixels.show();
      delay(wait);
  }
}
// routines to use for debugging.  Can use it to blink different counts at different parts of the script 
// with onboard led
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
