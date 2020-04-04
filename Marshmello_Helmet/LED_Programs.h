#ifndef LED_PROGRAMS
#define LED_PROGRAMS

#define mPLAIN        0
#define mPUMP         1
#define mPUMP_REVERSE 2
#define mCHASE        3
#define mCHASE_RGB    4

void chase(LEDControl* control, CRGB* leds);
void chase_rgb(LEDControl* control, CRGB* leds);
void pumpSingleRandom(LEDControl* control, CRGB* leds);
void plain(LEDControl* control, CRGB* leds);
void pump(LEDControl* control, CRGB* leds);
void pump_reverse(LEDControl* control, CRGB* leds);

#endif
