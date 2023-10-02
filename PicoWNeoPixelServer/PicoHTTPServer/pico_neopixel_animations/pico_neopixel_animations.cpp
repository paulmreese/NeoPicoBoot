#include "pico/stdlib.h"
#include "pico_neopixel_animations.h"
#include <stdio.h> //for debug
#include <array>
#include <string>
#include <vector>
#include <tuple>
#include "Adafruit_NeoPixel.hpp"

//  adding constructor so we don't have to copy an Adafruit_NeoPixel  object

NeoPixelStrip::NeoPixelStrip(
    uint16_t led_count, 
    uint16_t pin, 
    std::string pixelOrderString,
    uint8_t brightness
): 
    strip(led_count, pin, NEO_GRB + NEO_KHZ800)
    // this is a member initializer list. It allows us to construct strip 
    // without having to copy anything.
    // Argument 1 = Number of pixels in NeoPixel strip
    // Argument 2 = Arduino pin number (most are valid)
    // Argument 3 = Pixel type flags, add together as needed:
    //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
    //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
    //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
    //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
    //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
    //(pixelOrderString == "default") ? std::string("default") : std::string("A string!") //Make it an array to iterate over
    //TODO: Make setPixelOrder function checking for blank string or breaking
    //      the string into the appropriate number of pieces
{
    // INITIALIZE NeoPixel strip
    interpretPixelOrder(pixelOrderString);
    initializePixelColors(strip.Color(255, 255, 255), strip.Color(255, 30, 35));
    strip.begin();            
    strip.setBrightnessFunctions(
            adjustBrightness, adjustBrightness, adjustBrightness, adjustBrightness
        );
    strip.show();           
}

uint8_t NeoPixelStrip::brightness=100;

// Helper Functions ---------------------------------------------

// delay() function -- wait a number of milliseconds
void NeoPixelStrip::delay(uint32_t ms) {
    sleep_ms(ms);
}

// "Unpacks" a packed 32-bit RGB value into its components
std::array<uint8_t, 3> NeoPixelStrip::unpack(uint32_t packed_val) {
    std::array<uint8_t, 3> color;
    color[0] = (packed_val >> 16) & 0xff; // red
    color[1] = (packed_val >> 8) & 0xff; // green
    color[2] = packed_val  & 0xff; // blue
    return color;
}

// Converts an HTML color string(hex) to a packed color value
uint32_t NeoPixelStrip::packColor(
    uint8_t r_comp, uint8_t g_comp, uint8_t b_comp
    ) 
{
    uint32_t color = strip.Color(r_comp, g_comp, b_comp);
    return color;
}

uint16_t NeoPixelStrip::parseSpeed(uint8_t speed){
    printf("Speed Parsing. Intial Value: %d\n", speed);
    uint16_t new_speed = uint16_t((101-speed));
    printf("Speed Parsing. Altered Value: %d\n", new_speed);
    return new_speed;
}

uint8_t NeoPixelStrip::parseBrightness(int slider_value){
    printf("Brightness Parsing. Intial Value: %d\n", slider_value);
    uint8_t new_brightness = slider_value * 2.5;
    printf("Brightness Parsing. Altered Value: %d\n", new_brightness);
    return new_brightness;
}

// Accesses the full state of the NeoPixels
NeoPixelStrip::State_Settings_Struct& NeoPixelStrip::accessState() {
    return currentStateStruct;
}

void NeoPixelStrip::syncStateWithVector() {
    
    led_1 = pixelColors[pixelOrder[0]];
    led_2 = pixelColors[pixelOrder[1]];
    led_3 = pixelColors[pixelOrder[2]];
    led_4 = pixelColors[pixelOrder[3]];
    led_power = pixelColors[pixelOrder[4]];
    printf("Pwr: %x led1: %x led2: %x led3: %x led4: %x\n",led_power, led_1, led_2, led_3, led_4);
}

void NeoPixelStrip::updateStateColors() {
    //New Vector to replace current color vector
    std::vector<uint32_t> newPixelColors;
    // Sync Pixel Colors(returns undimmed value)
    for (int i=0; i<strip.numPixels(); i++) {
        newPixelColors.push_back(strip.getPixelColor(i));
        printf("pixelColor%d: %d newPixelColor%d: %d\n", 
               i, pixelColors[i], i, newPixelColors[i]);
    }
    newPixelColors.swap(pixelColors);
    syncStateWithVector();
}

// Initialization Functions -------------------------------------

// Interprets the initial pixelOrderString string into an appropriate length 
// Array by breaking the string on each space, then returns the visual index 
// of the pixel, based on its actual electrical order
void NeoPixelStrip::interpretPixelOrder(std::string str){
    std::string f = pixelOrderString;
    std::string word = "";
    for (char x : str) {
        if (x == ' ') {
            pixelOrder.push_back(std::stoi(word));
            word = "";
        } else if (x == 'd') {
            for (int i=0; i < strip.numPixels(); i++){
                pixelOrder.push_back(i);
            }
            return;
        } else {
            word = word + x;
        }
    }
}

// Takes two uint32_t colors as arguments and sets the 2 colors displayed
// in the browser, then alternates the colors onto every pixel
void NeoPixelStrip::initializePixelColors(uint32_t color_1, uint32_t color_2){
    effect_color_1 = color_1;
    effect_color_2 = color_2;

    for (int i=0; i < strip.numPixels(); i++){
        if (i % 2 == 0) {
            pixelColors.push_back(color_1);
        } else {
            pixelColors.push_back(color_2);
        }
    }
    syncStateWithVector();
}


// Functions for creating components of animations -----------------

// Adjusts the gamme brightness of the neopixels without directly 
// affecting them or the brightness value itself. Used in 
// setBrightnessFunctions calls

uint8_t NeoPixelStrip::adjustBrightness (uint8_t val) {
	if (val < 1){
        return val;
    }
    //printf("inputVal: %d\n", val);
    //printf("adjBrightness: %d\n", brightness);
    uint8_t gOut = neopixels_gamma8(brightness);
    // printf("gamma8Out: %d\n", gOut);
    uint8_t rVal = ((val * gOut) >> 8);
    //printf("returnedValue: %d\n", rVal);
    return (rVal);
};

// Steps proportionally to the difference of the values, up to a maximum 
// step size. This can be useful for brightness as well as color value.
// Expects the values to be scaled to from 0-255 
uint8_t NeoPixelStrip::propStep (
    uint8_t start, uint8_t finish, uint8_t min_step=2, uint8_t max_step=10) {
    uint8_t difference = 0;
    uint8_t new_value;
    //printf("pS Start: %d, Finish: %d\n", start, finish);
    
    if (start > finish) {
        difference = start - finish;
    } else if (finish > start){
        difference = finish - start;
    } else if (finish == start) {
        return start;
    }

    uint8_t step = uint8_t((difference / 255) * max_step); 
    // returns next lowest whole number(int)
    
    if (difference < min_step){
        step = 1;
    } else if (step < min_step){
        step = min_step;
    }
    
    if (start > finish) {
        new_value = start-step;
    } else {
        new_value = start+step; 
    }

    //printf("Difference: %d, Step: %d, New Value: %d\n", difference, step, new_value);
    return new_value;
}



// Applies the proportional step to an entire RGB color
uint32_t NeoPixelStrip::propStepColor(
    uint32_t start, uint32_t finish, uint8_t min_step=2, uint8_t max_step=10
){
    //printf("pSC Start: %d, Finish: %d\n", start, finish);
    std::array<uint8_t, 3> unp_start = unpack(start);
    std::array<uint8_t, 3> unp_finish = unpack(finish);
    uint8_t start_R = unp_start[0];
    uint8_t start_G = unp_start[1];
    uint8_t start_B = unp_start[2];
    uint8_t finish_R = unp_finish[0];
    uint8_t finish_G = unp_finish[1];
    uint8_t finish_B = unp_finish[2];

    //compare the R values between colors, G, and B as well
    uint8_t r = propStep(start_R, finish_R, min_step, max_step);
    uint8_t g = propStep(start_G, finish_G, min_step, max_step);
    uint8_t b = propStep(start_B, finish_B, min_step, max_step);

    //Compare the start_R, finish_R, and r values for a sanity check
    //printf("start_R: %d start_G: %d start_B: %d\n", start_R, start_G, start_B);
    //printf("finish_R: %d finish_G: %d finish_B: %d\n", finish_R, finish_G, finish_B);
    //printf("stepped_R: %d stepped_G: %d stepped_B: %d\n", r, g, b);

    //Return uint32_t Color compatible with Adafruit Neopixel Library
    return strip.Color(r, g, b);
}

// Functions for animating effects -----------------------------

// Function to fade from the current brightness up to the given value
void NeoPixelStrip::fadeInBrightness(
    uint8_t brightnessLevel, uint16_t wait
){
    int j=brightness;
    //printf("FadeInPreBrightness: %d, %d\n", j, brightness);
    for (int i=brightness; i<=brightnessLevel; i++) {
        //printf("Fade I: %d\n", i);
        i = uint8_t(i);
        //printf("Fade IUint: %d\n", i);
        brightness = i ;
        //printf("Brightness: %d\n", brightness);
        strip.setBrightnessFunctions(
            adjustBrightness, adjustBrightness, adjustBrightness, adjustBrightness
        );
        strip.show() ;
        delay(wait);
    }
    effect_index = 4;
}

// Function to fade from the current brightness down to 1
void NeoPixelStrip::fadeOutBrightness(uint16_t wait
){
    int j=brightness;
    //printf("FadeOutPreBrightness: %d, %d\n", j, brightness);
    for (int i=brightness; i>34; i--) {
        //printf("Fade I: %d\n", i);
        i = uint8_t(i);
        //printf("Fade IUint: %d\n", i);
        brightness = i;
        //printf("Brightness: %d\n", brightness);
        strip.setBrightnessFunctions(
            adjustBrightness, adjustBrightness, adjustBrightness, adjustBrightness
        );
        strip.show() ;
        delay(wait);
    }
    effect_index = 5;
}

// Transitions a single pixel from start color to finish color, using a
// step size between min_step and max_step, waiting wait milliseconds between
// each step.
void NeoPixelStrip::propTransitionSingle(
    uint16_t pixel, uint32_t start, uint32_t finish, uint16_t wait,
    uint8_t min_step=2, uint8_t max_step=10
){
    uint32_t current = start;
    //printf("pTS Start: %d, Finish: %d\n", start, finish);
    while(current != finish) {
       
        uint32_t next_color = propStepColor(current, finish, min_step, max_step);
        //printf("Current: %d, Next: %d\n", current, next_color);
    	strip.setPixelColor(pixel, next_color);
        
        current = next_color;
    	strip.show();                          //  Update strip to match
        delay(wait);                           //  Pause for a moment
    }
    updateStateColors();
}

void NeoPixelStrip::propTransitionAll(uint32_t finish_color, uint16_t wait, uint8_t min_step, uint8_t max_step){
    std::vector<uint32_t> current(strip.numPixels(), 1), finish(strip.numPixels(), 1);
    printf("checked state: #%x #%x #%x #%x #%x\n", led_power, led_1, led_2, led_3, led_4);
    for (int pixel = 0; pixel<strip.numPixels(); pixel++) {
        printf("got pixel color: %x\n", strip.getPixelColor(pixel));
        current[pixel] = strip.getPixelColor(pixel);
        finish[pixel] = finish_color;
    }
        
    while (current != finish){
        for (int i=0; i<strip.numPixels(); i++){
            printf("Pixel: %d\n", pixelColors[i]);
            printf("Current Color: %d\n", current[pixelOrder[i]]);
            uint32_t next_step = propStepColor(current[pixelOrder[i]], finish_color, min_step, max_step);
            strip.setPixelColor(pixelOrder[i], next_step);
            current[pixelOrder[i]] = next_step;
            printf("Next Color: %d\n", current[pixelOrder[i]]);
        }
        strip.show();
        delay(wait);
    }
    updateStateColors();
}

// Apply the proportional step function to brightness, creating a smoother
// brightness curve than just using all 255 values in the gamma function
void NeoPixelStrip::propTransitionBrightness(
    uint8_t finish_value, uint16_t wait, uint8_t min_step, uint8_t max_step
){
    uint8_t current=brightness, finish=finish_value;
        
    while (current != finish){
        uint8_t next = propStep(current, finish, 2, 10);
        brightness = next;
        current = next;
        strip.setBrightnessFunctions(
            adjustBrightness, adjustBrightness, adjustBrightness, adjustBrightness
        );
        strip.show();
        delay(wait);
    }
}

//handles repetitive tasks in alt_opp_fade. Alternates color assignment
void NeoPixelStrip::altOppFadeHelper(
    uint32_t color_1, uint32_t color_2, uint16_t wait, uint8_t min_step,
    uint8_t max_step
){
    uint32_t transition_1 = color_1;
    uint32_t transition_2 = color_2;
    //either would do as they are exact opposites
    while (transition_2 != color_1){
        for (int i=0; i<strip.numPixels(); i++){
            //even pixel
            if (i % 2 == 0){
                uint32_t nextStep = propStepColor(
                    transition_2, color_1, min_step, max_step
                );
                strip.setPixelColor(pixelOrder[i], nextStep);
            }
            //odd pixel
            else {
                uint32_t nextStep = propStepColor(
                    transition_1, color_2, min_step, max_step
                );
                strip.setPixelColor(pixelOrder[i], nextStep);
            }
        }
        delay(wait);
        strip.show();
        transition_1 = propStepColor(transition_1, color_2, min_step, max_step);
        transition_2 = propStepColor(transition_2, color_1, min_step, max_step);
    }
}

// Fades between two opposing colors for a given number of repetitions
void NeoPixelStrip::altOppFade(
    uint32_t color_1, uint32_t color_2, uint8_t repetitions, uint16_t wait, uint8_t min_step, uint8_t max_step
){
    for (int i=0; i<repetitions; i++){
        if (i % 2 == 0){
            altOppFadeHelper(color_2, color_1, wait, min_step, max_step);
        } else {
            altOppFadeHelper(color_1, color_2, wait, min_step, max_step);
        }
    }
    updateStateColors();
    effect_index = 3;
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void NeoPixelStrip::colorWipe(uint32_t color, int wait) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
        strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
        strip.show();                          //  Update strip to match
        delay(wait);                           //  Pause for a moment
    }
    updateStateColors();
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void NeoPixelStrip::theaterChase(uint32_t color, int wait) {
    for(int a=0; a<10; a++) {  // Repeat 10 times...
        for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
        strip.clear();         //   Set all pixels in RAM to 0 (off)
        // 'c' counts up from 'b' to end of strip in steps of 3...
        for(int c=b; c<strip.numPixels(); c += 3) {
            strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        }
        strip.show(); // Update strip with new contents
        delay(wait);  // Pause for a moment
        }
    }
    updateStateColors();
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void NeoPixelStrip::rainbow(int wait){
    // Hue of first pixel runs 2 complete loops through the color wheel.
    // Color wheel has a range of 65536 but it's OK if we roll over, so
    // just count from 0 to 2*65536. Adding 1024 to firstPixelHue each time
    // means we'll make 2*65536/512 = 256 passes through this outer loop:
    for(long firstPixelHue = 0; firstPixelHue < 2*65536; firstPixelHue += 512) {
        for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
        // Offset pixel hue by an amount to make one full revolution of the
        // color wheel (range of 65536) along the length of the strip
        // (strip.numPixels() steps):
        int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
        // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
        // optionally add saturation and value (brightness) (each 0 to 255).
        // Here we're using just the single-argument hue variant. The result
        // is passed through strip.gamma32() to provide 'truer' colors
        // before assigning to each pixel:
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
        }
        strip.show(); // Update strip with new contents
        delay(wait);  // Pause for a moment
    }
    updateStateColors();
    effect_index = 1;
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void NeoPixelStrip::theaterChaseRainbow(int wait) {
    int firstPixelHue = 0;     // First pixel starts at red (hue 0)
    for(int a=0; a<30; a++) {  // Repeat 30 times...
        for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
        strip.clear();         //   Set all pixels in RAM to 0 (off)
        // 'c' counts up from 'b' to end of strip in increments of 3...
        for(int c=b; c<strip.numPixels(); c += 3) {
            // hue of pixel 'c' is offset by an amount to make one full
            // revolution of the color wheel (range 65536) along the length
            // of the strip (strip.numPixels() steps):
            int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
            uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
            strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        }
        strip.show();                // Update strip with new contents
        delay(wait);                 // Pause for a moment
        firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
        }
    }
    updateStateColors();
    effect_index = 2;
}

// Endpoint functions intended to be interacted with by other projects

// Set a single pixel color. No return required as the parameters that set
// the final values would be the ones used in the return.
void NeoPixelStrip::htmlSinglePixel(int pixel_num, uint32_t packed_color, int wait) {
    propTransitionSingle(pixelOrder[pixel_num], strip.getPixelColor(pixelOrder[pixel_num]), packed_color, wait);
}

// demo_loop() function -- Demonstration of basic usage
void NeoPixelStrip::demo_loop() {
    printf("Color wipes\n");
    // Fill along the length of the strip in various colors...
    colorWipe(strip.Color(255,   0,   0), 50); // Red
    colorWipe(strip.Color(  0, 255,   0), 50); // Green
    colorWipe(strip.Color(  0,   0, 255), 50); // Blue
    printf("Theater scroll\n");
    // Do a theater marquee effect in various colors...
    theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
    theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
    theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness
    printf("Rainbows\n");
    // Rainbows
    rainbow(10);             // Flowing rainbow cycle along the whole strip
    theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}

// test_loop() function -- Demonstration of basic usage
void NeoPixelStrip::test_loop() {
    // Test sequence for startup
    // Should fade in to Crimson/White
    printf("Fade In\n");
    propTransitionBrightness(140, 30);
    printf("Rainbow Marquee\n");
    theaterChaseRainbow(12);
    printf("Rainbows\n");
    rainbow(5);
    altOppFade(strip.Color(255, 30, 35), strip.Color(255, 255, 255), 2, 35);

}

void NeoPixelStrip::test_wrapper(NeoPixelStrip *instance) {
    instance->test_loop();
}
