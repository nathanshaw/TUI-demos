/*
  Code for reading the input from a Force Sensitive Resistor

  Programmed by Nathan Villicana-Shaw in the Fall of 2017 for
  the California College of the Arts Interaction Design department.

  PLEASE NOTE:
  The FSR needs to have a pull-up or pull-down resistor which is connected to the same node
  as the Arduinos fsr_pin. 10k is an appropiate value.

*/

const int fsr_pin = A0;
const int fsrb_pin = A1;
const int vib_pin = 3;

// to keep track of time
const unsigned long polling_delay = 30; // the number of ms between readings
unsigned long last_poll = 0; // the last time the soft pot was read

int fsr_reading = 0;
int fsrb_reading = 0;

#include "Adafruit_VL53L0X.h"
#include <Adafruit_NeoPixel.h>

#define PINA 2
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, PINA, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(fsr_pin, INPUT);
  pinMode(fsrb_pin, INPUT);
  pinMode(vib_pin, OUTPUT);
  strip.begin();
  strip.setBrightness(120);
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(115200);
}

// Fill the dots one after the other with a color
void colorWipeA( uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < 8; i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Fill the dots one after the other with a color
void colorWipeB( uint32_t c, uint8_t wait) {
  for (uint16_t i = 8; i < 16; i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
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

void vibrationMotorA() {
      if (fsr_reading < 400) {
      analogWrite(vib_pin, 50);
    } else if (fsr_reading < 300) {
      analogWrite(vib_pin, 100);
    } else if (fsr_reading < 200) {
      analogWrite(vib_pin, 150);
    } else if (fsr_reading < 100) {
      analogWrite(vib_pin, 200);
    } else {
      analogWrite(vib_pin, 0);
    }
}

void sendSerial() {
  Serial.print(255);
  Serial.print(",");
  Serial.print(constrain(map(fsr_reading, 30, 550, 100, 0),0,100));
  Serial.print(",");
  Serial.print(constrain(map(fsrb_reading, 30, 550, 100, 0), 0, 100));
  Serial.print(",");
  Serial.print(255);
  Serial.print(",");
  Serial.print(255);
  Serial.println();
}

void lightFeedback(int fsrA, int fsrB) {
  int temp = 127 - constrain(map(fsrA, 30, 400, 0, 127), 0, 127);
  colorWipeA(strip.Color(0, temp, int(temp/8)), 0);
  temp = 127 - constrain(map(fsrB, 30, 400, 0, 127), 0, 127);
  colorWipeB(strip.Color(int(temp*0.5), 0, temp), 0);
}

void loop() {
  unsigned long now = millis();
  if (now > last_poll + polling_delay) {
    /* consider using a combination of constrain() followed by map()
      if the values are hard to manage */
    fsr_reading = analogRead(fsr_pin);
    fsrb_reading = analogRead(fsrb_pin);
    last_poll = now;
    sendSerial();
    lightFeedback(fsr_reading, fsrb_reading);
  }
}
