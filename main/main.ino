// Firmware for the Signum 3.0 keyboard on the Teensy 3.2 development board.
// Author: Dmitry Myadzelets
// 
// Notes:
// The original keyboard is designed for Teensy 2.0 based on 8 bit Atmel's chip.
// The Teensy 3.2 is based on 32 bit ARM Cortex-M4,
// so we use 32 bit variables by default.

// Signum 3.0 board configuration asdfa
#define COLS 12
#define ROWS 4
#define KEYS (COLS * ROWS)

// The pins of Teensy 3.2 used for the keyboard's matrix
uint8_t colPins[COLS] = { 11, 9, 6, 10, 7, 18, 20, 17, 16, 15, 8, 14 };
uint8_t rowPins[ROWS] = { 2, 5, 19, 12 };

// Number of uint32 to store keys' bits
const unsigned keys_uint32 = ((KEYS - 1) >> 5) + 1;

// Arrays to work with keys. For bit arrays the logic is positive.
uint32_t keys_old[keys_uint32]; // Keys' states in the previous scan
uint32_t keys_now[keys_uint32]; // Keys' states in the current scan
uint32_t keys_new[keys_uint32]; // Keys which changed it state
uint8_t keys_states[KEYS]; // Keys' states history for debouncing

//-----------------------------------------------------------------------------
// Layouts
const unsigned layout[KEYS] = {
  KEY_TILDE, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, 
  KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACE,
//
  KEY_TAB, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, 
  KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_QUOTE,
//
  MODIFIERKEY_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, 
  KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_BRACE/*MODIFIERKEY_RIGHT_SHIFT*/,
//
  MODIFIERKEY_CTRL, MODIFIERKEY_GUI, MODIFIERKEY_ALT, 4, MODIFIERKEY_SHIFT, KEY_ESC, 
  KEY_ENTER, KEY_SPACE, KEY_BACKSPACE, 10, KEY_BACKSLASH/*MODIFIERKEY_GUI*/, MODIFIERKEY_RIGHT_CTRL
};

//-----------------------------------------------------------------------------
// The setup function is invoked before the loop function
void setup() {
  for (int c = 0; c < COLS; c++) { pinMode(colPins[c], INPUT_PULLUP); }
  for (int r = 0; r < ROWS; r++) { pinMode(rowPins[r], INPUT_PULLUP); }
}

// Set bit value in the array
void set(uint32_t *arr, unsigned bit, int v) {
  static unsigned ix;
  ix = bit >> 5;
  bit -= ix * 32;
  if (v) {
    arr[ix] |= 1u << bit;
  } else {
    arr[ix] &= ~(1u << bit);
  }
}

// Set bit value in the array
unsigned get(uint32_t *arr, unsigned bit) {
  static unsigned ix;
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

// Debounce all keys
void debounce() {
  static unsigned i;

  for (i = 0; i < KEYS; i++) {
      (keys_states[i] <<= 1) |= get(keys_now, i);
      set(keys_now, i, keys_states[i]);
  }
}

void loop() {
  static unsigned i, t0, t, code, down;

  // Do the stuff every next millisecond at most 
  t = millis();
  if (t == t0) { return; }
  t0 = t;

  scan();
  debounce();
  
  for (i = 0; i < keys_uint32; i++) {
    keys_new[i] = keys_old[i] ^ keys_now[i]; // keys just pressed or released
    keys_old[i] = keys_now[i]; // save to old
  }

  // Send new keyboard events to the USB host
  for (i = 0; i < KEYS; i++) {
    if (get(keys_new, i)) {
      code = layout[i];
      down = get(keys_now, i);

      switch (code) {
        // Example of the hold-key behaviour
        case KEY_SPACE:
        {
          if (!down) {
            Keyboard.press(code);
            Keyboard.release(code);
          }
          break;
        }
        default:
        {
          down ? Keyboard.press(code) : Keyboard.release(code);
        }
      }
    }
  }
}
