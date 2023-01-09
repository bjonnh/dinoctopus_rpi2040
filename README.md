# Dinoctopus

This is a RP2040 based USB-Midi interface that also allows for Midi merging and other MIDI tools
(coming, I'm porting them from an Arduino Mega).

It is currently still highly experimental (but works well for me) and comes with no support.

The noticeable things here is that:
- MIDI interfaces are using the PIO state machines. We can have up to 8 MIDI I or O (or 4 duplex ports)
- Up to 16 virtual USB interfaces (duplex) that allow connecting things together
- (not included) 2 additional physical duplex ports with the RP2040 UARTs (trivial to add)

That's 22 MIDI interfaces (6 physical) on a $4 microcontroller.

I've not stressed tested it yet, but it handles normal playing with multiple controllers so far.

It works out of the box on Linux with no driver installation.
I do not have a Windows or MacOS machine to test on them.

## Building

I'm using [Jetbrains Clion](https://www.jetbrains.com/clion/) and [PlatformIO](https://platformio.org/) to code and upload it to my board
using a [picoprobe](https://github.com/raspberrypi/picoprobe) programmer.

This should probably get you started:
```shell
pio project init
pio run -t upload
```

Most of the configuration you care about is in src/main.cpp

## Thanks

[DiyElectroMusic](https://diyelectromusic.wordpress.com) - About the idea of using PIOs for MIDI

[Ha Thach](https://github.com/hathach/tinyusb) - For TinyUSB (I had to extract some code from there to get virtual cable workings)
