#pragma once
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "../tools/SimpleFSBuilder/SimpleFS.h"
struct SimpleFSContext {
    GlobalFSHeader *header;
	StoredFileEntry *entries;
	char *names, *data;
};
bool simplefs_init(struct SimpleFSContext *ctx, void *data);

void add_leading_zero(char * new_string, char * old_string);
void parse_colors_to_string(uint32_t packed_val, char * returned_color);


struct current_effect_params {
	int effect_index;
	uint32_t color_1;
	uint32_t color_2;
	uint8_t rgb_anim_speed;
	uint8_t rgb_anim_reps;
	uint8_t rgb_anim_brightness;
	uint32_t single_effect_color;
	uint8_t focused_port;
};
void single_led_wrapper();

void set_all_same_wrapper();
void rainbow_wrapper();
void rainbow_chase_wrapper();
void alt_opp_fade_wrapper();
void fade_in_wrapper();
void fade_out_wrapper();

void debug_printf(const char *format, ...);
int _write(int file, const void *data, int size);
void debug_write(const void *data, int size);
void initial_anim_wrapper();
void launch_server();
#ifdef __cplusplus
}
#endif