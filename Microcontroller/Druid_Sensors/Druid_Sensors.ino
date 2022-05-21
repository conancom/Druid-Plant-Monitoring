#include <SoftwareSerial.h>
#include <Wire.h>
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pins used on the Arduino
#define dhtPin 2
#define dhtType DHT11
#define photoresPin A3
#define airQualityPin A1
#define soilPin A0
#define buttonPin 3

#define duration 4000000  // Interrupt signal will be sent every 4 seconds

DHT dht(dhtPin, dhtType);
SoftwareSerial mySerial(10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile int modeSelect = 0;
int modeSelectCopy; 
String modes[5] = {"Temperature", "Soil Moisture", "Air Quality", "Humidity", "Sunlight"};
String units[5] = {"*C", "", "", "%", " Lumens"};
volatile float sensorValues[5];


int prevButtonState;
int buttonState;


void displaySoilMoisture(int soilVal) {
  if (soilVal == 0) {
    lcd.print("DRY");
  } else {
    lcd.print("WET");
  }
}


int getSoilMoistureValueV1(int soilAnalog) {    // For Resistive Soil Moisture Sensor
  int soilVal;
  if (soilAnalog >= 750) {
    soilVal = 0;
  } else {
    soilVal = 1;
  }

  return soilVal;
}

int getSoilMoistureValueV2(int soilAnalog) {    // For Capactive Soil Moisture Sensor
  int soilVal;
  if (soilAnalog >= 360) {
    soilVal = 0;
  } else {
    soilVal = 1;
  }

  return soilVal;
}




void displayAirQuality(int airQualityVal) {
  switch(airQualityVal) {
    case 0:
      lcd.print("GOOD");
      break;
    case 1:
      lcd.print("MODERATE");
      break;
    case 2:
      lcd.print("POOR");
      break;
    case 3:
      lcd.print("VERY POOR");
      break;
      
  }
}

int getAirQuality(float gasLevel) {
  float airQualityVal;
  if (gasLevel <= 175)  {
    airQualityVal = 0;
  } else if (gasLevel > 175 && gasLevel <= 225) {
    airQualityVal = 1;
  } else if (gasLevel > 225 && gasLevel <= 300) {
    airQualityVal = 2;
  } else if (gasLevel > 300)  {
    airQualityVal = 3;
  }

  return airQualityVal;
}


int getLuxValue(int analogVal)  {   // returns lux/lumen value from analog value of photoresistor
  // Conversion rule
  float Vout = float(analogVal) * (5 / float(1023));// Conversion analog to voltage
  float RLDR = (10000 * (5 - Vout))/ Vout; // Resistance of LDR
  int lux = 500/(RLDR/1000); // Calculate Lumens from Resistance of LDR
  
  return lux;
}


char* makeTransmitString() {  // creates a string containing sensor data to send to bluetooth module
  String combinedString = String(sensorValues[0]) + ";" + String(int(sensorValues[1])) + ";" + String(int(sensorValues[2])) + ";" 
                          + String(int(sensorValues[3])) + ";" + String(int(sensorValues[4]));
   Serial.print(combinedString);
   Serial.print(" this string has len of ");
   Serial.println(combinedString.length());

  char buf[18];
  combinedString.toCharArray(buf, 18);  // convert the string into a character array
  mySerial.write(buf);  // send the character array to the bluetooth module
  mySerial.end();     
}


void getSensorData()  {  // get the data from all of the Druid sensors
  sensorValues[0] = dht.readTemperature();
  sensorValues[1] = getSoilMoistureValueV2(analogRead(soilPin));
  sensorValues[2] = getAirQuality(analogRead(airQualityPin));
  sensorValues[3] = dht.readHumidity();
  sensorValues[4] = getLuxValue(analogRead(photoresPin));
}


void displayMode(int i) { 
  lcd.clear();      
  delay(150);   // Small delay before displaying the mode'
  lcd.setCursor(0, 0);
  lcd.print(modes[i]);
  lcd.setCursor(0, 1);

  if (i == 1) {
      displaySoilMoisture(sensorValues[i]);
  } else if (i == 2) {
      displayAirQuality(sensorValues[i]);
  } else  {
      lcd.print(sensorValues[i]);
      lcd.print(units[i]);
  }
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

  if (prevButtonState == HIGH && buttonState == LOW)  {   // If the button is pressed restart Timer1 (Trigger it)
    Timer1.restart();                         
  }

  mySerial.begin(9600); 

  
 if (mySerial.available() > 0){ // Take input from Bluetooth check
    if (mySerial.read() == 49){
      Timer1.restart();
      // Serial.println("Pressed");
    }
 }
    
  if (modeSelect != modeSelectCopy) {   // Move to the next mode is the modeSelect does not equal its copy
    displayMode(modeSelect);            // Update the display to the next mode
    modeSelectCopy = modeSelect;  
    makeTransmitString();
  }
 
}


void timer1_isr() { // Timer1 Interrupt Service Routine
  modeSelect++;
  if (modeSelect > 4) { // If the pointer is over the length of modes array
    modeSelect = 0;
  }
  
  interrupts();       // Enable interrupts in order to read data from sensors
  getSensorData();    // Get the data from the sensors
  
}
