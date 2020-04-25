// ATtiny85 Bike Shifter Dual Button with LED gear count

#include <SoftwareServo.h>        //needs to be 8Mhz

SoftwareServo servo;              // Define left servo

#define ACTIVATED LOW
const int buttonUp = 1;           // the number of the pushbutton pin 1
const int buttonDown = 2;         // the number of the pushbutton pin 2
const int ledPin =  3;
const int del = 5;
const int sup = 180;
const int dow = 0;
const int mid = 90;
const int net = 135;

// variables will change:
int buttonStateUp = 0;            // variable for reading the pushbutton status
int buttonStateDown = 0;          // variable for reading the pushbutton status
int pos = dow;                    // variable for servo position
int gear = 0;                     // variable for gear position

void setup() {
  pinMode(ledPin, OUTPUT);        // turn pin to LED
  servo.attach(0);                // Set left servo to digital pin 10
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonUp, INPUT);
  digitalWrite(buttonUp,HIGH);
  pinMode(buttonDown, INPUT);
  digitalWrite(buttonDown,HIGH);

  // initialize start position netrual
  down();                         // go down one gear shift 
  centre();                       // half shift to get to centre
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
    gear += 1; // increase gear number by one
    flash ();                     // flash gear number 
    }
    
  if (buttonStateDown == ACTIVATED) {
    down();                       // gear shift down
    gear -= 1;                     // decrease gear number by one
       if (gear == -1 ) gear = 1;
       if (gear == 0 ) gear = 1;       
    flash ();                     // flash gear number
  } 
}

// Motion routines for up, down and netrual

// Gear shift up
void up() {                       
   for(pos = mid; pos <= sup; pos += 1)      // goes from 90 degrees to 180 degrees 
     {                                       // in steps of 1 degree 
       servo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(del);                      
       SoftwareServo::refresh();             // required by SoftwareServo Library to sweep correctly
     } 
   for(pos = sup; pos >= mid; pos -= 1)      // goes from 180 degrees to 90 degrees 
     {                                       // in steps of 1 degree 
       servo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(del);                      
       SoftwareServo::refresh();             // required by SoftwareServo Library to sweep correctly
     }           
}

// Gear shift down
void down() {                       
   for(pos = mid; pos>=dow; pos-=1)  
     {                                
       servo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(del);                      
       SoftwareServo::refresh();             // required by SoftwareServo Library to sweep correctly
     }      
   for(pos = dow; pos<=mid; pos+=1)  
     {                                
       servo.write(pos);                     // tell servo to go to position in variable 'pos' 
       delay(del);                      
       SoftwareServo::refresh();             // required by SoftwareServo Library to sweep correctly
     }
  } 

void centre() {
   for(pos = dow; pos <= mid; pos += 1)    // goes from 90 degrees to 0 degrees 
     {                                     // in steps of 1 degree 
       servo.write(pos);                   // tell servo to go to position in variable 'pos' 
       delay(del);                      
       SoftwareServo::refresh();           // required by SoftwareServo Library to sweep correctly
     }           
  }
    
void netrual() {
   for(pos = mid; pos <= net; pos += 1)        // goes from 90 degrees to 135 degrees 
     {                                         // in steps of 1 degree 
       servo.write(pos);                       // tell servo to go to position in variable 'pos' 
       delay(del);                      
       SoftwareServo::refresh();               // required by SoftwareServo Library to sweep correctly
     }           
   for(pos = net ; pos >= mid; pos -= 1)       // goes from 135 degrees to 90 degrees 
     {                                         // in steps of 1 degree 
       servo.write(pos);                       // tell servo to go to position in variable 'pos' 
       delay(del);                      
       SoftwareServo::refresh();               // required by SoftwareServo Library to sweep correctly
     } 
     gear = 0;               
}

// Flashes gear number
void flash(){
   for (int i=0 ; i < gear; i +=1)
     {
       digitalWrite(ledPin, HIGH);
       delay(300);
       digitalWrite(ledPin, LOW);
       delay(300);
     }
}
