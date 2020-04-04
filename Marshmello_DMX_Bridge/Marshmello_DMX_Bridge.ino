#include <Wire.h>
#include <Conceptinetics.h>
#include <Smoothed.h>

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


void requestEvent() {
  byte message[5] = {mode, t, r, g, b};  
  Wire.write(message, 5);
}

// DMX Setup
#define DMX_SLAVE_CHANNELS   20
DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS );

Smoothed <int> pot;

void setup() {
  pot.begin(SMOOTHED_AVERAGE, 30);
  
  // dmx
  dmx_slave.enable();  
  dmx_slave.setStartAddress(1);

  // i2c
  Wire.begin(1);                // join i2c bus with address #1
  Wire.onRequest(requestEvent); // register event

  #ifdef DEBUG
  setup7Segment();
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
//  t = dmx_slave.getChannelValue (10);
  mode = dmx_slave.getChannelValue(1);

  // Scale to 0-256 range from 0-1024
  int val = map(analogRead(A3), 0, 1023, 0, 255);
  pot.add(val);
  t = pot.get();

  #ifdef DEBUG
  display7Segment(mode);  
  #endif
}
