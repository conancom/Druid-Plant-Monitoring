#include <LiquidCrystal_I2C.h>
int sensorValue;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(){  
  lcd.begin();
  lcd.clear();
  Serial.begin(9600);     
}
void loop() {
  sensorValue = analogRead(A1);       // read analog input pin 0
  Serial.print("AirQua=");
  Serial.print(sensorValue);     // prints the value read
  Serial.println(" PPM");
  lcd.setCursor(0,0);
  lcd.print("ArQ=");
  lcd.print(sensorValue,DEC);
  lcd.print(" PPM");
  lcd.println("       "); 
  lcd.print("  ");
  delay(100);                        // wait 100ms for next reading
}
