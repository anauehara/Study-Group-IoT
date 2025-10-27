#define TEST_PIN 2

void setup() {
  pinMode(TEST_PIN, OUTPUT);
}

void loop() {
  digitalWrite(TEST_PIN, HIGH);
  delay(500);
  digitalWrite(TEST_PIN, LOW);
  delay(500);
}
