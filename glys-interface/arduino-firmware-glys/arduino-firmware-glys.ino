#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>
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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, PINA, NEO_GRB + NEO_KHZ800);
/* Assign a unique ID to the sensors */
Adafruit_9DOF                dof   = Adafruit_9DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);


/* Update this with the correct SLP for accurate altitude measurements */
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

float head = 0.0;
int brightness = 0;
int pitch = 0;
int lastP = 0;
int roll = 0;
int lastR = 0;
/**************************************************************************/
/*!
    @brief  Initialises all the sensors used by this example
*/
/**************************************************************************/
void initSensors()
{
  if(!accel.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void setup(void)
{
  Serial.begin(115200);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  } 
  colorWipe(strip.Color(55, 55, 55), 0);
  strip.setBrightness(55);
  /* Initialise the sensors */
  initSensors();
}


// Fill the dots one after the other with a color
void colorWipe( uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
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

/**************************************************************************/
/*!
    @brief  Constantly check the roll/pitch/heading/altitude/temperature
*/
/**************************************************************************/
void loop(void)
{
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_vec_t   orientation;

  /* Calculate pitch and roll from the raw accelerometer data */
  accel.getEvent(&accel_event);
  mag.getEvent(&mag_event);
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
  {
    head = orientation.heading;
    brightness = constrain(map(int(head), 0, 360, 0, 255), 0, 255);
  }
  if (dof.accelGetOrientation(&accel_event, &orientation))
  { 
    lastR = roll;
    lastP = pitch;
    roll = int(orientation.roll);
    pitch = int(orientation.pitch);

    if ((roll > 75 && lastR < -75) || (roll < -75 && lastR > 75)){
        // do nothing the sensor just flipped
        roll = lastR;
     }
         if ((pitch > 165 && lastP < -165) || (pitch < -165 && lastP > 165)){
        // do nothing the sensor just flipped
        pitch = lastP;
     }
    Serial.print(255);
      Serial.print(",");
      Serial.print(int(roll));
      Serial.print(",");
      Serial.print(int(pitch));
      Serial.print(",");
      Serial.print(int(head));
      Serial.print(",");
      Serial.print(255);
      Serial.println();
  }
  colorWipe(strip.Color(int(constrain(map(pitch, -60, 120, 0, 155),0,255)), constrain(map(roll, -70, 70, 0, 55), 0, 255), constrain(map(roll, -70, 70, 0, 155), 0, 255)), 0);
}
