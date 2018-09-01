#include "Adafruit_VL53L0X.h"
#include <Adafruit_NeoPixel.h>

#define PINA 3
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, PINA, NEO_GRB + NEO_KHZ800);

int minD = 50;
int maxD = 200;
int minI = 350;
int maxI = 525;
int tofDistance = 0;
int irDistance = 0;
int lastIrD = 0;
int ir2Distance = 0;
int lastIr2D = 0;
int transDelay = 0;

long last_color_change = 0;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
//Adafruit_VL6180X vl = Adafruit_VL6180X();

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(115200);
  Serial.println("Booting");

  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (millis() < 20000);
  }
  Serial.println("Setting up Neo Pixels");
  strip.setBrightness(20);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  /*
    if (! vl.begin()) {
    Serial.println("Failed to find Adafruit VL6180x");
    while (millis() < 20000);
    }
  */
}

// Fill the dots one after the other with a color
void colorWipe( uint32_t c, uint8_t mi, uint8_t ma) {
  for (uint16_t i = 0; i < 20; i++) {
    if (i >= mi && i <= ma)
      strip.setPixelColor(i, c);
  }
  strip.show();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel( byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  tofDistance = measure.RangeMilliMeter;
  tofDistance = constrain(tofDistance, minD, maxD);

  lastIrD = irDistance;
  Serial.println(analogRead(A0));
  irDistance = constrain(int((map(analogRead(A0), minI, maxI, 255, 0) + irDistance) * 0.5), 0, 255);
  /*
    lastIr2D = ir2Distance;
    ir2Distance = constrain(int((map(analogRead(A1), 0, 1024, 255, 0) + ir2Distance)*0.5), 0, 255);
  */
  int brightness = constrain(irDistance + tofDistance, 0, 255);

  int _temp2 = constrain(map(tofDistance, minD, maxD, 0, 255), 0, 255);
  uint32_t color1 = strip.Color(irDistance, 255 - irDistance, 0);

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print(255);
    Serial.print(",");
    Serial.print(constrain(map(tofDistance, minD, maxD, 0, 100), 0, 100));
    Serial.print(",");
    Serial.print(constrain(map(irDistance, 0, 255, 0, 100), 0, 100));
    Serial.print(",");
    Serial.print(brightness);
    Serial.println();
    // strip.setBrightness(255 - brightness);
  }
  if (millis() > last_color_change + 100) {
    last_color_change = millis();
    colorWipe(color1, 0, 9);
    color1 = strip.Color(255 - _temp2, 0, _temp2);
    colorWipe(color1, 10, 19);
  }
}
