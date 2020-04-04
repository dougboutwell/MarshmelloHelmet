#include <Wire.h>
#include <Conceptinetics.h>

#define DEBUG 1

// indexes of LED Control memory layout
#define iMODE  0  // LED Progam Mode #
#define iCLOCK 1  // Clock - decreasing saw wave
#define iRED   2  // 2-4: RGB intensity of primary color
#define iGREEN 3  // 
#define iBLUE  4  //  

// Artificial Clock Generation
inline byte lerp(unsigned long x, unsigned long x1) {
  return x * 255 / x1;
}

inline byte generateClock(byte bpm) {
  unsigned long beatDuration = 60000 / bpm; // in MS
  unsigned long ms = millis() % beatDuration;
  return lerp(ms, beatDuration);
}

byte mode = 0;  // Program mode
byte t = 0;     // Clock
byte r = 0;
byte g = 0;
byte b = 0;

byte digits[10] = {
  0b01111110, // 0
  0b01010000, // 1
  0b00011111, // 2
  0b01011101, // 3
  0b01110001, // 4
  0b01101101, // 5
  0b01100111, // 6
  0b01011000, // 7
  0b01111111, // 8
  0b01111001, // 9
};

#define SR_LATCH_PIN 5
#define SR_CLOCK_PIN 3
#define SR_DATA_PIN 6

void displayMode7Segment(int mode) {
  digitalWrite(SR_LATCH_PIN, LOW);
  shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, digits[mode]);
  digitalWrite(SR_LATCH_PIN, HIGH);
}

void requestEvent() {
  byte message[5] = {mode, t, r, g, b};  
  Wire.write(message, 5);
}

// DMX Setup
#define DMX_SLAVE_CHANNELS   20
DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS );

void setup() {
  // dmx
  dmx_slave.enable();  
  dmx_slave.setStartAddress(1);

  // i2c
  Wire.begin(1);                // join i2c bus with address #1
  Wire.onRequest(requestEvent); // register event

  #ifdef DEBUG
  pinMode(SR_LATCH_PIN, OUTPUT);
  pinMode(SR_CLOCK_PIN, OUTPUT);
  pinMode(SR_DATA_PIN, OUTPUT);
  #endif
}

/*  I currently have this rig setup in SoundSwitch as a Chauvet
 *  COLORPalette unit, which seems to have a reasonable mapping
 *  of DMX channels for the intended usage.
 *  
 *  Documentation at https://www.chauvetdj.com/wp-content/uploads/2015/12/COLORpalette_UM_Rev4_WO.pdf
 *  (see page 12). Currently using "9-channel" mode.
 */
void loop() {
  r = dmx_slave.getChannelValue (11);
  g = dmx_slave.getChannelValue (12);
  b = dmx_slave.getChannelValue (13);
  t = dmx_slave.getChannelValue (10);
  mode = dmx_slave.getChannelValue(1);

  #ifdef DEBUG
  displayMode7Segment(mode);  
  delay(1000);
  #endif
}
