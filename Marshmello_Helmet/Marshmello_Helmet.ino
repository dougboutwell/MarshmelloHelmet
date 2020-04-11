#include "LED_Control.h"
#include "LED_Programs.h"

void setup() {
  setupControl();
  setupLEDs();
}

void loop() {  
  static LEDControl control;
  getControlData(&control);
  updateLEDs(&control);
}
