#ifndef LED_PROGRAMS
#define LED_PROGRAMS

#include "LED_Control.h"

#define mPLAIN          0
#define mPUMP           1
#define mPUMP_REVERSE   2
#define mCHASE          3
#define mCHASE_VERTICAL 5
#define mCHASE_RGB      5
#define mRAINBOW_H      6

void setupLEDs();

void updateLEDs(LEDControl* control);

void chase_horizontal(LEDControl* control);
void chase_vertical(LEDControl* control);
void chase_rgb(LEDControl* control);
void pumpSingleRandom(LEDControl* control);
void plain(LEDControl* control);
void pump(LEDControl* control);
void pump_reverse(LEDControl* control);
void rainbow_h(LEDControl* control);

#endif
