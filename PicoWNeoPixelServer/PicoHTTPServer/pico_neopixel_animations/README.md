# Adafruit NeoPixel Animation Library for Pi PICO C/C++ SDK

Wrapper library for the Pi Pico Port of the Adafruit Library in a C/C++ SDK environment.
Built according to SDK standards as an INTERFACE library. In addition to functions and capabilities offered in the [original repo](https://github.com/martinkooij/pi-pico-adafruit-neopixels), this library allows the user to choose a custom animation order for a given NeoPixel(WS2812, etc.) strand, independent of the electrical order of the NeoPixel wiring. This enables easy control from an extrnal WiFi or Bluetooth library.

Friendly usage of pio statemachine implementations: Any NeoPixel object seizes sm and pio program space in a friendly, cooperative way.

The source library can be found in the main folder of this project (pico_neopixel_animations subdirectory). Adapt your project Makefile by using the command "add_subdirectory", then adding adding pico_neopixel_animations to the libraries of your project.

## Installation
Clone this repo to you local system using
````
git clone https://github.com/paulmreese/pico_neopixel_animations
````


## Building Examples
If the Pico SDK is installed according to its documentation, then the original examples should easily build.

Linux/Mac
````
cd pico_neopixel_animations/pi-pico-adafruit-neopixels/example
mkdir build && cd build
cmake ..
make
````
Windows
````
cd pico_neopixel_animations/pi-pico-adafruit-neopixels/example
mkdir build && cd build
cmake .. -G "NMake Makefiles"
nmake
````

The examples that work are `simple`, `strandtest_wheels` and ....
for the owner of the Maker PI PICO board of Cytron the example `onboard_cytron` that gives a nice colorful show on the built in NeoPixel (a strand of length 1 on PIN 28).

## Code Setup
NeoPixels may be connected to any GPIO pin. Once you are confident in the wiring and pin choices for your NeoPixel strand, you'll need to construct a `NeoPixelStrip` object to send instructions to, as shown:
1. Declare your LED pin, count(amount of LEDs), and pixel order. The `PIXEL_ORDER` defines the order the NeoPixels should follow, relative to the way they are electrically sequenced(The first NeoPixel of the electrical sequence would automatically be number "0", next is "1", etc.). **The string should terminate with a trailing space!!**
````
uint16_t LED_PIN =  28;
uint16_t LED_COUNT = 5;
std::string PIXEL_ORDER = "3 2 1 0 4 ";
````
2. Declare our `NeoPixelStrip` object(name `npStrip`):
````
NeoPixelStrip npStrip(LED_COUNT, LED_PIN, PIXEL_ORDER);
````
3. Send desired animation instructions:
````
npStrip.test_loop();
````

## Library documentation
This library is intended to provide animation functions that are easy to access from an external project. The available functions fall generally into two types: animation functions and their helper functions. An explanation of the purpose of each individual function is provided in the [header file of this project](pico_neopixel_animations.h)

## Resources specific to the Adafruit Neopixel library
See [the Adafruit Neopixel library documentation](https://github.com/adafruit/Adafruit_NeoPixel) for more information specific to it.

## Resources specific to the Adafruit Pi Pico port
[The original repo](https://github.com/martinkooij/pi-pico-adafruit-neopixels) contains more specific implementation details.
