#include "LSM6DS3.h"
#include "Wire.h"
#include "nrfx_qspi.h"

//initiate IMU
LSM6DS3 myIMU(I2C_MODE, 0x6A);


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
  nrf_gpio_cfg_sense_input(digitalPinToPinName(PIN_LSM6DS3TR_C_INT1), NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH); //Setting INT1 as an intterupt, digitalPinToPinName is necessary to get the right pin coz nordic use differents values that arduino (TRUST ME I STUCKED ON THIS FOR)

  delay(200); //delay to not wake instantly

  NRF_POWER->SYSTEMOFF = 1;

  while(1); // nothing while execute after this coz CPU is disconnected from power supply :).
}



void setup() {
  //config Serial for tests, useless for final version
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Serial opened, waiting 5s in case of flash new firmware");

  // Security boot for stuck in deep sleep:
  delay(5000);

  //Call .begin() to configure the IMUs
  if (myIMU.begin() != 0) {
      Serial.println("Device error");
  } else {
      Serial.println("Device OK!");
  }
  goToSleep();
}



void loop() {
  // put your main code here, to run repeatedly:
}