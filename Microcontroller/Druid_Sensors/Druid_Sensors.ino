#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TimerOne.h>
#include <DHT.h>

#define dhtPin 4
#define dhtType DHT11
#define photoresPin A2
#define soilPin A0

DHT dht(dhtPin, dhtType);


const int buttonPin = 5;
int lastButtonState;
int currentButtonState;

const unsigned long duration = 4000000;   // Duration of display for 1 mode
volatile int modeSelect = 0;              // Index of modes array
int modeSelectCopy;                       // Copy of Index of modes array


LiquidCrystal_I2C lcd(0x27, 16, 2);
String modes[5] = {"Temperature", "Soil Moisture", "Air Quality", "Humidity", "Brightness"};  // Modes for LCD display
String sensorValues[5];


int getLuxValue(int analogVal)  {
  // Conversion rule
  float Vout = float(analogVal) * (5 / float(1023));// Conversion analog to voltage
  float RLDR = (10000 * (5 - Vout))/ Vout; // Conversion voltage to resistance
  int lux = 500/(RLDR/1000); // Conversion resitance to lumen
  
  return lux;
}


void getSensorData(void)  {
  sensorValues[0] = String(float(dht.readTemperature())) + "*C";
  sensorValues[1] = String(float(analogRead(soilPin)));
  sensorValues[3] = String(float(dht.readHumidity())) + "%"; 
  sensorValues[4] = String(getLuxValue(analogRead(photoresPin))) + " Lux";

  // Testing values
  Serial.print("Temperature : ");
  Serial.print(sensorValues[0]);
  Serial.print("\tHumidity : ");
  Serial.print(sensorValues[3]);
  Serial.print("\tBrightness : ");
  Serial.println(sensorValues[4]);
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
