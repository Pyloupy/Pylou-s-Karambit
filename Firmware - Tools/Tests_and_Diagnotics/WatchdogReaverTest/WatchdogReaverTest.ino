#define WDT_RELOAD_VALUE 0x6E524635


void configWatchdog(){
  NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) |
                    (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);

  NRF_WDT->CRV = 32768 * 3; // Fix reset at 3s

  NRF_WDT->RREN |= WDT_RREN_RR0_Msk; // Activate Canal 0

  NRF_WDT->TASKS_START = 1;

}

void checkDog(){
  NRF_WDT->RR[0] = WDT_RELOAD_VALUE;
}
unsigned long timerA;
void setup() {
  Serial.begin(9600);
  while(!Serial);
  uint32_t ResetReason = NRF_POWER->RESETREAS; //check reason of reset from deepsleep
  NRF_POWER->RESETREAS = ResetReason; // Erase register after reading it to not block it in next restarts

  if(ResetReason & POWER_RESETREAS_DOG_Msk){
    Serial.println("Watchdog reset!");
  }else{
    Serial.println("Normal/deepsleep reset");
  }
  timerA = millis();
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("config watcdog!");
  configWatchdog();
}

void loop() {
  unsigned long timerB = millis();
  if(timerB-timerA<10000){
    Serial.println("Fonctionnement normal");
    digitalWrite(LED_BUILTIN, LOW);
    checkDog();
    delay(800);
  }else{
    Serial.println("Simulate crash to test watchdog");
    digitalWrite(LED_BUILTIN, HIGH);
    while(1){
      //donohthing for crash test
    }
  }

}
