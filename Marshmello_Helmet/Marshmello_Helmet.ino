//#include <noise.h>
#include <bitswap.h>
//#include <fastspi_types.h>
#include <pixelset.h>
#include <fastled_progmem.h>
#include <led_sysdefs.h>
#include <hsv2rgb.h>
#include <fastled_delay.h>
#include <colorpalettes.h>
#include <color.h>
#include <fastspi_ref.h>
#include <fastspi_bitbang.h>
#include <controller.h>
#include <fastled_config.h>
#include <colorutils.h>
#include <chipsets.h>
#include <pixeltypes.h>
#include <fastspi_dma.h>
#include <fastpin.h>
#include <fastspi_nop.h>
#include <platforms.h>
#include <lib8tion.h>
#include <cpp_compat.h>
#include <fastspi.h>
#include <FastLED.h>
#include <power_mgt.h>

#include <Wire.h>

#include "types.h"
#include "LED_Programs.h"


// To my eye, the response of the LED strip I have becomes pretty non-linear
// beyond a certain point. As values approach 255, it's pretty much full on
// for the last 30-50% of the value range. Instead, scale the values to
// a MUCH lower max so we get something approaching a linear mapping of
// light output to input value.
#define MAX_BRIGHTNESS 64

// indexes of LED Control memory layout in i2c messages
#define iMODE  0  // LED Progam Mode #
#define iCLOCK 1  // Clock - decreasing saw wave
#define iRED   2  // 2-4: RGB intensity of primary color
#define iGREEN 3  // 
#define iBLUE  4  //  

// Fetches data from DMX slave via i2c, and stores the response in the LEDControl
// structure.
void getControlData(LEDControl* control) {
  Wire.requestFrom(1, 5);    // request 3 bytes from slave device #1

  byte data[5];
  int i = 0;
  while(Wire.available()) {  // slave may send less than requested
    data[i] = Wire.read();
    i++;
  }

  control->mode = data[iMODE];
  control->t = data[iCLOCK];
  control->r = data[iRED];
  control->g = data[iGREEN];
  control->b = data[iBLUE];

  if (data[iCLOCK] < 64) { 
    // Whenever the clock goes low, we're eligible to transition state on the
    // next clock high
    control -> shouldResetTransitionPending = true;
  } else if (data[iCLOCK] > 192 && control->shouldResetTransitionPending) {
    control->isTransitionPending = true;
    control->shouldResetTransitionPending = false;
  }
}


void adjustMaxBrightness(CRGB* leds) {
//  for(int i = 0; i < NUM_LEDS; i++) {
//    leds[i].nscale8(MAX_BRIGHTNESS);
//  }
}

/* -------------- MAIN PROGRAM -------------------*/
CRGB ledArray[NUM_LEDS];

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(ledArray, NUM_LEDS);

  // Beyond this value, the LEDs shift REALLY hard toward red. They
  // are brighter, but colors are super wacky.
  FastLED.setMaxPowerInMilliWatts(9000);
//  FastLED.setCorrection(OvercastSky);
}


void loop() {  
  static LEDControl control;
  getControlData(&control);
//  control.mode = mCHASE_RGB; 
  switch (control.mode) {
    case mPLAIN:        plain(&control, ledArray); break;
    case mPUMP:         pump(&control, ledArray); break;
    case mPUMP_REVERSE: pump_reverse(&control, ledArray); break;
    case mCHASE:        chase(&control, ledArray); break;
    case mCHASE_RGB:    chase_rgb(&control, ledArray); break;
  }

//  adjustMaxBrightness(ledArray);
  FastLED.show();
}
