void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  Serial.println("Running forwards...");
  for (int i = 1; i < 255; i ++) {
    Serial2.write(i);
    Serial.println(i);
    delay(100);
  }
  Serial.println("Running backwards...");
  for (int i = 255; i > 1; i --) {
    Serial.println(i);
    Serial2.write(i);
    delay(100);
  }

}
