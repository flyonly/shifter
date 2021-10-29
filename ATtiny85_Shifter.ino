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

SoftRcPulseOut myservo;                                     // Create servo object to control a servo 
                        
#define SERVO_PIN          3                                // Attiny 85 pin output 
#define PIN                4                                // NeoPixel pin number
#define NUMPIXELS          8                                // NeoPixel Pixels
#define REFRESH_PERIOD_MS 10                                // Servo refresh
#define NOW                1                                // Servo pulse
#define SPEED              10                               // Move servo fast till this many degrees before limit

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

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
byte dow ;                                                  // shift down degree
byte sup ;                                                  // shift up degrees
byte net ;                                                  // half shift up to get netrual
byte correction = 5;                                        // adjustment for backlash
byte mid = 90;                                              // rest degree postition
byte address = 0;                                           // eeprom address location 
byte pos = mid;                                             // variable for servo position
int gear = 0;                                               // variable for gear position
byte degree = 1;                                            // degree of movement per cycle
int milliSecPress = millis();

// The numbers 0-9 in Morse code  
char *numbers[] = {                                         
  "-----", ".----", "..---", "...--", "....-", ".....", "-....", 
  "--...", "---..", "----."
};
 
byte dot_duration = 200;
bool done = false;
   
void setup() {
  myservo.attach(SERVO_PIN);                                // Attaches the servo on pin 3 to the servo 
  
// Initialize the pushbutton pin as an input:
  pinMode(buttonUp, INPUT_PULLUP); 
  pinMode(buttonDown, INPUT_PULLUP);
  

// Read previous position settings from EEprom 
  dow = EEPROM.read(address);
  sup = EEPROM.read(address+1);    
  net = EEPROM.read(address+2);   
// Uncomment to reset inital positions

/*
  EEPROM.write(address,  50);                               //dow 
  EEPROM.write(address+1, 131);                             //sup 
  EEPROM.write(address+2, 115);                             //net  
*/
 
  pixels.begin();                                           // Turn on Neopixels
  
// Read the state of the down pushbutton value:
// Record *roughly* the of tenths of seconds the button in being held down 
  while (digitalRead(buttonDown) == LOW ||digitalRead(buttonUp) == LOW ){
    delay(100);                                             //if you want more resolution, lower this number  
    pressLength_milliSeconds = pressLength_milliSeconds + 100;    
    } 
// If button press is greater than 1000 mills then enter setting movement positions
  if ((pressLength_milliSeconds >= optionTwo_milliSeconds) ){ 
    centre ();                                              // Start in mid position
    delay(500);
// Adjust Down Position
  for(pos = mid; pos>=dow; pos-=1)                          // goes from mid to down   
  {                                                         // in steps of 1 degree                                 
    motion(pos);
  }                                                         // Move to down position
  pixels.setPixelColor(0, pixels.Color(0,0,155));           // (R,G,B)
  pixels.show();                                            // Initialize all pixels
  milliSecPress = millis();                                 // Reset last button press time to equal now
  while (millis()-milliSecPress<5000){                      // Do until button is not pressed in 5000 mills
    if (digitalRead(buttonDown) == LOW) {                   // Move down 1 degree to adjust down position
      dow -= 1;
    if (dow == 1){                                          // dont move past max position
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
    motion(dow);                                            // Move servo
    delay(50);
  } 
  nopixels();                                               // Clear all Neopixels                                
  for(pos = dow; pos <= net; pos += 1)                      // goes from mid to netrual 
    {                                                       // in steps of 1 degree 
      motion(pos);                                          // Move servo
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
      motion(net);                                          // Move servo
      delay(50); 
      }
    nopixels();
  for(pos = net; pos <= sup; pos += 1)                      // goes from mid to up 
  {                                                         // in steps of 1 degree 
    motion(pos);                                            // Move to Up
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
      motion(sup);                                          // Move servo
      delay(50); 
     } 
    nopixels();
    centre();                                               // Set position to midpoint      
    pressLength_milliSeconds = 0;
    EEPROM.update(address, dow);                            // Write new positions to EEprom if they have changed
    EEPROM.update(address+1, sup);
    EEPROM.update(address+2, net);  
    morse(dow);                                             // Show adjusted postion in morse - 3 digits     
    morse(sup);                                             // Show adjusted postion in morse - 3 digits
    morse(net);                                             // Show adjusted postion in morse - 3 digits       
  }
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
      down();                                               // gear shift down
      hidegear(gear);                                       // Show on Neoplixel  
   }
    else if (gear == 0 ){
      gear = 1;
      down();                                               // gear shift down
      showgear(gear-1);                                     // Show on Neoplixel       
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
      showgear(gear-1);
      showgear(gear-2);
      up();                                                 // gear shift up  
    }
    else if (gear < 6){
      gear += 1;                                            // increase gear number by one
      up();                                                 // gear shift up
      showgear(gear-1);
    }                                                       // If gear = 6 do nothing
  pressLength_milliSeconds = 0;                             //Every time through the loop, we need to reset the pressLength_Seconds counter 
  } 
}

// Motion routines for up, down and netrual
// Gear shift up
void up() {                 
  for(pos =mid; pos <= sup; pos += degree)                  // goes from mid to up 
  {                                                         // in steps of 1 degree 
    if((sup-pos)>SPEED) pos++;                              // move faster if not near end point
    motion(pos);
  } 
  for(pos = sup; pos >= mid; pos -= degree)                 // goes from up to up to mid 
    {                                                       // in steps of 1 degree 
    if((pos-mid)>SPEED) pos--;                              // move faster if not near end point
    motion(pos);
    }            
}

// Gear shift down
void down() {           
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
}

void centre() {                                             // setup for start
    motion(mid);                                            // move to centre
    delay(200);
    down();                                                 // move to down
    delay(200);
    netrual();                                              // move to netural
}
   
void netrual() {                                            //
  for(pos = mid; pos <= net; pos += 1)                      // goes from mid to netrual 
    {                                                       // in steps of 1 degree 
      motion(pos);
    }           
  delay (500);
  for(pos = net ; pos >= (mid-correction); pos -= 1)        // goes netrual to mid with backlash allowance 
    {                                                       // in steps of 1 degree 
      motion(pos);
    } 
  gear = 0;
  nopixels();
}

void motion(uint8_t p){ 
       myservo.write(p);                                    // tell servo to go to position in variable 'pos' 
       delay(REFRESH_PERIOD_MS);                            // waits 20ms for refresh period 
       SoftRcPulseOut::refresh(NOW);                        // generates the servo pulse Now  
}
      
void showgear(uint8_t g){
    pixels.setPixelColor(g, pixels.Color(155,0,0));         //(R,G,B)
    pixels.show();                                          // This sends the updated pixel color to the hardware.
}

void hidegear(uint8_t g){
    pixels.setPixelColor(g, pixels.Color(0,0,0));           //(R,G,B)
    pixels.show();                                          // This sends the updated pixel color to the hardware.
}
      
void nopixels(){
  for(uint8_t i=0; i<8 ; i +=1){
    pixels.setPixelColor(i, pixels.Color(0,0,0));           //(R,G,B)
    pixels.show();                                          // This sends the updated pixel color to the hardware.         
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

/**
 *  Main function that breaks number to digits and shows result in morese
 */
void morse(uint8_t p) {
    char ch1 = (p/100%10) + 48;                             //48 is added to the number to get the ASCII number to flash  
       flash_morse_code(numbers[ch1 - '0']);
       char ch2 = (p/10%10) + 48;
       flash_morse_code(numbers[ch2 - '0']);
       char ch3 = (p/1%10) + 48;
       flash_morse_code(numbers[ch3 - '0']);
                                                            // Put space between two words in a message...equal to seven dots
        delay(dot_duration * 7);       
}
/**
  *  Flashes the Morse code for a number
  *  https://automaticaddison.com/how-to-display-a-string-as-morse-code-on-an-led-using-arduino/
  *  @param morse_code pointer to the morse code
  */
void flash_morse_code(char *morse_code) {    
  unsigned int i = 0;
                                                            // Read the dots and dashes and flash accordingly
  while (morse_code[i] != NULL) {
    flash_dot_or_dash(morse_code[i]);
    i++;
  } 
                                                            // Space between two letters is equal to three dots
  delay(dot_duration * 3);    
}
 
/**
  *  Flashes the dot or dash in the Morse code
  *  @param dot_or_dash character that is a dot or a dash
  */
void flash_dot_or_dash(char dot_or_dash) {  
                                                            // Make the LED shine
  showgear(6);
  if (dot_or_dash == '.') {                                 // If it is a dot
    delay(dot_duration);           
  }
  else {                                                    // Has to be a dash...equal to three dots
    delay(dot_duration * 3);           
  }
                                                            // Turn the LED off
  hidegear(6);
                                                            // Give space between parts of the same letter...equal to one dot
  delay(dot_duration); 
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
