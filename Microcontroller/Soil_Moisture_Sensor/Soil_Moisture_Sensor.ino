int Soil_value;

void setup() {
  Serial.begin(9600);

}

void loop() {
  Soil_value = analogRead(A0);
  Serial.print("The soil moisture value is ");
  Serial.println(Soil_value);
  delay(1000);

}
