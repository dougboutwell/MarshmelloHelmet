#include <Smoothed.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


typedef struct _LEDControl {
  byte mode;
  byte t;
  byte r;
  byte g;
  byte b;
} LEDControl;

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

LEDControl control;
byte* data = (void*)&control;
byte i = 0;

// Interrupt service routine for SPI slave
ISR (SPI_STC_vect) {
  i = SPDR;
  SPDR = data[i];
}

void setupDisplay() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
}

void setup() {
//  pot.begin(SMOOTHED_AVERAGE, 30);

  Serial.begin(9600);
  pinMode(SS, INPUT_PULLUP);
  pinMode(MISO, OUTPUT);
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE); // turn on interrupts
  SPI.attachInterrupt();

  setupDisplay();
}

void drawDisplay(LEDControl* control) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("Clk"));

  display.setCursor(32,0);             // Start at top-left corner
  display.println(F("Red"));

  display.setCursor(64,0);             // Start at top-left corner
  display.println(F("Grn"));

  display.setCursor(96,0);             // Start at top-left corner
  display.println(F("Blu"));

  display.setCursor(0,8);             // Start at top-left corner
  display.println(control->t);

  display.setCursor(32,8);             // Start at top-left corner
  display.println(control->r);

  display.setCursor(64,8);             // Start at top-left corner
  display.println(control->g);

  display.setCursor(96,8);             // Start at top-left corner
  display.println(control->b);

  display.display();
}


void loop() {
  // Scale to 0-256 range from 0-1024
//  int val = map(analogRead(A0), 0, 1023, 0, 255);
//  pot.add(val);
//  r = pot.get();

  control.mode = 0;
  control.t = generateClock(60);
  control.r = 255;
  control.b = 255;
  control.g = 0;
//  control->r = 255;

  static unsigned long lastUpdate = 0;
  static unsigned long minimumFrame = 1000/10;
 
  unsigned long now = millis();
  unsigned long nextUpdate = lastUpdate + minimumFrame;
  long wait = nextUpdate - now;

  if (wait < 0) { 
    drawDisplay(&control);
    lastUpdate = millis();
  }

//  Serial.println(control.t);
}
