/*
 * Data types and routines for DMX -> i2c communication with the
 * other Arduino
 */

#ifndef MARSHMELLO_TYPES
#define MARSHMELLO_TYPES

#include <Arduino.h>

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

void setupI2c();
void getControlData(LEDControl* control);

#endif
