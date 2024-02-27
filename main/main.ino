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
uint32_t keys_states[keys_uint32]; // Keys' states in the current scan
uint16_t keys_history[KEYS];       // Keys' states history for debouncing

//-----------------------------------------------------------------------------
// Layouts
#define KEY_LAYOUT_2 256u
#define KEY_LAYOUT_1 257u
#define LAYOUTS 3

#define KEY_SHIFT  MODIFIERKEY_SHIFT
#define KEY_CTRL   MODIFIERKEY_CTRL
#define KEY_GUI    MODIFIERKEY_GUI
#define KEY_ALT    MODIFIERKEY_ALT

const unsigned layout_0[KEYS] = {
  KEY_TILDE, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, /**/ KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACE,
  KEY_TAB,   KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, /**/ KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_QUOTE,
  KEY_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, /**/ KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_BRACE/*KEY_RIGHT_SHIFT*/,
  KEY_CTRL,  KEY_GUI, KEY_ALT, KEY_LAYOUT_1, KEY_BACKSPACE, KEY_ESC, /**/ KEY_ENTER, KEY_SPACE, KEY_DELETE, 0, KEY_BACKSLASH, MODIFIERKEY_RIGHT_CTRL
};

const unsigned layout_1[KEYS] = {
  KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, /**/ KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL,
  0, 0, 0, 0, 0, 0,                         /**/ KEY_LEFT, KEY_DOWN, KEY_UP, KEY_RIGHT, KEY_INSERT, 0,
  KEY_SHIFT, 0, 0, KEY_CAPS_LOCK, 0, 0,     /**/ 0, 0, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_BRACE,
  KEY_CTRL, KEY_GUI, KEY_ALT, KEY_LAYOUT_1, KEY_BACKSPACE, KEY_ESC, /**/ KEY_ENTER, KEY_SPACE, KEY_LAYOUT_2, 0, KEY_BACKSLASH, MODIFIERKEY_RIGHT_CTRL
};

const unsigned layout_2[KEYS] = {
  KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, /**/ KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
  0, 0, 0, 0, 0, 0,                               /**/ KEY_HOME, KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_END, 0, KEY_PRINTSCREEN,
  KEY_SHIFT, 0, 0, KEY_CAPS_LOCK, 0, 0,           /**/ 0, 0, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_BRACE,
  KEY_CTRL, KEY_GUI, KEY_ALT, KEY_LAYOUT_1, KEY_BACKSPACE, KEY_ESC, /**/ KEY_ENTER, KEY_SPACE, KEY_LAYOUT_2, 0, KEY_BACKSLASH, MODIFIERKEY_RIGHT_CTRL
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

/*
 * Tht KEY_RIGHT_BRACE acts as Shift when hold and other key is pressed.
 * It implies that the key itself can't be hold.
 * Returns false if the key shouldn't be processed after, true otherwise.
 */ 
inline bool process_a(unsigned code, unsigned down) {
  static unsigned state = 0;

  switch (state) {
    case 0: // Wait for the magic key
      switch (code) {
        case KEY_RIGHT_BRACE:
          state = 1;
          break;
        default: return true;
      }
      break;
    case 1: // Wait for the magic key again, or another key
      switch (code) {
        case KEY_RIGHT_BRACE:
          Keyboard.press(code);
          Keyboard.release(code);
          state = 0;
          break;
        default:
          Keyboard.press(KEY_RIGHT_SHIFT);
          state = 2;
          return true;
      }
      break;
    case 2: // Wait for the magic key release
      switch (code) {
        case KEY_RIGHT_BRACE:
          Keyboard.release(KEY_RIGHT_SHIFT);
          state = 0;
        default: return true;
      }
  }
  return false;
}

/*
 * Default handler for key codes
 */
inline bool process_b(unsigned code, unsigned down) {
  down ? Keyboard.press(code) : Keyboard.release(code);
  return false;
}

// Process key events
// Decide on which layer the event is occured and
// send keyboard events to an USB host
void on_key(unsigned bit, unsigned down) {
  static unsigned layout_ix = 0; // Index of current layout
  static unsigned state = 0;

  unsigned code = 0; // Valid key codes begin from 1, so 0 isn't valid
  /* 
    We can press a key on one layout, and release on another.
    When released we have to check if the key was pressed elsewere
  */
  if (down) {
    // Remember the key pressed at the current layout
    set_bit(layouts[layout_ix].bits, bit, 1); 
    code = layouts[layout_ix].codes[bit]; 
  } else {
    // Get the code from the layout the key was pressed at
    for (unsigned i = 0; i < LAYOUTS; i++) {
      if (get_bit(layouts[i].bits, bit)) {
        set_bit(layouts[i].bits, bit, 0);
        code = layouts[i].codes[bit];
      }
    }
  }

  if (0 == code) { return; }

  // State maching for switching the layouts
  switch (code) {
    case KEY_LAYOUT_1: {
      switch (state) {
        case 0: // down
          state = 1;
          layout_ix = 1;
          break;
        case 1: // up
          state = 0;
          layout_ix = 0;
          break;
        case 2: // up
          state = 3;
          break;
        case 3: // down
          state = 2;
          break;
      }
      break;
    }
    case KEY_LAYOUT_2: {
      switch (state) {
        case 1: // down
          state = 2;
          layout_ix = 2;
          break;
        case 2: // up
          state = 1;
          layout_ix = 1;
          break;
        case 3: // up
          state = 0;
          layout_ix = 0;
          break;
      }
      break;
    }
    default: {
      process_a(code, down) && process_b(code, down);
    }
  }
}

void loop() {
  static unsigned i, t0, t;

  // (Don't) Do the stuff every next millisecond at most
  t = millis();
  // if (t - t0 < 3) { return; } // 333 Hz scan rate
  if (t - t0 < 1) { return; } // 1000 Hz scan rate
  t0 = t;

  scan(keys_states); // Discrete inputs to arrays

  // Debounce and trigger keys' events if any
  for (i = 0; i < KEYS; i++) {
      (keys_history[i] <<= 1) |= get_bit(keys_states, i);
      switch (keys_history[i]
           & 0b11111111111) {
        case 0b01111111111: on_key(i, 1); break;
        case 0b10000000000: on_key(i, 0); break;
      }
  }
}
