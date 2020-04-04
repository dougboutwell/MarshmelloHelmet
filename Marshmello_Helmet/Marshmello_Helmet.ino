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

#define BAUD_RATE 38400 // For i2c communication with DMX slave
#define LED_DATA_PIN 9  // Pin we're using for WS2812 communication
#define NUM_LEDS 216    // Total number of LEDs in the strip / array
#define NUM_SEGMENTS 27 // Number of segments to split NUM_LEDS into
                        // when it matters, i.e. for chase effects

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

// To my eye, the response of the LED strip I have becomes pretty non-linear
// beyond a certain point. As values approach 255, it's pretty much full on
// for the last 30-50% of the value range. Instead, scale the values to
// a MUCH lower max so we get something approaching a linear mapping of
// light output to input value.
#define MAX_BRIGHTNESS 64

// Structure for communicating between DMX slave and LED controller.
typedef struct _LEDControl {
  byte mode;   // Program mode to use
  byte t;      // The current clock. Decreasing saw wave with peak on each beat
               // Determines the position we should be at in whatever animation 
               // we're playing.
  byte r;      // RGB color to use
  byte g;
  byte b;
  
  // Is a state transition pending? This should be set to true when
  // the clock goes high and shouldReset is true. After a state
  // transition has occurred, LED code should set this to false.
  bool isTransitionPending;

  // Should we set shouldTransition on the next clock high? Set this
  // to true whenever the clock goes low.
  bool shouldResetTransitionPending;
} LEDControl;

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

#define mPLAIN        0
#define mPUMP         1
#define mPUMP_REVERSE 2
#define mCHASE        3
#define mCHASE_RGB    4

void chase(LEDControl* control, CRGB* leds) {
  unsigned long factor = NUM_LEDS / NUM_SEGMENTS;
  for(int i = 0; i < NUM_LEDS; i++) {  
    byte x = control->t;
    x = x - ((i % factor) * NUM_SEGMENTS);
    leds[i].setRGB(x - i, x - i, x - i);
  }
}

void chase_rgb(LEDControl* control, CRGB* leds) {
  unsigned long factor = NUM_LEDS / NUM_SEGMENTS;
  for(int i = 0; i < NUM_LEDS; i++) {  
    byte x = control->t;
    x = x - ((i % factor) * NUM_SEGMENTS);
    CHSV hsvCol(x,255,255);
    leds[ledTable[i]] = hsvCol;
  }
}

void pumpSingleRandom(LEDControl* control, CRGB* leds) {
    for(int i = 0; i < NUM_LEDS; i++) {  

    }
}

// Simply set all pixels to the RGB color
void plain(LEDControl* control, CRGB* leds) {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(control->r, control->g, control->b);
  }
}

void pump(LEDControl* control, CRGB* leds) {
  for(int i = 0; i < NUM_LEDS; i++) {
    byte x = control->t;    
    leds[i].setRGB(control->r, control->g, control->b);
//    leds[i].nscale8(x);
  }
}

void pump_reverse(LEDControl* control, CRGB* leds) {
  control->t = 255 - control->t;
  control->r = 255 - control->r;  
  control->g = 255 - control->g;
  control->b = 255 - control->b;
  
  pump(control, leds);
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
  Serial.begin(BAUD_RATE);  // start Serial for output
  
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
