#include "LED_Control.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// indexes of LED Control memory layout in i2c messages
#define iMODE  0  // LED Progam Mode #
#define iCLOCK 1  // Clock - decreasing saw wave
#define iRED   2  // 2-4: RGB intensity of primary color
#define iGREEN 3  // 
#define iBLUE  4  //  

// If not defined, use i2c for communication
#define USE_SPI 1

void setupI2c();
void setupSPI();
void getControlDataI2c(LEDControl* control);
void getControlDataSPI(LEDControl* control);

void setupControl() {
  #ifdef USE_SPI
  setupSPI();
  #else
  setupI2c();
  #endif  

  Serial.begin(9600);
}

void setupI2c() {
  Wire.begin();        // join i2c bus (address optional for master)
}

void setupSPI() {
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  digitalWrite(SS,HIGH);
}

void getControlData(LEDControl* control) {
  #ifdef USE_SPI
  getControlDataSPI(control);
  #else
  getControlDataI2c(control);
  #endif    
}

// Fetches data from DMX slave via i2c, and stores the response in the LEDControl
// structure.
void getControlDataI2c(LEDControl* control) {
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

//  if (data[iCLOCK] < 64) { 
//    // Whenever the clock goes low, we're eligible to transition state on the
//    // next clock high
//    control -> shouldResetTransitionPending = true;
//  } else if (data[iCLOCK] > 192 && control->shouldResetTransitionPending) {
//    control->isTransitionPending = true;
//    control->shouldResetTransitionPending = false;
//  }
}

void getControlDataSPI(LEDControl* control) {
  static int dataSize = 5;

  byte* x = (byte*)control;

  digitalWrite(SS, LOW);
  // SPI works synchronously, so when we transfer a value to the slave, we
  // don't get the actual value from the slave until the next loop iteration,
  // hence the weird loop structure here where we're writing one behind the
  // current index. First transfer is to request byte 0
  SPI.transfer(0);
  for (int i=0; i < dataSize; i++) {
    x[i] = SPI.transfer(i+1); // read this byte while requesting the next
  }
  digitalWrite(SS, HIGH); // close it

  control->mode = 3;

  static char buf[200];
  sprintf(buf, "mode: %3d, clock: %3d, r: %3d, g: %3d, b: %3d", control->mode, control->t, control->r, control->g, control->b);
  Serial.println(buf);
}
