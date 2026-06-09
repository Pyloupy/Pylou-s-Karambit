#include "LSM6DS3.h"
#include "Wire.h"
#include "nrfx_qspi.h"
#include <Adafruit_NeoPixel.h>
#include <PDM.h>
#include <PylouVoiceRecognitionReaver_inferencing.h>

//Watchdog value to reload
#define WDT_RELOAD_VALUE 0x6E524635

//Pins definitions
#define FanPin D0
#define eHeadPin D1
#define LedPin D2
#define ReedPin D3

// If your target is limited in memory remove this macro to save 10K RAM
#define EIDSP_QUANTIZE_FILTERBANK   0

#define VBAT_READ     32
#define CHG           23
#define VBAT_ENABLE P0_14
float Batterylvl;

//initiate Led
Adafruit_NeoPixel RGBLed(1, LedPin, NEO_GRB + NEO_KHZ800);


//initiate IMU
LSM6DS3 myIMU(I2C_MODE, 0x6A);

unsigned long startMillis;
unsigned long currentMillis;

/** Audio buffers, pointers and selectors */
typedef struct {
    int16_t *buffer;
    uint8_t buf_ready;
    uint32_t buf_count;
    uint32_t n_samples;
} inference_t;

static inference_t inference;
static signed short sampleBuffer[2048];
static bool debug_nn = false; 

int VRC; //choose action on VoiceRecognition

float confidence = 0.8;

//Object and function to disable the flash Memory to save up some power
static nrfx_qspi_config_t QSPIConfig;
void turnOffFlashMemory(){
    QSPIConfig.pins = { // Setup for the SEEED XIAO BLE - nRF52840                                                     
   .sck_pin     = 21,                                
   .csn_pin     = 25,                                
   .io0_pin     = 20,                                
   .io1_pin     = 24,                                
   .io2_pin     = 22,                                
   .io3_pin     = 23,                                
  }; 

  QSPIConfig.irq_priority = (uint8_t)NRFX_QSPI_CONFIG_IRQ_PRIORITY; //priority 7 lowest because we just want to send a command 0xB9 to turn the qspi flash to deep sleep mode          
  QSPIConfig.prot_if = {                                                        
    // .readoc     = (nrf_qspi_readoc_t)NRFX_QSPI_CONFIG_READOC,
    .readoc     = (nrf_qspi_readoc_t)NRF_QSPI_READOC_READ4O,       
    // .writeoc    = (nrf_qspi_writeoc_t)NRFX_QSPI_CONFIG_WRITEOC,     
    .writeoc    = (nrf_qspi_writeoc_t)NRF_QSPI_WRITEOC_PP4O,
    .addrmode   = (nrf_qspi_addrmode_t)NRFX_QSPI_CONFIG_ADDRMODE,   
    .dpmconfig  = false,                                            
  };  
  // initialize the QSPI flash and check any errors
  uint32_t Error_Code;
  Error_Code = nrfx_qspi_init(&QSPIConfig, NULL, NULL);
  if (Error_Code != NRFX_SUCCESS) {
    Serial.print("(QSPI_Initialise) nrfx_qspi_init returned : ");
    Serial.println(Error_Code);
  } else {
    Serial.println("(QSPI_Initialise) nrfx_qspi_init successful");
  }
  //setting up the command 0xB9 
  nrf_qspi_cinstr_conf_t cmd_config = {
      .opcode = 0xB9,
      .length = NRF_QSPI_CINSTR_LEN_1B,
      .io2_level = true,
      .io3_level = true,
      .wipwait = false,
      .wren = false
  };
  //sending the command
  nrfx_qspi_cinstr_xfer(&cmd_config, NULL, NULL);

  delayMicroseconds(5); 
  //uninitilaize the QSPI flash to avoid any comsumption
  nrfx_qspi_uninit();
}


void IMUSleepConfig(){
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, 0x10); // accel 12.5 Hz, 2g
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x80); // activate Interupt
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x03); // threshold
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_DUR, 0x03); // duration to check, here no delay
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x20); // Signal to INT1 (which is connected to the board)
}


void goToSleep(){
  Serial.println("Turning device donw (deep sleep SYSTEM OFF)");
  Serial.flush(); // waiting for Serial to finish sending the message to avoid any errors
  turnOffFlashMemory();
  IMUSleepConfig();// calling the config for intterupt

  microphone_inference_end(); //end PDM

  nrf_gpio_cfg_sense_input(digitalPinToPinName(PIN_LSM6DS3TR_C_INT1), NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH); //Setting INT1 as an intterupt, digitalPinToPinName is necessary to get the right pin coz nordic use differents values that arduino (TRUST ME I STUCKED ON THIS FOR)
  nrf_gpio_cfg_sense_input(digitalPinToPinName(ReedPin), NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW); //Reed Interrupt for wake up



  delay(200); //delay to not wake instantly

  NRF_POWER->SYSTEMOFF = 1;

  while(1); // nothing while execute after this coz CPU is disconnected from power supply :).
}


void configWatchdog(){
  NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) |
                    (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);

  NRF_WDT->CRV = 32768 * 3; // Fix reset at 3s

  NRF_WDT->RREN |= WDT_RREN_RR0_Msk; // Activate Canal 0

  NRF_WDT->TASKS_START = 1; // Start the Watchdog

}

void checkDog(){
  NRF_WDT->RR[0] = WDT_RELOAD_VALUE;
}

void setup() {
  // Enable DC-DC converter Mode
  NRF_POWER->DCDCEN = 1;            // Enable DC/DC converter for REG1 stage

    // initialise ADC wireing_analog_nRF52.c:73
  analogReference(AR_DEFAULT);      // default 0.6V*6=3.6V 　wireing_analog_nRF52.c:73
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

  //Initiate eHead and fan mosfets pins and Reed sensor
  pinMode(eHeadPin, OUTPUT);
  pinMode(FanPin, OUTPUT);
  pinMode(ReedPin, INPUT_PULLUP);

  //begin RGB and set to White for test
  RGBLed.begin();

  //Turn off in case
  fumeeOff();

  uint32_t ResetReason = NRF_POWER->RESETREAS; //check reason of reset from deepsleep
  NRF_POWER->RESETREAS = ResetReason; // Erase register after reading it to not block it in next restarts

  if(ResetReason & POWER_RESETREAS_DOG_Msk){
    Serial.println("Watchdog reset!");
  }else{
    Serial.println("Normal/deepsleep reset");
  }

  // summary of inferencing settings (from model_metadata.h)
  ei_printf("Inferencing settings:\n");
  ei_printf("\tInterval: %.2f ms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
  ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
  ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
  ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

  if (microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false) {
      ei_printf("ERR: Could not allocate audio buffer (size %d), this could be due to the window length of your model\r\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT);
      return;
  }

  //config Serial for tests, useless for final version
  /* Serial.begin(9600);
  while (!Serial);
  Serial.println("Serial opened, waiting 5s in case of flash new firmware"); */

  // Security boot for stuck in deep sleep:
  delay(5000);

  //Call .begin() to configure the IMUs
  if (myIMU.begin() != 0) {
      Serial.println("Device error");
  } else {
      Serial.println("Device OK!");
  }
  configWatchdog(); // start watchdog
  startMillis = millis();
  //goToSleep();
}

int VoiceRecognition(){
  if (inference.buf_ready == 0) {
        return -1; 
  }
  signal_t signal;
  signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
  signal.get_data = &microphone_audio_signal_get_data;
  ei_impulse_result_t result = { 0 };

  EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
  if (r != EI_IMPULSE_OK) {
      ei_printf("ERR: Failed to run classifier (%d)\n", r);
      return -1;
  }

  // Analyse the predictions
  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
      if(result.classification[ix].value>confidence){
        //compare label to these names to return int since return string is a bit complicated
        /* if (strcmp(result.classification[ix].label, "reaver") == 0) return 0;
        else if (strcmp(result.classification[ix].label, "allumeToi") == 0) return 1;
        else if (strcmp(result.classification[ix].label, "fumee") == 0) return 2; */
        return ix; //noise = 2 allumetoi = 0 reaver = 3 fumee = 1
      } 
  }
  #if EI_CLASSIFIER_HAS_ANOMALY == 1
      ei_printf("    anomaly score: %.3f\n", result.anomaly);
  #endif

  inference.buf_ready = 0; //reset data from mic

  return -1;
}

static void pdm_data_ready_inference_callback(void)
{
    int bytesAvailable = PDM.available();

    // read into the sample buffer
    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    if (inference.buf_ready == 0) {
        for(int i = 0; i < bytesRead>>1; i++) {
            inference.buffer[inference.buf_count++] = sampleBuffer[i];

            if(inference.buf_count >= inference.n_samples) {
                inference.buf_count = 0;
                inference.buf_ready = 1;
                break;
            }
        }
    }
}

static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffer = (int16_t *)malloc(n_samples * sizeof(int16_t));

    if(inference.buffer == NULL) {
        return false;
    }

    inference.buf_count  = 0;
    inference.n_samples  = n_samples;
    inference.buf_ready  = 0;

    // configure the data receive callback
    PDM.onReceive(&pdm_data_ready_inference_callback);

    PDM.setBufferSize(4096);

    // initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate
    if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
        ei_printf("Failed to start PDM!");
        microphone_inference_end();

        return false;
    }

    // set the gain, defaults to 20
    PDM.setGain(127);

    return true;
}

/**
 * @brief      Wait on new data
 *
 * @return     True when finished
 */
static bool microphone_inference_record(void)
{
    inference.buf_ready = 0;
    inference.buf_count = 0;

    while(inference.buf_ready == 0) {
        delay(10);
    }

    return true;
}

/**
 * Get raw audio signal data
 */
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffer[offset], out_ptr, length);

    return 0;
}

/**
 * @brief      Stop PDM and release buffers
 */
static void microphone_inference_end(void)
{
    PDM.end();
    free(inference.buffer);
}

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

void showBatteryLevel(){
  Batterylvl = ReadBattery();
  if(Batterylvl>3.7){
    RGBLed.setPixelColor(0, RGBLed.Color(0, 255, 0)); //Green for high battery lvl
  }else if (Batterylvl>3.4) {
    RGBLed.setPixelColor(0, RGBLed.Color(255, 128, 0)); //Orange for medium battery lvl
  }else{
    RGBLed.setPixelColor(0, RGBLed.Color(255, 0, 0)); //Red for low battery lvl
  }
  RGBLed.show();
}

void fumee(){
  analogWrite(FanPin, 128); //control fan speed test will be needed
  digitalWrite(eHeadPin, 1); //Turn on eHead
  RGBLed.setPixelColor(0, RGBLed.Color(255, 192, 203)); //different color here pink
}

void fumeeOff(){
  digitalWrite(FanPin, 0); //Turn off Fan
  digitalWrite(eHeadPin, 0); //Turn off eHead
  RGBLed.setPixelColor(0, RGBLed.Color(255, 255, 255));
}

void loop() {
  checkDog(); //Reload watchdog so it wont restart if everythings working properly

  currentMillis = millis();
  float gX = myIMU.readFloatAccelX();
  float gY = myIMU.readFloatAccelY();
  float gZ = myIMU.readFloatAccelZ();

  float magnitude = sqrt(gX * gX + gY * gY + gZ * gZ);
  /* Serial.println(gX);
  Serial.println(gY);
  Serial.println(gZ); */
  // Detect rapid movement
  if (magnitude > 2.5) {
    Serial.println("slash detected");
    fumee();
    delay(1000);
    startMillis = millis();//restart timer coz movement detected
  }else{
    fumeeOff();
  }
  //voiceRecogntion system
  int VRC = VoiceRecognition();
  switch(VRC){ //noise = 2 allumetoi = 0 reaver = 3 fumee = 1
    case -1: break; //nothing recognised or error
    case 0: break;//do smthg
    case 1: break; //do smthg
    case 2: break; //noise, do nothing
    case 3: break; //do smthg
  }

  //Gotosleep if inactivity of detect on stand and show battery level before
  if (digitalRead(ReedPin) || currentMillis-startMillis>60000){
    showBatteryLevel();
    delay(1500);
    checkDog();
    delay(1500);
    goToSleep();
  }
  if(currentMillis - startMillis > 2000){
    Batterylvl = ReadBattery();
    if (Batterylvl<3.2){ //gotoSleep if battery lvl to low, blink in red to notice the user
      for(int i = 0; i<4; i++){
        RGBLed.setPixelColor(0, RGBLed.Color(255, 0, 0));
        RGBLed.show();
        delay(500);
        
        RGBLed.setPixelColor(0, RGBLed.Color(0, 0, 0));
        RGBLed.show();
        delay(500);
        checkDog();
      }
      goToSleep();
    }
  }
}