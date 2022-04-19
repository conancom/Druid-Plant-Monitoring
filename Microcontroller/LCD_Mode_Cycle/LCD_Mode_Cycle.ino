#include <Wire.h>
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>

const unsigned long duration = 2000000;   // Duration of display for 1 mode
volatile int modeSelect = 0;              // Index of modes array   
int modeSelectCopy = 0;                   // Copy of Index of modes array

LiquidCrystal_I2C lcd(0x27, 16, 2);
String modes[5] = {"Temperature", "Soil Moisture", "Air Quality", "Air Humidity", "Sunlight"};  // Modes for LCD display

void displayMode(int i) {
  lcd.clear();      
  delay(500);   // Small delay before displaying the mode
  lcd.print(modes[i]);
}


void setup(void) {
  lcd.begin();
  lcd.backlight();
  displayMode(modeSelect);
  Timer1.initialize(duration);
  Timer1.attachInterrupt(changeMode);
}


void changeMode(void) { // Timer1 Interrupt (2 Seconds)
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
