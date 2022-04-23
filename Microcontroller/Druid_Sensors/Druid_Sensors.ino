#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TimerOne.h>
#include <DHT.h>

#define dhtPin 4
#define dhtType DHT11
DHT dht(dhtPin, dhtType);


const int buttonPin = 5;
int lastButtonState;
int currentButtonState;

const unsigned long duration = 2000000;   // Duration of display for 1 mode
volatile int modeSelect = 0;              // Index of modes array
int modeSelectCopy;                       // Copy of Index of modes array


LiquidCrystal_I2C lcd(0x27, 16, 2);
String modes[5] = {"Temperature", "Soil Moisture", "Air Quality", "Humidity", "Sunlight"};  // Modes for LCD display
String sensorValues[5];


void getSensorData(void)  {
  sensorValues[0] = String(float(dht.readTemperature())) + "*C";
  sensorValues[3] = String(float(dht.readHumidity())) + "%"; 
  Serial.print("Temperature : ");
  Serial.print(sensorValues[0]);
  Serial.print("\tHumidity : ");
  Serial.println(sensorValues[3]);
}


void displayMode(int i) { 
  lcd.clear();      
  delay(150);   // Small delay before displaying the mode'
  lcd.setCursor(0, 0);
  lcd.print(modes[i]);
  lcd.setCursor(0, 1);
  lcd.print(sensorValues[i]);
}


void setup(void) {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  displayMode(modeSelect);
  
  pinMode(buttonPin, INPUT_PULLUP);
  
  Timer1.initialize(duration);
  Timer1.attachInterrupt(timer1_isr);
  dht.begin();

}


void timer1_isr(void) { // Timer1 Interrupt (2 Seconds)
  modeSelect = modeSelect + 1;
  
  if (modeSelect > 4) { // If the pointer is over the length of modes array
    modeSelect = 0;
  }

  interrupts();       // Enable interrupts in order to read data from sensors
  getSensorData();    // Get the data from the sensors
  
}


void loop(void) {
  lastButtonState = currentButtonState;
  currentButtonState = digitalRead(buttonPin);
  

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    Timer1.restart();
  }

  
  if (modeSelect != modeSelectCopy) {   // Move to the next mode is the modeSelect does not equal its copy
    displayMode(modeSelect);            // Update the display to the next mode
    modeSelectCopy = modeSelect;        
  }
}
