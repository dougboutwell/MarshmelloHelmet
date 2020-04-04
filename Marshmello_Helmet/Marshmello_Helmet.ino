#include "LED_Control.h"
#include "LED_Programs.h"

void setup() {
  setupI2c();
  setupLEDs();
}

void loop() {  
  static LEDControl control;
  getControlData(&control);
  updateLEDs(&control);
}
