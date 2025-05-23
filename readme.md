![How the keyboard looks](img/keyboard-right.jpg)

# Signum keyboard
My ergonomic keyboard based on [Troy Fletcher's](http://troyfletcher.net) board.

The keyboard uses [Teensy USB development board](https://www.pjrc.com/teensy/).

The layouts of my keyboard are at [keyboard-layout-editor.com](http://www.keyboard-layout-editor.com/#/gists/1db4e9d50eaa5da4385a4fb53a21c67d).

## Development Setup
1. Download and Unpack [Arduino IDE](//www.arduino.cc). Don't use distro's
   package.
2. Install [Teensyduino add-on](https://www.pjrc.com/teensy/teensyduino.html) for Arduino IDE
3. Select in the Arduino IDE "Tools > USB Type : Keyboard". It adds a HID
   library to the project.

Note: On Rock64 I used packages built for `arrch64` architecture.
Check the About of Arduino IDE, you must see a version Teensyduino there.

## Resources
* [Signum 3 configuration](http://troyfletcher.net/config.html)
* [USB HID keyboard codes, see page 53](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
* [QMK Keycodes](https://docs.qmk.fm/#/keycodes)
* [Teensy keycodes](https://www.pjrc.com/teensy/td_keyboard.html)
* [Code example](https://gist.github.com/huytd/8dabf762a868b86d2aa597b878e53df0)
* [Teensy 3.1 bare metal: Writing a USB driver](http://kevincuzner.com/2014/12/12/teensy-3-1-bare-metal-writing-a-usb-driver/)
* [Debouncing](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)
* [Debounce in C](https://github.com/tcleg/Button_Debouncer)
* [My collection of layouts](https://gist.github.com/DmitryMyadzelets/c22403c905512ba3f0da4bed3c205506)
* [Debouncing](https://geekhack.org/index.php?topic=42385.0)

## Teensy's pins used to scan the keys
The 12x4 I/O create a matrix for 48 keys.
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
Note: I use Teensy 3.2

## Indexes of the keys
```
 0  1  2  3  4  5    6  7  8  9 10 11
12 13 14 15 16 17   18 19 20 21 22 23
24 25 26 27 28 29   30 31 32 33 34 35
36 37 38 39 40 41   42 43 44 45 46 47
```

## Layouts
### My full keyboard
The 47 keys with symbols
```
` 1 2 3 4 5  6 7 8 9 0 - =
  Q W E R T  Y U I O P [ ]
  A S D F G  H J K L ; ' \
  Z X C V B  N M , . /
```
```
Ё 1 2 3 4 5   6 7 8 9 0 - =
  Й Ц У К Е   Н Г Ш Щ З Х Ъ 
  Ф Ы В А П   Р О Л Д Ж Э \
  Я Ч С М И   Т Ь Б Ю .
```
### My ergonomic keyboard
```
` Q W E R T  Y U I O P [
  A S D F G  H J K L ; '
  Z X C V B  N M , . / ]
                     \ 
```
```
Ё Й Ц У К Е  Н Г Ш Щ З Х
  Ф Ы В А П  Р О Л Д Ж Э
  Я Ч С М И  Т Ь Б Ю . Ъ
                     \ 
```
### Other keyboards
Atreus keboard, main layout:
```
Q W E R T      Y U I O P
A S D F G      H J K L ;
Z X C V B `  \ N M , . /
                   - '
```

Modifiers: `Ctrl, Shift, Alt, Win`

Press-only keys: `Esc, Tab, Enter, Caps`

## Observations
Printable symbols can't be delayed. Normally symbols appear when the keys go down. Some reduced keyboards make some symbols (e.g space) appear when a key goes up. When you type fast you can press keys in correct order, but release them in another order. E.g. when you type `a b` you may get `ab ` if the space symbol is delayed. However, some non-printable symbols, like Enter and Tab may have no such effect.

The most used non-symbol keys are Space and Backspace, according to
[this](https://www.businessinsider.com/these-are-the-three-most-popular-keys-on-a-keyboard-2013-7?IR=T)
and [this](https://daniel.haxx.se/blog/2014/11/12/keyboard-key-frequency/)
articles. These keys should be at default positions for thumbs. Another frequently used key is Shift. When it's pressed, the thumbs should remain at their default position.

## Notes
* The Arduino lib `Keyboard.h` has hardcoded limit of 6 keys pressed simultaneously
  (plus modifiers). You may search for `N-Key Roll Over` (NKRO) solution to
overcome this limit.
* To ovecome MCU's limit of the digital inputs you may use an I/O expander with
  a serial interface, like the `MCP23S17` and `MCP23S8` chips (16 and 8 I/O
correspondingly). This way you can even replace the scanning approach with interrupts for every single key.
* After some research about ergonomics for the modiifers I found nothing
  better then [this post](https://dygma.com/blogs/ergonomics/home-row-modifiers-vs-thumbkeys). The kewords: `home-raw modifiers`, `thumb-key modifiers`, if the article goes offline. See also [this page](https://github.com/callum-oakley/qmk_firmware/tree/master/users/callum) for a cool example of such approach.

## Rethinking the layouts
The upper 3 rows of the keys:

Main layout:
```
__`__ __q__ __w__ __e__ __r__ __t__   __y__ __u__ __i__ __o__ __p__ __p__
_tab_ __a__ __s__ __d__ __f__ __g__   __h__ __j__ __k__ __l__ __;__ __'__ 
shift __z__ __x__ __c__ __v__ __b__   __n__ __m__ __,__ __.__ __/__ __]__ 
```
2nd layout:
```
__1__ __2__ __3__ __4__ __5__ __6__   __7__ __8__ __9__ __0__ __-__ __=__
_____ _____ _____ _____ _____ _____   left_ down_ _up__ right insrt _____ 
shift __z__ __x__ caps_ _____ _____   _____ _____ __,__ __.__ __/__ __]__ 
```
3rd layout:
```
_f1__ _f2__ _f3__ _f4__ _f5__ _f6__   _f7__ _f8__ _f9__ _f10_ _f11_ _f12_
_____ _____ _____ _____ _____ _____   home_ pg_dn pg_up _end_ _____ print 
shift __z__ __x__ caps_ _____ _____   _____ _____ __,__ __.__ __/__ __]__ 
```

The lower row of the keys:

Main layout:
```
ctrl_ _win_ _alt_ lay_1 _bsp_ _esc_   enter space _del_ _____ __\__ ctrl_
```
2nd and 3rd layouts:
```
ctrl_ _win_ _alt_ lay_1 _bsp_ _esc_   enter space lay_2 _____ __\__ ctrl_
```
