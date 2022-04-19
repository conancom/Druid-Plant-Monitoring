#include <Wire.h>
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>

const unsigned long duration = 2000000;
volatile int modeSelect = 0;
int modeSelectCopy = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
String modes[5] = {"Temperature", "Soil Moisture", "Air Quality", "Air Humidity", "Sunlight"};

void displayMode(int i) {
  lcd.clear();
  delay(500);
  lcd.print(modes[i]);
}


void setup(void) {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  displayMode(modeSelect);
  Timer1.initialize(duration);
  Timer1.attachInterrupt(changeMode);
}


void changeMode(void) {
  modeSelect = modeSelect + 1;
  
  if (modeSelect > 4) {
    modeSelect = 0;
  }
}

void loop(void) {
  if (modeSelect != modeSelectCopy) {
    displayMode(modeSelect);
    modeSelectCopy = modeSelect;
  }

}
