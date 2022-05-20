#include <SoftwareSerial.h>
#include <Wire.h>
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define dhtPin 4
#define dhtType DHT11
#define photoresPin A2
#define airQualityPin A1
#define soilPin A0
#define buttonPin 5
#define duration 4000000

DHT dht(dhtPin, dhtType);
SoftwareSerial mySerial(2,3);
LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile int modeSelect = 0;
int modeSelectCopy; 
String modes[5] = {"Temperature", "Soil Moisture", "Air Quality", "Humidity", "Sunlight"};
volatile float sensorValues[5];
float sensorValuesCopy[5];

int prevButtonState;
int buttonState;


int getLuxValue(int analogVal)  {
  // Conversion rule
  float Vout = float(analogVal) * (5 / float(1023));// Conversion analog to voltage
  float RLDR = (10000 * (5 - Vout))/ Vout; // Conversion voltage to resistance
  int lux = 500/(RLDR/1000); // Conversion resitance to lumen
  
  return lux;
}


char* makeTransmitString() {
  String combinedString = String(int(sensorValues[0])) + ";" + String(int(sensorValues[1])) + ";" + String(int(sensorValues[2])) + ";" + String(int(sensorValues[3])) + ";" + String(int(sensorValues[4]));
  Serial.print(combinedString);
  Serial.print(" this string has len of ");
  Serial.println(combinedString.length());

  char buf[18];
  combinedString.toCharArray(buf, 18);
  mySerial.write(buf);
  mySerial.end();
  
}

void getSensorData()  {
  sensorValues[0] = dht.readTemperature();
  sensorValues[1] = analogRead(soilPin);
  sensorValues[2] = analogRead(airQualityPin);
  sensorValues[3] = dht.readHumidity();
  sensorValues[4] = getLuxValue(analogRead(photoresPin));
}


void displayMode(int i) { 
  lcd.clear();      
  delay(150);   // Small delay before displaying the mode'
  lcd.setCursor(0, 0);
  lcd.print(modes[i]);
  lcd.setCursor(0, 1);
  lcd.print(sensorValues[i]);
}


void setup(){
  mySerial.begin(9600);   
  Serial.begin(9600);  

  lcd.begin();
  lcd.backlight();
  displayMode(modeSelect);

  pinMode(buttonPin, INPUT_PULLUP);
  
  Timer1.initialize(duration);
  Timer1.attachInterrupt(timer1_isr);
  dht.begin();

  delay(500);
}


void loop(){
  prevButtonState = buttonState;
  buttonState = digitalRead(buttonPin);

  if (prevButtonState == HIGH && buttonState == LOW)  {
    Timer1.restart();
  }

  mySerial.begin(9600); 
  if (modeSelect != modeSelectCopy) {   // Move to the next mode is the modeSelect does not equal its copy
    displayMode(modeSelect);            // Update the display to the next mode
    modeSelectCopy = modeSelect;  
    makeTransmitString();
  }
 
}


void timer1_isr() { // Timer1 Interrupt (2 Seconds)
  modeSelect++;
  if (modeSelect > 4) { // If the pointer is over the length of modes array
    modeSelect = 0;
  }
  
  interrupts();       // Enable interrupts in order to read data from sensors
  getSensorData();    // Get the data from the sensors
  
}
