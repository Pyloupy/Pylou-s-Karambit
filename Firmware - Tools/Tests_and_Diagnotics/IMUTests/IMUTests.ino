#include "LSM6DS3.h"

//initiate IMU
LSM6DS3 myIMU(I2C_MODE, 0x6A);

void setup() {
  Serial.begin(9600);
   //Call .begin() to configure the IMUs
  if (myIMU.begin() != 0) {
      Serial.println("Device error");
  } else {
      Serial.println("Device OK!");
  }

}

void loop() {

  float gX = myIMU.readFloatAccelX();
  float gY = myIMU.readFloatAccelY();
  float gZ = myIMU.readFloatAccelZ();

  float magnitude = sqrt(gX * gX + gY * gY + gZ * gZ);
  Serial.print("gX: ");
  Serial.println(gX);
  Serial.print("gY: ");
  Serial.println(gY);
  Serial.print("gZ: ");
  Serial.println(gZ);
  Serial.print("magnitude: ");
  Serial.print(magnitude);
  delay(200);

}
