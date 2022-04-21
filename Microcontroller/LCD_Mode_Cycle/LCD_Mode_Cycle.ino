#include <Wire.h>
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>

const byte toggleButton = 2;
const unsigned long duration = 3000000;   // Duration of display for 1 mode
volatile int modeSelect = 0;              // Index of modes array   
int modeSelectCopy = 0;                   // Copy of Index of modes array

LiquidCrystal_I2C lcd(0x27, 16, 2);
String modes[5] = {"1 Temperature", "2 Soil Moisture", "3 Air Quality", "4 Humidity", "5 Sunlight"};  // Modes for LCD display

void displayMode(int i) { 
  lcd.clear();      
  delay(250);   // Small delay before displaying the mode
  lcd.print(modes[i]);
}


void setup(void) {
  lcd.begin();
  lcd.backlight();
  displayMode(modeSelect);
  pinMode(toggleButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(toggleButton), manualChange, FALLING);
  Timer1.initialize(duration);
  Timer1.attachInterrupt(autoChange);
  
}

void manualChange(void) {
  modeSelect = modeSelect + 1;

  if (modeSelect > 4) { // If the pointer is over the length of modes array
    modeSelect = 0;
  }

  Timer1.restart();
  modeSelect = modeSelect - 1;
}


void autoChange(void) { // Timer1 Interrupt (3 Seconds)
  modeSelect = modeSelect + 1;
  
  if (modeSelect > 4) { // If the pointer is over the length of modes array
    modeSelect = 0;
  }
}

void loop(void) {
  if (modeSelect != modeSelectCopy) {   // Move to the next mode is the modeSelect does not equal its copy
    displayMode(modeSelect);            // Update the display to the next mode
    modeSelectCopy = modeSelect;        
  }

}
