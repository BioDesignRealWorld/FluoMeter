#include <math.h>
#include <stdlib.h> 
#include <LiquidCrystal.h>   
#include "Counter.h"

#define TIMER_LENGTH 60000   // Time (in milliseconds) of measurement with the blue light for GFP fluorescence quantification
#define TIMER_LENGTH2 300    // Time (in milliseconds) of measurement with the red light for turbidity quantification
#define AVG_LEN 1
#define PBS 807200.0

// Define functions to control our LEDs
#define led_on() digitalWrite(pinLed, LOW);
#define led_off() digitalWrite(pinLed, HIGH);
#define led_config() pinMode(pinLed, OUTPUT);
#define led2_on() digitalWrite(pinLed2, LOW);
#define led2_off() digitalWrite(pinLed2, HIGH);
#define led2_config() pinMode(pinLed2, OUTPUT);

// Pins used
int pinLed = 13;        // Digital pin 13
int pinLed2 = A2;       // Analog pin 2
int pinCounter = 5;     // Digital pin 5
int button = 3;         // Digital pin 3

// state variables
#define IDLE 0
#define COUNT 1
int state = IDLE;

#define FLUO 0
#define TURB 1
int type = FLUO;        // by default set to FLUO as this is the first measurement we want to do

// Variables used for the button
int buttonstate = 0;
int lastbuttonstate = 0;

// Counter function
HardwareCounter hwc(pinCounter, TIMER_LENGTH);
HardwareCounter hwc2(pinCounter, TIMER_LENGTH2);

// Associate digital pins with the LCD
LiquidCrystal lcd(2, 3, 4, 8, 6, 7);


// Starts the counting for fluorescence
void StartFluo()
{
  if (state == IDLE)
  {
    // State change so we know that we've started counting
    state = COUNT;
    
    // Print that we've started counting
    lcd.begin(8, 2);          // Initialize screen
    lcd.clear();
    lcd.print("Start");
    lcd.setCursor(0, 1);      // Jump to next line
    lcd.print("Counting");
    
    // Turn blue led on
    led_on();
    
    // Start the counter
    hwc.start();
  }  
}


// Starts the counting for turbulence
void StartTurb()
{
  if (state == IDLE)
  {
    // State change so we know that we've started counting
    state = COUNT;
    
    // Print that we've started counting
    //lcd.begin(8, 2);          // Initialize screen
    //lcd.print("Start2");
    //lcd.setCursor(0, 1);      // Jump to next line
    //lcd.print("Counting2");
    
    // Turn red led on
    led2_on();
    
    // Start the counter
    hwc2.start();
  }  
}


// Setup of the system
void setup()
{
  // Set up the LCD's number of columns and rows: 
  lcd.begin(8, 2);
  
  // Prepare pin LED
  pinMode(pinLed, OUTPUT);
  led_off();
  
  // Prepare pin LED2
  pinMode(pinLed2, OUTPUT);
  led2_off();
  
  // Prepare the button
  pinMode(A3, INPUT);
  digitalWrite(A3, HIGH); 
}


// Loop function that is continuously executed
void loop()
{
  // Read the state of the button
  buttonstate = digitalRead(A3);
  
  // If the state of the button has changed and that we're in FLUO state, we start the counting for fluorescence
  if (buttonstate != lastbuttonstate && type == FLUO)
  {
    StartFluo();	   
  }
  
  // If fluorescence quantification was done and printed, we next want to quantify turbulance
  if(type == TURB)
  {
    StartTurb();
  }
  
  
  // If the counter for fluorescence has started, we print its result
  if (state == COUNT && hwc.available() && type == FLUO)
  {
    // Get count result
    long count = hwc.count();
    
    // Print result
    lcd.begin(8,2);              // Initialize screen
    lcd.clear();
    //lcd.print("Count: ");
    //lcd.setCursor(0,1);          // Jump to next line
    lcd.print(count);
    
    // Turn blue led off
    led_off();
    
    // Wait before we start the next counting
    delay(3000);
    
    // Indicate that the next step is turbulance quantification
    state = IDLE;
    type = TURB;
  }  
  
  // If the counter for turbidity has started, we print its result
  if (state == COUNT && hwc2.available() && type == TURB)
  {
    // Get count result
    long count = hwc2.count();
    
    // Print results
    //lcd.begin(8,2);              // Initialize screen
    //lcd.print("Count2: ");
    lcd.setCursor(0,1);          // Jump to next line
    lcd.print(count);
    
    // Turn red light off
    led2_off();
    
    // Indicate that the next step is fluorescence quantification i.e. set system to initial state
    state = IDLE;
    type = FLUO;
  }   
  
  
  // Save present button state
  lastbuttonstate = buttonstate;
}


