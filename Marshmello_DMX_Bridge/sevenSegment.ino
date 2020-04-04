byte digits[20] = {
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

  // Same as the first 10, but with the dot
  0b11111110, // 0
  0b11010000, // 1
  0b10011111, // 2
  0b11011101, // 3
  0b11110001, // 4
  0b11101101, // 5
  0b11100111, // 6
  0b11011000, // 7
  0b11111111, // 8
  0b11111001, // 9
};

#define SR_LATCH_PIN 5
#define SR_CLOCK_PIN 3
#define SR_DATA_PIN 6

void setup7Segment() {
  pinMode(SR_LATCH_PIN, OUTPUT);
  pinMode(SR_CLOCK_PIN, OUTPUT);
  pinMode(SR_DATA_PIN, OUTPUT);
}

void display7Segment(int val) {
  digitalWrite(SR_LATCH_PIN, LOW);
  shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, digits[val % 20]);
  digitalWrite(SR_LATCH_PIN, HIGH);
}
