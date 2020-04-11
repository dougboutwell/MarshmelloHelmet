#include "LED_Control.h"
#include "LED_Programs.h"

void setup() {
  setupControl();
  setupLEDs();
}

unsigned long lastUpdate = 0;
unsigned long minimumFrame = 1000/30;
void loop() {
  static LEDControl control;

  unsigned long now = millis();
  unsigned long nextUpdate = lastUpdate + minimumFrame;
  long wait = nextUpdate - now;

  if (wait < 0) { 
    getControlData(&control);
    updateLEDs(&control);
    lastUpdate = millis();
  }
}
