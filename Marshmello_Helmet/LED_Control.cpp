#include <Arduino.h>
#include <Wire.h>

#include "LED_Control.h"

// indexes of LED Control memory layout in i2c messages
#define iMODE  0  // LED Progam Mode #
#define iCLOCK 1  // Clock - decreasing saw wave
#define iRED   2  // 2-4: RGB intensity of primary color
#define iGREEN 3  // 
#define iBLUE  4  //  

void setupI2c() {
  Wire.begin();        // join i2c bus (address optional for master)
}

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
