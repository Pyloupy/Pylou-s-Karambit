#include <Adafruit_NeoPixel.h>

#define LedPin D2

//initiate Led
Adafruit_NeoPixel RGBLed(1, LedPin, NEO_GRB + NEO_KHZ800);

void setup() {
  //begin RGB and set to White for test
  RGBLed.begin();

}

void loop() {
  RGBLed.setPixelColor(0, RGBLed.Color(255, 0, 0)); 
  delay(500);
  RGBLed.setPixelColor(0, RGBLed.Color(0, 255, 0)); 
  delay(500);
  RGBLed.setPixelColor(0, RGBLed.Color(0, 0, 255)); 
  delay(500);
}
