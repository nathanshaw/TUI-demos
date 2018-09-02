#include "Adafruit_VL53L0X.h"
#include <Adafruit_NeoPixel.h>

#define PINA 3
#define NUM_PIXELS 40
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PINA, NEO_GRB + NEO_KHZ800);
int brightness = 20;
uint32_t color1 = 0;
uint32_t color2 = 0;
int ramp = 0;
long last_ramp = 0;
int ramp_dir = 1;

int minD = 50;
int maxD = 200;
int minI = 350;
int maxI = 525;

int tof1Distance = 0;
int tof2Distance = 0;

// int ir2Distance = 0;
// int lastIr2D = 0;
int transDelay = 0;

long last_color_change = 0;
long last_serial_msg = 0;

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 5
#define SHT_LOX2 6

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(115200);
  // Serial.println("Booting");

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  Serial.println("Shutdown pins inited...");

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);

  Serial.println("Starting...");
  setID();
  Serial.println("Setting up Neo Pixels");
  strip.setBrightness(255);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if (!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while (1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if (!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while (1);
  }
}

void readDualSensors() {

  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  // print sensor one reading
  if (measure1.RangeStatus != 4) {    // if not out of range
    Serial.println(measure1.RangeMilliMeter);
    tof1Distance =  constrain(map(measure1.RangeMilliMeter, 240, 265, 0, 100), 0, 100);
  }

  if (measure2.RangeStatus != 4) {
    tof2Distance = constrain(map(measure2.RangeMilliMeter, 240, 265, 0, 100), 0, 100);
  }
}

// Fill the dots one after the other with a color
void colorWipe( uint32_t c, uint8_t mi, uint8_t ma) {
  for (uint16_t i = 0; i < NUM_PIXELS; i++) {
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
  long now = millis();
  // read IR sensor
  readDualSensors();
  // Serial.println(analogRead(A0));

  if (tof1Distance > 92) {
    color1 = strip.Color(0, ramp, 0);
    if (now > last_ramp + 50) {
      ramp = ramp + ramp_dir;
      if (ramp > 20) {
        ramp_dir = -1;
        ramp = 20;
      } else if (ramp < 0) {
        ramp_dir = 1;
        ramp = 0;
      }
      last_ramp = now;
    }
  } else {
    color1 = strip.Color(map(tof1Distance, 0, 100, 255, 200), map(tof1Distance, 0, 100, 0, 105), map(tof1Distance, 0, 100, 40, 200));
  }

  if (tof2Distance > 92) {
    color2 = strip.Color(0, ramp, 0);
    if (now > last_ramp + 50) {
      ramp = ramp + ramp_dir;
      if (ramp > 20) {
        ramp_dir = -1;
        ramp = 20;
      } else if (ramp < 0) {
        ramp_dir = 1;
        ramp = 0;
      }
      last_ramp = now;
    }
  } else {
  color2 = strip.Color(map(tof2Distance, 0, 100, 255, 150), map(tof2Distance, 0, 100, 0, 105), map(tof2Distance, 0, 100, 40, 205));
  }

  last_serial_msg = now;
  Serial.print(255);
  Serial.print(",");
  Serial.print(tof1Distance);
  Serial.print(",");
  Serial.print(tof2Distance);
  Serial.print(",");
  Serial.print(brightness);
  Serial.print(",");
  Serial.print(ramp);
  Serial.println();

  // update the lights 10x a second
  if (now > last_color_change + 100) {
    // brightness is
    brightness = int((tof1Distance + tof2Distance)*0.5);
    last_color_change = now;
    colorWipe(color1, 0, 19);
    colorWipe(color2, 20, 39);
    strip.setBrightness(255);
    strip.setBrightness(brightness);
  }
}
