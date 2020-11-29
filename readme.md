# Signum keyboard
My ergonomic keyboard based on [Troy's Flatcher](http://troyfletcher.net) board.

The keyboard uses [Teency USB development board](https://www.pjrc.com/teensy/).

## Development Setup
1. Install [Arduino IDE](//www.arduino.cc)
2. Install [Teensyduino IDE](https://www.pjrc.com/teensy/teensyduino.html)

Note: On Rock64 I used packages build for `arrch64` architecture.

## Resources
* [Signum 3 configuration](http://troyfletcher.net/config.html)
* [USB HID keyboard codes, see page 53](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
* [QMK Keycodes](https://docs.qmk.fm/#/keycodes)
* [Teensy keycodes](https://www.pjrc.com/teensy/td_keyboard.html)
* [Code example](https://gist.github.com/huytd/8dabf762a868b86d2aa597b878e53df0)
* [Teensy 3.1 bare metal: Writing a USB driver](http://kevincuzner.com/2014/12/12/teensy-3-1-bare-metal-writing-a-usb-driver/)
* [Debouncing](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)
* [Debounce in C](https://github.com/tcleg/Button_Debouncer)

## Teensy's pins used to scan the keys
```
Teensy    2.0   3.2

col 0     C6    11
col 1     D2    9
col 2     B7    6
col 3     D3    10
col 4     D0    7
col 5     F7    18
col 6     F5    20
col 7     B6    17
col 8     B5    16
col 9     B4    15
col 10    D1    8
col 11    D7    14

row 0     B0    2
row 1     B3    5
row 2     F6    19
row 3     C7    12
```
Note: I use Teency 3.2

## Indexes of the keys
```
 0  1  2  3  4  5    6  7  8  9 10 11
12 13 14 15 16 17   18 19 20 21 22 23
24 25 26 27 28 29   30 31 32 33 34 35
36 37 38 39 40 41   42 43 44 45 46 47
```
