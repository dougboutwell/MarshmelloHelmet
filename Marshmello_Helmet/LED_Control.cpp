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
  digitalWrite(SS,LOW);
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

  if (data[iCLOCK] < 64) { 
    // Whenever the clock goes low, we're eligible to transition state on the
    // next clock high
    control -> shouldResetTransitionPending = true;
  } else if (data[iCLOCK] > 192 && control->shouldResetTransitionPending) {
    control->isTransitionPending = true;
    control->shouldResetTransitionPending = false;
  }
}

void getControlDataSPI(LEDControl* control) {
//  static int dataSize = sizeof(LEDControl);

  byte* x = (byte*)control;

  char hello[] = "hello";
  int dataSize = 6;
  byte val;

  char buf[100];
  digitalWrite(SS, LOW);
//  control->mode = SPI.transfer(0);
  for (int i=0; i <= dataSize; i++) {
    val = SPI.transfer(i);
    Serial.print(hello[i]);
    if (i == 0) continue;
    buf[i-1] = val;
  }
  Serial.print("\n");

//  static byte i = 0;
//  SPI.transfer(i++);
  digitalWrite(SS, HIGH);


  Serial.println(buf);

//  Serial.println(i);
//  Serial.println(sprintf("mode: %3d, clock: %3d, r: %3d, g: %3d, b: %3d", control->mode, control->t, control->r, control->g, control->b));
}
