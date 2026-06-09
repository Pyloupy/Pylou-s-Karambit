#define ReedPin D3
#define FanPin D0

void setup() {
  Serial.begin(9600);

  pinMode(FanPin, OUTPUT);
  pinMode(ReedPin, INPUT_PULLUP);

}

void loop() {
  if(!digitalRead(ReedPin)){
    digitalWrite(FanPin, 0);
    delay(1000);
    analogWrite(FanPin, 128);
    delay(1000);
  }else{
    digitalWrite(FanPin, 1);
    delay(200);
  }

}
