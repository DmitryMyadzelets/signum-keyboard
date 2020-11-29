int ost = 0, st;
int pin = 20;
int pou = 1;

unsigned long pins;
int bytes = sizeof(pins);
int bits = bytes * 8;

void set(int ix, int v) {
  if (v) {
    pins |= 1ul << ix;
  } else {
    pins &= ~(1ul << ix);
  }
}

int get(int ix) {
  return pins >> ix & 1ul;
}

void read() {
  for (int i = 0; i < bits; i += 1) {
    set(i, digitalRead(i));
  }
}

void setup() {
  for (int pin = 0; pin < bits; pin += 1) {
    pinMode(pin, INPUT_PULLUP);
  }
  read();
  ost = pins;
  delay(5000);
}

void loop() {
  read();
  st = pins;
  if (st ^ ost) {
    ost = pins;
    Keyboard.println(pins, BIN);
  }
}
