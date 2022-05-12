#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3);

//BLE scanner needed for Phone

void setup(){
  mySerial.begin(9600);   
  Serial.begin(9600);   
  delay(100);
}


void loop(){
   mySerial.begin(9600); 
  if (Serial.available()>0)
    mySerial.write(Serial.read());
    mySerial.write("xx;xxx;xxx;xxx;xxx");
    delay(1500);
    mySerial.end();
  if (mySerial.available()>0)
    Serial.write(mySerial.read());
}
