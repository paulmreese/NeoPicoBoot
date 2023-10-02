// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include "pico/stdio.h"
#include "pico/time.h"
#include <tusb.h>
#include "Adafruit_NeoPixel.hpp"

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        27 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 60 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.


#define DELAYVAL 200 // Time (in milliseconds) to pause between pixels

void example() {
  Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//  Adafruit_NeoPixel pixels2(NUMPIXELS + 5, 16, NEO_GRB + NEO_KHZ800);
  
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
//  pixels2.begin() ;
  
  for (int l ; l <4 ; l++) {
  pixels.clear(); // Set all pixel colors to 'off'
   printf("cleared pixels 1\n"); 
//  pixels2.clear() ;
//  printf("cleared pixels 2\n"); 
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color((i%3)*150,((i+1)%3)*150 ,((i+2)%3)*150));
//	pixels2.setPixelColor(NUMPIXELS - (i +1),pixels2.Color((i%3)*150,((i+1)%3)*150 ,((i+2)%3)*150));
    pixels.show();   // Send the updated pixel colors to the hardware.
//	pixels2.show() ;

    sleep_ms(DELAYVAL); // Pause before next pass through loop
  }
  }
};
  


int main () {
	
  stdio_init_all();
  while (!tud_cdc_connected()) {sleep_ms(100);}
  printf("Starting simple\n"); 
  example() ;
  printf ("Ending example\n");
  example() ;
  printf("Ending 2nd example\n");
  sleep_ms(5000);
};