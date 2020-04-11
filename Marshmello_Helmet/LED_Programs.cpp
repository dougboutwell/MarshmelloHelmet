#include <Arduino.h>

#include <noise.h>
#include <bitswap.h>
#include <fastspi_types.h>
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

#include "LED_Programs.h"
#include "LED_Control.h"

#define LED_DATA_PIN 2  // Pin we're using for WS2812 communication
#define NUM_LEDS 216    // Total number of LEDs in the strip / array
#define NUM_SEGMENTS 27 // Number of segments to split NUM_LEDS into
                        // when it matters, i.e. for chase effects
#define SEGMENT_LENGTH 8

CRGB leds[NUM_LEDS];

// Lookup table mapping LED index to its position in the LED array.
// LEDs are daisy-chained so each row's direction is reversed
byte ledTable[NUM_LEDS] = {
  0,   1,   2,   3,   4,   5,   6,   7,
  15,  14,  13,  12,  11,  10,  9,   8,
  16,  17,  18,  19,  20,  21,  22,  23,
  31,  30,  29,  28,  27,  26,  25,  24,
  32,  33,  34,  35,  36,  37,  38,  39,
  47,  46,  45,  44,  43,  42,  41,  40,
  48,  49,  50,  51,  52,  53,  54,  55,
  63,  62,  61,  60,  59,  58,  57,  56,
  64,  65,  66,  67,  68,  69,  70,  71,
  79,  78,  77,  76,  75,  74,  73,  72,
  80,  81,  82,  83,  84,  85,  86,  87,
  95,  94,  93,  92,  91,  90,  89,  88,
  96,  97,  98,  99,  100, 101, 102, 103,
  111, 110, 109, 108, 107, 106, 105, 104,
  112, 113, 114, 115, 116, 117, 118, 119,
  127, 126, 125, 124, 123, 122, 121, 120,
  128, 129, 130, 131, 132, 133, 134, 135,
  143, 142, 141, 140, 139, 138, 137, 136,
  144, 145, 146, 147, 148, 149, 150, 151,
  159, 158, 157, 156, 155, 154, 153, 152,
  160, 161, 162, 163, 164, 165, 166, 167,
  175, 174, 173, 172, 171, 170, 169, 168,
  176, 177, 178, 179, 180, 181, 182, 183,
  191, 190, 189, 188, 187, 186, 185, 184,
  192, 193, 194, 195, 196, 197, 198, 199,
  207, 206, 205, 204, 203, 202, 201, 200,
  208, 209, 210, 211, 212, 213, 214, 215
  };

void setupLEDs() {
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);

  // Beyond this value, the LEDs shift REALLY hard toward red. They
  // are brighter, but colors are super wacky.
  FastLED.setMaxPowerInMilliWatts(9000);
}

void updateLEDs(LEDControl* control) {
  switch (control->mode) {
    case mPLAIN:          plain(control); break;
    case mPUMP:           pump(control); break;
    case mPUMP_REVERSE:   pump_reverse(control); break;
    case mCHASE:          chase_horizontal(control); break;
    case mCHASE_VERTICAL: chase_vertical(control); break;
    case mCHASE_RGB:      chase_rgb(control); break;
    case mRAINBOW_H:      rainbow_h(control); break;
  }

//  chase_vertical(control);

  FastLED.show();
}

void chase_horizontal(LEDControl* control) {
  // The column that should be at full strength
  byte fsCol = map(control->t, 0, 255, 0, NUM_SEGMENTS);
  
  for(int i = 0; i < NUM_LEDS; i++) {
    byte thisCol = i / SEGMENT_LENGTH;
    byte x = fsCol == thisCol ? 255 : 0;
    leds[i].setRGB(x, x, x);
  }
}

void chase_vertical(LEDControl* control) {
  // The column that should be at full strength
  byte fsRow = map(control->t, 0, 255, 0, SEGMENT_LENGTH);
  
  for(int i = 0; i < NUM_LEDS; i++) {
    byte thisRow = i % SEGMENT_LENGTH;
    byte x = fsRow == thisRow ? 255 : 0;
    leds[ledTable[i]].setRGB(x, x, x);
  }
}

void chase_rgb(LEDControl* control) {
  for(int i = 0; i < NUM_LEDS; i++) {  
    byte x = control->t;
    x = x - ((i % SEGMENT_LENGTH) * NUM_SEGMENTS);
    CHSV hsvCol(x,255,255);
    leds[ledTable[i]] = hsvCol;
  }
}

void rainbow_h(LEDControl* control) {
    for(int i = 0; i < NUM_LEDS; i++) {  
    }  
}

void pumpSingleRandom(LEDControl* control) {
    for(int i = 0; i < NUM_LEDS; i++) {  
    }
}

// Simply set all pixels to the RGB color
void plain(LEDControl* control) {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(control->r, control->g, control->b);
  }
}

void pump(LEDControl* control) {
  for(int i = 0; i < NUM_LEDS; i++) {
    byte x = control->t;    
    leds[i].setRGB(control->r, control->g, control->b);
//    leds[i].nscale8(x);
  }
}

void pump_reverse(LEDControl* control) {
  control->t = 255 - control->t;
  control->r = 255 - control->r;  
  control->g = 255 - control->g;
  control->b = 255 - control->b;
  
  pump(control);
}
