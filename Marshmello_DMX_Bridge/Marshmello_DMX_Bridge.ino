#include <Wire.h>
#include <Conceptinetics.h>

// indexes of LED Control memory layout
#define iMODE  0  // LED Progam Mode #
#define iCLOCK 1  // Clock - decreasing saw wave
#define iRED   2  // 2-4: RGB intensity of primary color
#define iGREEN 3  // 
#define iBLUE  4  //  

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

void setup() {
  // dmx
  dmx_slave.enable();  
  dmx_slave.setStartAddress(1);

  // i2c
  Wire.begin(1);                // join i2c bus with address #1
  Wire.onRequest(requestEvent); // register event
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
}
