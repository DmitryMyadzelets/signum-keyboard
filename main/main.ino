// Firmware for the Signum 3.0 keyboard on the Teensy 3.2 development board.
// Author: Dmitry Myadzelets
// 
// Notes:
// The original keyboard is designed for Teensy 2.0 based on 8 bit Atmel's chip.
// The Teensy 3.2 is based on 32 bit ARM Cortex-M4,
// so we use 32 bit variables by default.

// Signum 3.0 board configuration
#define COLS 12
#define ROWS 4
#define KEYS (COLS * ROWS)

#define LED 13 // LED pin
#define LEDON digitalWrite(LED, HIGH);
#define LEDOF digitalWrite(LED, LOW);

// The pins of Teensy 3.2 used for the keyboard's matrix
uint8_t colPins[COLS] = { 11, 9, 6, 10, 7, 18, 20, 17, 16, 15, 8, 14 };
uint8_t rowPins[ROWS] = { 2, 5, 19, 12 };

// Number of uint32 to store keys' bits
const unsigned keys_uint32 = ((KEYS - 1) >> 5) + 1;

// Arrays to work with keys. For bit arrays the logic is positive.
uint32_t keys_old[keys_uint32]; // Keys' states in the previous scan
uint32_t keys_now[keys_uint32]; // Keys' states in the current scan
uint32_t keys_new[keys_uint32]; // Keys which changed it state
uint32_t keys_states[KEYS]; // Keys' states history for debouncing

//-----------------------------------------------------------------------------
// Layouts
#define KEY_LAYOUT_2 256u
#define KEY_LAYOUT_1 257u
#define LAYOUTS 3

const unsigned layout_0[KEYS] = {
  KEY_TILDE, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, 
  KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACE,
  //
  KEY_TAB, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, 
  KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_QUOTE,
  //
  MODIFIERKEY_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, 
  KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_BRACE/*MODIFIERKEY_RIGHT_SHIFT*/,
  //
  MODIFIERKEY_CTRL, MODIFIERKEY_GUI, MODIFIERKEY_ALT, KEY_LAYOUT_1, KEY_BACKSPACE, KEY_ESC, 
  KEY_ENTER, KEY_SPACE, KEY_DELETE, 0, KEY_BACKSLASH, MODIFIERKEY_RIGHT_CTRL
};

const unsigned layout_1[KEYS] = {
  KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6,
  KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL,
  //
  0, 0, 0, 0, 0, 0,
  KEY_LEFT, KEY_DOWN, KEY_UP, KEY_RIGHT, KEY_INSERT, 0,
  //
  MODIFIERKEY_SHIFT, 0, 0, KEY_CAPS_LOCK, 0, 0,
  0, 0, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_BRACE,
  //
  MODIFIERKEY_CTRL, MODIFIERKEY_GUI, MODIFIERKEY_ALT, KEY_LAYOUT_1, KEY_BACKSPACE, KEY_ESC, 
  KEY_ENTER, KEY_SPACE, KEY_LAYOUT_2, 0, KEY_BACKSLASH, MODIFIERKEY_RIGHT_CTRL
};

const unsigned layout_2[KEYS] = {
  KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
  KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
  //
  0, 0, 0, 0, 0, 0,
  KEY_HOME, KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_END, 0, KEY_PRINTSCREEN,
  //
  MODIFIERKEY_SHIFT, 0, 0, KEY_CAPS_LOCK, 0, 0,
  0, 0, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_BRACE,
  //
  MODIFIERKEY_CTRL, MODIFIERKEY_GUI, MODIFIERKEY_ALT, KEY_LAYOUT_1, KEY_BACKSPACE, KEY_ESC, 
  KEY_ENTER, KEY_SPACE, KEY_LAYOUT_2, 0, KEY_BACKSLASH, MODIFIERKEY_RIGHT_CTRL
};

typedef struct layout_t {
  uint32_t bits[keys_uint32];
  const unsigned * const codes;
} layout_t;

layout_t layouts [LAYOUTS] = {
  { { 0 }, layout_0 },
  { { 0 }, layout_1 },
  { { 0 }, layout_2 }
};

//-----------------------------------------------------------------------------
// The setup function is invoked before the loop function
void setup() {
  for (int c = 0; c < COLS; c++) { pinMode(colPins[c], INPUT_PULLUP); }
  for (int r = 0; r < ROWS; r++) { pinMode(rowPins[r], INPUT_PULLUP); }
  pinMode(LED, OUTPUT);
}

// Set the bit value in an array
void set_bit(uint32_t *arr, unsigned bit, int v) {
  static unsigned ix;
  ix = bit >> 5;
  bit -= ix * 32;
  if (v) {
    arr[ix] |= 1u << bit;
  } else {
    arr[ix] &= ~(1u << bit);
  }
}

// Get a bit value in the array
unsigned get_bit(uint32_t *arr, unsigned bit) {
  static unsigned ix;
  ix = bit >> 5;
  bit -= ix * 32;
  return arr[ix] >> bit & 1u;
}

// Scan all phisical keys' inputs
void scan(uint32_t *bits) {
  static unsigned r, c, v, ix;

  for (r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], 0);

    for (c = 0; c < COLS; c++) {
      v = digitalRead(colPins[c]);
      ix = r * COLS + c;
      set_bit(bits, ix, !v);
    }

    pinMode(rowPins[r], INPUT_PULLUP);
  }
}

// Debounce all keys
void debounce(uint32_t *debounced) {
  static unsigned i;

  for (i = 0; i < KEYS; i++) {
      (keys_states[i] <<= 1) |= get_bit(debounced, i);
      set_bit(debounced, i, keys_states[i]);
  }
}

// Process key events
// Decide on which layer the event is occured and
// send keyboard events to an USB host
void on_key(unsigned bit, unsigned down) {
  static unsigned code;
  static unsigned layout_ix = 0;
  static unsigned shift_state = 0; 
  static unsigned layout_state = 0;

  // Assume the key code is from the current layout
  code = layouts[layout_ix].codes[bit]; 

  if (down) {
    // Remember the layout the key
    set_bit(layouts[layout_ix].bits, bit, 1); 
  } else {
    // Get the code from the layout the key was pressed at
    for (unsigned i = 0; i < LAYOUTS; i++) {
      if (get_bit(layouts[i].bits, bit)) {
        set_bit(layouts[i].bits, bit, 0);
        code = layouts[i].codes[bit];
      }
    }
  }

  switch (code) {
    case KEY_LAYOUT_1: {
      switch (layout_state) {
        case 0: // down
          layout_state = 1;
          layout_ix = 1;
          break;
        case 1: // up
          layout_state = 0;
          layout_ix = 0;
          break;
        case 2: // up
          layout_state = 3;
          break;
      }
      break;
    }
    case KEY_LAYOUT_2: {
      switch (layout_state) {
        case 1: // down
          layout_state = 2;
          layout_ix = 2;
          break;
        case 2: // up
          layout_state = 1;
          layout_ix = 1;
          break;
        case 3: // up
          layout_state = 0;
          layout_ix = 0;
          break;
      }
      break;
    }
    case KEY_RIGHT_BRACE: {
      switch (shift_state) {
        case 0:
          shift_state = 1;
          break;
        case 1:
          shift_state = 0;
          Keyboard.press(code);
          Keyboard.release(code);
          break;
        case 2:
          shift_state = 0;
          Keyboard.release(KEY_RIGHT_SHIFT);
          break;
      }
      break;
    }
    default: {
      if (1 == shift_state) {
        shift_state = 2;
        Keyboard.press(KEY_RIGHT_SHIFT);
      }
      down ? Keyboard.press(code) : Keyboard.release(code);
    }
  }
}

void loop() {
  static unsigned i, j, t0, t, bit, down;
  static uint32_t tmp;

  // Do the stuff every next millisecond at most
  t = millis();
  if (t == t0) { return; }
  t0 = t;

  scan(keys_now); // Discrete inputs to arrays
  debounce(keys_now);
 
  // Get bits of the keys changed their state after the previous scan
  for (i = 0; i < keys_uint32; i++) {
    keys_new[i] = keys_old[i] ^ keys_now[i]; // keys just pressed or released
    keys_old[i] = keys_now[i]; // update the olds
  }

  // Check if any of the keys has changed its state and call the handle
  for (i = 0; i < keys_uint32; i++) {
    if (keys_new[i]) { 
      tmp = keys_new[i];
      for (j = 0; tmp > 0; j++) {
        if (tmp & 1u) {
          bit = (i << 5) + j;
          down = get_bit(keys_now, bit);
          on_key(bit, down); // Handle the change of a key
        }
        tmp >>= 1;
      }
    }
  }
}
