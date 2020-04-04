#ifndef LED_PROGRAMS
#define LED_PROGRAMS

#include "LED_Control.h"

#define mPLAIN        0
#define mPUMP         1
#define mPUMP_REVERSE 2
#define mCHASE        3
#define mCHASE_RGB    4

void setupLEDs();

void updateLEDs(LEDControl* control);

void chase(LEDControl* control);
void chase_rgb(LEDControl* control);
void pumpSingleRandom(LEDControl* control);
void plain(LEDControl* control);
void pump(LEDControl* control);
void pump_reverse(LEDControl* control);

#endif
