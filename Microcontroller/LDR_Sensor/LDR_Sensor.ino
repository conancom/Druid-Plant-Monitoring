int LDR_value;

void setup() {
  Serial.begin(9600);
}

void loop() {
  LDR_value = analogRead(A2);
  Serial.print("The LDR value is ");
  Serial.println(LDR_value);
  delay(500);
}
