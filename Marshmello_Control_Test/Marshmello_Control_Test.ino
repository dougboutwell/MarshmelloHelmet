#include <Wire.h>
#include <Smoothed.h>
#include <SPI.h>

typedef struct _LEDControl {
  byte mode;
  byte t;
  byte r;
  byte g;
  byte b;
} LEDControl;

// indexes of LED Control memory layout
#define iMODE  0  // LED Progam Mode #
#define iCLOCK 1  // Clock - decreasing saw wave
#define iRED   2  // 2-4: RGB intensity of primary color
#define iGREEN 3  // 
#define iBLUE  4  //  

byte mode = 0;  // Program mode
byte t = 0;     // Clock
byte r = 255;
byte g = 0;
byte b = 0;

void requestEvent() {
  byte message[5] = {mode, t, r, g, b};  
  Wire.write(message, 5);
}

// Artificial Clock Generation
inline byte lerp(unsigned long x, unsigned long x1) {
  return x * 255 / x1;
}

inline byte generateClock(byte bpm) {
  unsigned long beatDuration = 60000 / bpm; // in MS
  unsigned long ms = millis() % beatDuration;
  return lerp(ms, beatDuration);
}

Smoothed <int> pot;

void setup() {
  pot.begin(SMOOTHED_AVERAGE, 30);

  // i2c
  Wire.begin(1);                // join i2c bus with address #1
  Wire.onRequest(requestEvent); // register event

  setup7Segment();

  Serial.begin(9600);
  pinMode(SS, INPUT_PULLUP);
  pinMode(MISO, OUTPUT);
  pinMode(SCK, INPUT);
  SPCR |= _BV(SPE);
  // turn on interrupts
  SPCR |= _BV(SPIE);
  SPI.attachInterrupt();
}

volatile byte i = 0;
char buf[] = "world";

ISR (SPI_STC_vect) {
  i = SPDR;
  SPDR = buf[i];
//  i %= 6;
}

void loop() {
  // Scale to 0-256 range from 0-1024
  int val = map(analogRead(A0), 0, 1023, 0, 255);
  pot.add(val);
  r = pot.get();

  #ifdef DEBUG
  display7Segment(mode);  
  #endif

  if (digitalRead (SS) == HIGH)
    i = 0;
    
  Serial.println(buf);
  delay(100);
}
