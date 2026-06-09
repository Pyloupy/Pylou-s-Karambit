#define VBAT_READ     32
#define CHG           23
#define VBAT_ENABLE P0_14

float ReadBattery(){
  digitalWrite(VBAT_ENABLE, LOW); // vattery voltage read enable
  delay(10);

  uint32_t rawSum = 0;
  for(int i = 0; i < 256; i++) {
    rawSum += analogRead(VBAT_READ);
  }

  digitalWrite(VBAT_ENABLE, HIGH);  // vattery voltage read disable 

  float averageRaw = (float)rawSum / 256.0;

  return (averageRaw * 3.6 * 2.961) / 4096.0;  
}


void setup() {
  Serial.begin(9600);

      // initialise ADC wireing_analog_nRF52.c:73
  analogReadResolution(12);         // wireing_analog_nRF52.c:39
  delay(1000);

  //pin to use High charge current (100mA instead of 50mA)
  pinMode(P0_13, OUTPUT);   //Charge Current setting pin
  pinMode(P0_14, OUTPUT);   //Enable Battery Voltage monitoring pin
  digitalWrite(P0_13, LOW); //Charge Current 100mA  

  pinMode(CHG, INPUT_PULLUP);       // sleep current increases significantly without pull-up  
  digitalWrite(CHG, LOW);

  pinMode(VBAT_ENABLE, OUTPUT);
  digitalWrite(VBAT_ENABLE, HIGH);   // vattery voltage read disable  
}

void loop() {
  Serial.println(ReadBattery());
  delay(500);

}
