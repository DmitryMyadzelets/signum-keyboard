// Firmware for my Sygnum 3.0 keyboard on Teensy 3.2 development board.
// Author: Dmitry Myadzelets
// 
// Notes:
// The original keyboard is designed for Teensy 2.0 based on 8 bit Atmel's chip.
// The Teensy 3.2 is based on 32 bit ARM Cortex-M4. So we use 32 bit variables by default.

// Sygnum 3.0 configuration
#define COLS 12
#define ROWS 4
#define KEYS (COLS * ROWS)

// The pins of Teensy 3.2 used for the keyboard's matrix
uint8_t colPins[COLS] = { 11, 9, 6, 10, 7, 18, 20, 17, 16, 15, 8, 14 };
uint8_t rowPins[ROWS] = { 2, 5, 19, 12 };

// Number of uint32 to store keys' bits
const unsigned keys_uint32 = ((KEYS - 1) >> 5) + 1;

uint32_t keys_old[keys_uint32]; // Keys pressed in the previous scan
uint32_t keys_now[keys_uint32]; // Keys pressed in the current scan
uint32_t keys_new[keys_uint32]; // Keys pressed in the current but not in the previous scan

// Set bit value in an array
void set(uint32_t *arr, int bit, int v) {
  static int ix;
  ix = bit >> 5;
  bit -= ix * 32;
  if (v) {
    arr[ix] |= 1u << bit;
  } else {
    arr[ix] &= ~(1u << bit);
  }
}

// Set bit value in an array
int get(uint32_t *arr, int bit) {
  static int ix;
  ix = bit >> 5;
  bit -= ix * 32;
  return arr[ix] >> bit & 1u;
}

// Scan all keys
void scan() {
  static int r, c, v, ix;

  for (r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], 0);

    for (c = 0; c < COLS; c++) {
      v = digitalRead(colPins[c]);
      ix = r * COLS + c;
      set(keys_now, ix, !v);
    }
    
    pinMode(rowPins[r], INPUT_PULLUP);
  }
}

void setup() {
  for (int c = 0; c < COLS; c++) { pinMode(colPins[c], INPUT_PULLUP); }
  for (int r = 0; r < ROWS; r++) { pinMode(rowPins[r], INPUT_PULLUP); }
}

void loop() {
  static unsigned i;

  scan();

  // Get keys just pressed
  for (i = 0; i < keys_uint32; i++) {
    keys_new[i] = keys_old[i] ^ keys_now[i];
    keys_old[i] = keys_now[i];
  }

  // Debugging
  bool b = false;
  for (i = 0; i < KEYS; i++) {
    // Show newly pressed keys only
    if (get(keys_new, i) && get(keys_now, i)) {
      if (!b) {
        Keyboard.print("keys pressed: ");
      }
      b = true;
      Keyboard.print(i);
      Keyboard.print(" ");
    }
  }

  if (b) {
    Keyboard.println();
  }  
}
