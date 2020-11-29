
#define COLS 12
#define ROWS 4
#define KEYS (COLS * ROWS)
// Number of uint32 to store keys' bits
const unsigned keys_uint32 = ((KEYS - 1) >> 5) + 1;

byte colPins[COLS] = { 11, 9, 6, 10, 7, 18, 20, 17, 16, 15, 8, 14 };
byte rowPins[ROWS] = { 2, 5, 19, 12 };
uint32_t bits[keys_uint32];
uint32_t pins;
int ost = 0, st;

void set(uint32_t *arr, int bit, int v) {
  static unsigned ix;
  ix = bit >> 5;
  bit -= ix * 32;
  if (v) {
    arr[ix] |= 1u << bit;
  } else {
    arr[ix] &= ~(1u << bit);
  }
}

int get(uint32_t *arr, int bit) {
  static unsigned ix;
  ix = bit >> 5;
  bit -= ix * 32;
  return arr[ix] >> bit & 1u;
}

void read() {
  static int v, ix;

  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      v = digitalRead(colPins[c]);
      // set(&pins, colPins[c], v); // pin based index (for debugging)
      set(&pins, c, v); // column based index (for debugging)
      ix = r * COLS + c;
      set(bits, ix, !v);
    }
  }
}

void setup() {
  for (int c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
  for (int r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], 0);
  }

  read();
  ost = pins;
}


void loop() {
  read();
  st = pins;
  if (st ^ ost) {
    ost = pins;
    for (unsigned i = 0; i < KEYS; i++) {
      Keyboard.print(get(bits, i));
    }
    Keyboard.print(".");
    for (unsigned i = 0; i < sizeof(pins) * 8; i++) {
      Keyboard.print(get(&pins, i));
    }
    Keyboard.println();
  }
}
