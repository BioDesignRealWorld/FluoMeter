#include <math.h>
#include <stdlib.h>  
#include <LiquidCrystal.h>   
#include "Counter.h"

#define TIMER_BLUE 30000    // Time (in milliseconds) of measurement with the blue light for GFP fluorescence quantification
#define TIMER_RED 500  // time (ms) for the measurement of turbidity
#define TIMER_WAIT 150 //time between each intermediary count

#define LED_ON LOW
#define LED_OFF HIGH

#define VERSION "Prototype v4.0"

//define the pin's used 
int blue_led = A2;
int red_led = A3;
int pinCounter = 5;

int button1 = 2;
int button1_int = 0;

// state variables
enum State { NIL, IDLE, COUNT_BLUE, COUNT_RED, COUNT_NOISE };
State state = NIL;

// Counter functions, we have one separate counter for each state of the system.
HardwareCounter hwc(pinCounter, TIMER_BLUE);

// Associate digital pins with the LCD
// Arguments: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(9, 3, 4, 8, 6, 7);


void button_pressed()
{
  // here we start the counting process
  if (state == IDLE)
  {
    state = COUNT_BLUE;
    lcd.clear();
    lcd.print("Taking");
    lcd.setCursor(0,1);
    lcd.print("measure.");
    Serial.println("Start BLUE.");
    hwc.set_delay(TIMER_BLUE);
    digitalWrite(blue_led, LED_ON);
    hwc.start();
  }
}

void setup()
{
  
  Serial.begin(9600); // Prepare the serial port
  Serial.println(VERSION);
  
  pinMode(blue_led, OUTPUT);
  digitalWrite(blue_led, LED_OFF);
  pinMode(red_led, OUTPUT);
  digitalWrite(red_led, LED_OFF);

  // Prepare the button
  pinMode(button1, INPUT);
  digitalWrite(button1, HIGH);
  attachInterrupt(button1_int, button_pressed, FALLING);

  state = IDLE;

  // LCD start
  lcd.begin(8,2);
  lcd.print("BioDRW");
  lcd.setCursor(0,1);
  lcd.print("HeLLo!");

}

void loop()
{
  
  long count;
  
  if (state != IDLE)
  {
    if (hwc.available())
    {
      switch (state)
      {
        
        case COUNT_BLUE:
          count = hwc.count();
          digitalWrite(blue_led, LED_OFF);
          Serial.print("Fluorescence count: ");
          Serial.println(count);
          lcd.clear();
          lcd.print("B");
          lcd.print(count);
          
          delay(TIMER_WAIT);
          
          // go to next state
          state = COUNT_RED;
          Serial.println("Start RED.");
          hwc.set_delay(TIMER_RED);
          digitalWrite(red_led, LED_ON);
          hwc.start();
          break;
        
        case COUNT_RED:
          count = hwc.count();
          digitalWrite(red_led, LED_OFF);
          Serial.print("Turbidity count: ");
          Serial.println(count);

          lcd.setCursor(0,1);
          lcd.print("R");
          lcd.print(count);
          
          // go to next state
          Serial.println("Finished my job. Go to IDLE.");
          state = IDLE;
          break;
        
        case COUNT_NOISE:
        
          break;
        
        default:
          break;
      }
    }
  }
 
}
