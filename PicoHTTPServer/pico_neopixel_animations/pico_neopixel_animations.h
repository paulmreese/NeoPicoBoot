#ifndef PICO_NEOPIXEL_ANIMATIONS_H_INCLUDED
#define PICO_NEOPIXEL_ANIMATIONS_H_INCLUDED
/* ^^ these are the include guards */
#include "Adafruit_NeoPixel.hpp"
#include <array>
#include <string>
#include <vector>


/* Prototypes for the class and functions */
class NeoPixelStrip {
    private:
    /* only needed if the = delete trick below doesn't work.
        NeoPixelStrip(const NeoPixelStrip& );
        NeoPixelStrip & operator=(const NeoPixelStrip& );
    */
        Adafruit_NeoPixel strip;
        std::string pixelOrderString;
        std::vector<int> pixelOrder;
        //Retain the current color of each LED
        std::vector<uint32_t> pixelColors;
        //Make externalizing the colors simpler
        uint32_t led_power, led_1, led_2, led_3, led_4;
        
        
        uint8_t effect_index = 0; //0.set-all 1.rainbow 2.rainbow-chase 3.alt-fade 4.fade-in 5.fade-out
        //Retain user set colors
        uint32_t effect_color_1, effect_color_2;

        uint8_t rgb_anim_speed = 50; //Max 255
        uint8_t rgb_anim_reps = 2; //Max 255


    public:
        // preventing copying of NeoPixelStrip
        NeoPixelStrip(const NeoPixelStrip& ) = delete;
        NeoPixelStrip & operator=(const NeoPixelStrip& ) = delete;
        // note if the compiler doesn't like the = delete, remove it, make 
        // the copy constructor and assignment operator private, and do not 
        // implement them

        NeoPixelStrip(
            uint16_t led_count, 
            uint16_t pin, 
            std::string pixelOrderString="default",
            uint8_t brightness=160
        );

        static uint8_t brightness; //Max 255

        /* Return type for entire state */

        struct State_Settings_Struct { 
            uint8_t* effect_index;
            uint32_t* effect_color_1;
            uint32_t* effect_color_2;
            uint8_t* rgb_anim_speed;
            uint8_t* rgb_anim_reps;
            uint8_t* rgb_anim_brightness;
            uint32_t* power_rgb;
            uint32_t* port_rgb_1;
            uint32_t* port_rgb_2;
            uint32_t* port_rgb_3;
            uint32_t* port_rgb_4;
        };

        uint8_t* ei_ref = &effect_index;
        uint32_t* ec1_ref = &effect_color_1;
        uint32_t* ec2_ref = &effect_color_2;
        uint8_t* as_ref = &rgb_anim_speed;
        uint8_t* ar_ref = &rgb_anim_reps;
        uint8_t* ab_ref = &brightness;
        uint32_t* pwr_ref = &led_power;
        uint32_t* p1_ref = &led_1;
        uint32_t* p2_ref = &led_2;
        uint32_t* p3_ref = &led_3;
        uint32_t* p4_ref = &led_4;

        State_Settings_Struct currentStateStruct {
            ei_ref, ec1_ref, ec2_ref, as_ref, ar_ref, ab_ref, 
            pwr_ref, p1_ref, p2_ref, p3_ref, p4_ref
        };
        
        /* Delay for (ms) milliseconds */
        void delay(uint32_t ms);

        /* "Unpacks" a packed 32-bit RGB value into its components by exposing
            strip.Color() */
        std::array<uint8_t, 3> unpack(uint32_t packed_val);

        /* "Packs" 3 8-bit values into a single 32-bit RGB color */
        uint32_t packColor(uint8_t r_comp, uint8_t g_comp, uint8_t b_comp);

        /* Returns the index of a specific NeoPixel in the electrical order, 
           based on it's visual position.
        */
        uint16_t parseOrder(uint16_t value);

        /* Translates a slider value from 1-100 and translates it to a delay
           value in milliseconds(ms)
         */
        uint16_t parseSpeed(uint8_t speed);

        /* Accepts a slider value from 1-100 and translates it to an 8-bit 
           value(from 0-255)
        */
        uint8_t parseBrightness(int slider_value);

        /* Accesses the complete NeoPixel state, then returns it */
        State_Settings_Struct& accessState();

        /* Uses the current colors vector to assign values to state variables */
        void syncStateWithVector();

        /* Updates the colors contained in the state tuple */
        void updateStateColors();

        /* Interprets the initial pixelOrderString string into an appropriate 
           length Array by breaking the string on each space, then returns 
           the visual index of the pixel, based on its actual electrical 
           order */
        void interpretPixelOrder(std::string str);

        /* Takes two uint32_t colors as arguments and sets the 2 colors displayed
           in the browser, then alternates the colors onto every pixel */
        void initializePixelColors(uint32_t color_1, uint32_t color_2);

        /* Adjusts the overall brightness of the string */
        static uint8_t adjustBrightness (
            uint8_t brightnessLevel
        );
        
        /* Steps proportionally to the difference of the values */
        uint8_t propStep (
            uint8_t start,
            uint8_t finish,
            uint8_t min_step,
            uint8_t max_step
        );

        /* Applies the propStep function to an entire RGB color */
        uint32_t propStepColor(
            uint32_t start,
            uint32_t finish,
            uint8_t min_step,
            uint8_t max_step
        );

        /* Function to fade from the current brightness up to the given value */
        void fadeInBrightness(uint8_t brightnessLevel, uint16_t wait = 50);

        /* Function to fade from the current brightness down to 1 */
        void fadeOutBrightness(uint16_t wait = 50);

        /* Function time specifically to fade in before the startup music of 
           the GameCube */
        void initialFadeIn();

        /* Transitions a single pixel from start color to finish color */
        void propTransitionSingle(
            uint16_t pixel,
            uint32_t start,
            uint32_t finish,
            uint16_t wait,
            uint8_t min_step,
            uint8_t max_step
        );

        /* Transitions all pixels from start color to finish color */
        void propTransitionAll(
            uint32_t finish_color,
            uint16_t wait = 50,
            uint8_t min_step = 2,
            uint8_t max_step = 10
        );

        /* Apply the proportional step function to brightness */
        void propTransitionBrightness(
            uint8_t finish_value,
            uint16_t wait = 50,
            uint8_t min_step = 2,
            uint8_t max_step = 10
        );

        /* handles repetitive tasks in alt_opp_fade. Alternates color 
           assignment */
        void altOppFadeHelper(
            uint32_t color_1,
            uint32_t color_2,
            uint16_t wait = 50,
            uint8_t min_step = 2,
            uint8_t max_step = 10
        );

        /* Fades between two opposing colors for a given number of repetitions */
        void altOppFade(
            uint32_t color_1,
            uint32_t color_2,
            uint8_t repetitions = 3,
            uint16_t wait = 50,
            uint8_t min_step = 2,
            uint8_t max_step = 10
            
        );

        /* Fill strip pixels one after another with a color. */
        void colorWipe(uint32_t color, int wait);

        /* Theater-marquee-style chasing lights. */
        void theaterChase(uint32_t color, int wait);

        /* Rainbow cycle along whole strip. Pass delay time (in ms) between 
            frames. */
        void rainbow(int wait);

        /* Rainbow-enhanced theater marquee. Pass delay time (in ms) between 
            frames. */
        void theaterChaseRainbow(int wait);

        /* Endpoint function intended to be interacted with by other projects. 
           Transitions a single pixel to a new color */
        void htmlSinglePixel(int pixel_num, uint32_t packed_color, int wait);

        /* Startup animation synced with the GameCube startup song */
        void gameCubeStartUp();

        /* Demonstration of basic usage */
        void demo_loop();

        /* For testing new animations */
        void test_loop();

        /* A wrapper to allow reference */
        static void test_wrapper(NeoPixelStrip *instance);

};
#endif