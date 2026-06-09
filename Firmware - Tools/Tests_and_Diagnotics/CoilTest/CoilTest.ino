#define eHeadPin D1

void setup() {
  pinMode(eHeadPin, OUTPUT);
}

void loop() {
  digitalWrite(eHeadPin, 1);
  delay(1000);
  digitalWrite(eHeadPin, 0);
  delay(3000);
  

}
