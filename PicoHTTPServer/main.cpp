#include "picow_neopixel_server.h"
#include "pico_neopixel_animations.h"
#include "pico/multicore.h"


extern "C"
{
#include <stdio.h>
#include <stdarg.h>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>
#include <hardware/watchdog.h>

#include <lwip/ip4_addr.h>
#include <lwip/netif.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "dhcpserver/dhcpserver.h"
#include "dns/dnsserver.h"
#include "server_settings.h"
#include "httpserver.h"
#include "../tools/SimpleFSBuilder/SimpleFS.h"

#define TEST_TASK_PRIORITY (tskIDLE_PRIORITY + 2UL)
//#define F_CPU 250000000

// Which pin on the Arduino is connected to the NeoPixels?
uint16_t LED_PIN =  28;

// How many NeoPixels are attached to the Pico?
uint16_t LED_COUNT = 5;

// What order should the NeoPixels follow, relative to the way they are 
// electrically sequenced? (The first NeoPixel of the electrical sequence is 
// number "0", next is "1", etc.) The last pixel should be the Power LED.
// The string should terminate with a trailing space!!
std::string PIXEL_ORDER = "4 0 1 2 3 ";

// Declare our NeoPixel strip object:
NeoPixelStrip npStrip(LED_COUNT, LED_PIN, PIXEL_ORDER);

struct SimpleFSContext s_SimpleFS;

bool simplefs_init(struct SimpleFSContext *ctx, void *data)
{
	ctx->header = (GlobalFSHeader *)data;
	if (ctx->header->Magic != kSimpleFSHeaderMagic)
		return false;
	ctx->entries = (StoredFileEntry *)(ctx->header + 1);
	ctx->names = (char *)(ctx->entries + ctx->header->EntryCount);
	ctx->data = (char *)(ctx->names + ctx->header->NameBlockSize);
	return true;
}

static bool do_retrieve_file(http_connection conn, enum http_request_type type, char *path, void *context)
{
	for (int i = 0; i < s_SimpleFS.header->EntryCount; i++)
	{
		if (!strcmp(s_SimpleFS.names + s_SimpleFS.entries[i].NameOffset, path))
		{
			http_server_send_reply(conn, 
				"200 OK", 
				s_SimpleFS.names + s_SimpleFS.entries[i].ContentTypeOffset,
				s_SimpleFS.data + s_SimpleFS.entries[i].DataOffset,
				s_SimpleFS.entries[i].FileSize);
			return true;
		}
	}
	
	return false;
}

void add_leading_zero(char * new_string, char * old_string) {
	char tmp_string[3];
	char * single_digit = old_string;
	tmp_string[0] = '0';
	tmp_string[1] = *single_digit;
	tmp_string[2] = '\0';
	strncpy(new_string, tmp_string, 3);
}

void parse_colors_to_string(uint32_t packed_val, char * returned_color) {
	uint8_t r_comp = (packed_val >> 16) & 0xff; // red
    uint8_t g_comp = (packed_val >> 8) & 0xff; // green
    uint8_t b_comp = packed_val  & 0xff; // blue

    char r_str[3], g_str[3], b_str[3], tmp_hex_string[11];
	sprintf(r_str, "%x", r_comp);
    sprintf(g_str, "%x", g_comp);
    sprintf(b_str, "%x", b_comp);
	char r_ret[3], g_ret[3], b_ret[3];
	if (r_comp < 16) {
		add_leading_zero(r_ret, r_str);
		strncpy (r_str, r_ret, 3);
	}
	if (g_comp < 16) {
		add_leading_zero(g_ret, g_str);
		strncpy (g_str, g_ret, 3);
	}
	if (b_comp < 16) {
		add_leading_zero(b_ret, b_str);
		strncpy (b_str, b_ret, 3);
	}
	sprintf(tmp_hex_string, "#%s%s%s", r_str, g_str, b_str);
	strncpy(returned_color, tmp_hex_string, 8);
	//needs to use strncpy() instead of returning
	//return tmp_hex_string;
}

static void parse_string_to_colors
	(uint8_t * r_comp, uint8_t * g_comp, uint8_t * b_comp, char * color_string)
{
	char r_value[3], g_value[3], b_value[3];
	strncpy (r_value, color_string, 2);
	strncpy (g_value, color_string+2, 2);
	strncpy (b_value, color_string+4, 2);
	r_value[2]='\0';
	g_value[2]='\0';
	b_value[2]='\0';
	int r_num = (int)strtol(r_value, NULL, 16);
	int g_num = (int)strtol(g_value, NULL, 16);
	int b_num = (int)strtol(b_value, NULL, 16);
	*r_comp = uint8_t(r_num);
	*g_comp = uint8_t(g_num);
	*b_comp = uint8_t(b_num);
}

static void parse_state_response(
	NeoPixelStrip::State_Settings_Struct * state_settings,
	http_write_handle * reply
){
	printf("Parsing State for Response.\n");
	http_server_write_reply(*reply, ",\"effect_index\": \"%d\"", *state_settings->effect_index);
	char ec1_hex[8], ec2_hex[8];
	parse_colors_to_string((*state_settings->effect_color_1), ec1_hex);
	parse_colors_to_string((*state_settings->effect_color_2), ec2_hex);
	http_server_write_reply(*reply, ",\"effect_color_1\": \"%s\"", ec1_hex);
	http_server_write_reply(*reply, ",\"effect_color_2\": \"%s\"", ec2_hex);
	http_server_write_reply(*reply, ",\"rgb_anim_speed\": \"%d\"", *state_settings->rgb_anim_speed);
	http_server_write_reply(*reply, ",\"rgb_anim_reps\": \"%d\"", *state_settings->rgb_anim_reps);
	http_server_write_reply(*reply, ",\"rgb_anim_brightness\": \"%d\"", int((*state_settings->rgb_anim_brightness)/2.5));
	char pwr_hex[8], pt1_hex[8], pt2_hex[8], pt3_hex[8], pt4_hex[8];
	parse_colors_to_string((*state_settings->power_rgb), pwr_hex);
	parse_colors_to_string((*state_settings->port_rgb_1), pt1_hex);
	parse_colors_to_string((*state_settings->port_rgb_2), pt2_hex);
	parse_colors_to_string((*state_settings->port_rgb_3), pt3_hex);
	parse_colors_to_string((*state_settings->port_rgb_4), pt4_hex);
	http_server_write_reply(*reply, ",\"power_rgb\": \"%s\"", pwr_hex);
	http_server_write_reply(*reply, ",\"port_rgb_1\": \"%s\"", pt1_hex);
	http_server_write_reply(*reply, ",\"port_rgb_2\": \"%s\"", pt2_hex);
	http_server_write_reply(*reply, ",\"port_rgb_3\": \"%s\"", pt3_hex);
	http_server_write_reply(*reply, ",\"port_rgb_4\": \"%s\"", pt4_hex);
	http_server_end_write_reply(*reply, "}");
	printf("Parsing Complete.\n");
}



static char *parse_server_settings(http_connection conn, pico_server_settings *settings)
{
	bool has_password = false, use_domain = false, use_second_ip = false;
	bool bad_password = false, bad_domain = false;
	
	for (;;)
	{
		char *line = http_server_read_post_line(conn);
		if (!line)
			break;
				
		char *p = strchr(line, '=');
		if (!p)
			continue;
		*p++ = 0;
		if (!strcasecmp(line, "has_password")) 
			has_password = !strcasecmp(p, "true") || p[0] == '1';
		else if (!strcasecmp(line, "use_domain")) 
			use_domain = !strcasecmp(p, "true") || p[0] == '1';
		else if (!strcasecmp(line, "use_second_ip")) 
			use_second_ip = !strcasecmp(p, "true") || p[0] == '1';
		else if (!strcasecmp(line, "dns_ignores_network_suffix")) 
			settings->dns_ignores_network_suffix = !strcasecmp(p, "true") || p[0] == '1';
		else if (!strcasecmp(line, "ssid")) 
		{
			if (strlen(p) >= sizeof(settings->network_name))
				return "SSID too long";
			if (!p[0])
				return "missing SSID";
			strcpy(settings->network_name, p);
		}
		else if (!strcasecmp(line, "password")) 
		{
			if (strlen(p) >= sizeof(settings->network_password))
				bad_password = true;
			else
				strcpy(settings->network_password, p);
		}
		else if (!strcasecmp(line, "hostname")) 
		{
			if (strlen(p) >= sizeof(settings->hostname))
				return "hostname too long";
			if (!p[0])
				return "missing hostname";
			strcpy(settings->hostname, p);
		}
		else if (!strcasecmp(line, "domain")) 
		{
			if (strlen(p) >= sizeof(settings->domain_name))
				bad_domain = true;
			else
				strcpy(settings->domain_name, p);
		}
		else if (!strcasecmp(line, "ipaddr")) 
		{
			settings->ip_address = ipaddr_addr(p);
			if (!settings->ip_address || settings->ip_address == -1)
				return "invalid IP address";
		}
		else if (!strcasecmp(line, "netmask")) 
		{
			settings->network_mask = ipaddr_addr(p);
			if (!settings->network_mask || settings->network_mask == -1)
				return "invalid network mask";
		}
		else if (!strcasecmp(line, "ipaddr2")) 
		{
			settings->secondary_address = ipaddr_addr(p);
		}
	}
	
	if (!has_password)
		memset(settings->network_password, 0, sizeof(settings->network_password));
	else if (bad_password)
		return "password too long";
	
	if (!use_domain)
		memset(settings->domain_name, 0, sizeof(settings->domain_name));
	else if (bad_domain)
		return "domain too long";
	
	if (!use_second_ip)
		settings->secondary_address = 0;
	else if (!settings->secondary_address || settings->secondary_address == -1)
		return "invalid secondary IP address";
	
	return NULL;
}

struct current_effect_params currentParams;

void single_led_wrapper() {
	npStrip.htmlSinglePixel(
		(currentParams.focused_port - 1), currentParams.single_effect_color,
		currentParams.rgb_anim_speed
	);
}

static bool do_handle_single_call(http_connection conn, enum http_request_type type, char *path, void *context)
{
	// Because this isn't an endpoint-based differentiation, we check for a POST.
	if (type == HTTP_POST)
	{		
		for (;;) 
		{
			// Next, we read the post to see which led is being set to which color.
			char *line = http_server_read_post_line(conn);
			// To see what the content of a POSTed line is
			printf("The line is: %s\n", line);
			if (!line)
				break;
			
			// For destruction
			char* duped_line = strdup(line);
			//printf("The duped line is now: %s\n", duped_line);
			char *led_param = strtok(duped_line, "&");
			//printf("The led_param is: %s\n", led_param);
			char *color_param = strchr(line, '&') + 1;
			//printf("The color_param is: %s\n", color_param);
							
			char *led_number = strchr(led_param, '=') + 1;
			
			char *color_value = strchr(color_param, '=') + 1;
			
			printf("The led #%s should become %s-colored\n", led_number, color_value);
			
			*color_value++;
			uint8_t r_comp, g_comp, b_comp;
			parse_string_to_colors(&r_comp, &g_comp, &b_comp, color_value);
			*color_value--;
		
			printf("The color components as ints are R:%d G:%d B:%d\n", r_comp, g_comp, b_comp);
			// Set params for eventual effect call now
			currentParams.focused_port = atoi(led_number);
			currentParams.single_effect_color = npStrip.packColor(r_comp, g_comp, b_comp);
			currentParams.rgb_anim_speed = npStrip.parseSpeed(*npStrip.accessState().rgb_anim_speed);
			
			// Send NeoPixel command here
			single_led_wrapper();

			// This json/text reply will not depend on the pico_neopixel_animations response
			http_write_handle reply = http_server_begin_write_reply(conn, "200 OK", "text/json");	
			http_server_write_reply(reply, "{\"effect_type\": \"single\"");
			if (*led_number == 5) {
				http_server_write_reply(reply, ",\"port_id\": \"power_rgb\"");
			} else {
				http_server_write_reply(reply, ",\"port_id\": \"port_rgb_%s\"", led_number);
			}
			http_server_write_reply(reply, ",\"port_color\": \"%s\"", color_value);
			http_server_end_write_reply(reply, "}");
			return true;
			
		}		
	}
	
	return false;
}

//Wrapper functions for running NeoPixel animations
void set_all_same_wrapper(){
	npStrip.brightness = currentParams.rgb_anim_brightness;
	printf("%d", currentParams.rgb_anim_brightness);
	npStrip.propTransitionAll(
		currentParams.color_1, currentParams.rgb_anim_speed
	);
}

void rainbow_wrapper() {
	npStrip.brightness = currentParams.rgb_anim_brightness;
	for (int i=0; i<currentParams.rgb_anim_reps; i++) {
		npStrip.rainbow(currentParams.rgb_anim_speed);
	}
}

void rainbow_chase_wrapper() {
	npStrip.brightness = currentParams.rgb_anim_brightness;
	for (int i=0; i<=currentParams.rgb_anim_reps; i++) {
		npStrip.theaterChaseRainbow(currentParams.rgb_anim_speed);
	}
}

void alt_opp_fade_wrapper() {
	npStrip.brightness = currentParams.rgb_anim_brightness;
	npStrip.altOppFade(
		currentParams.color_1,
		currentParams.color_2,
		currentParams.rgb_anim_reps,
		currentParams.rgb_anim_speed
	);
}

void fade_in_wrapper() {
	npStrip.fadeOutBrightness(2);
	npStrip.propTransitionAll(
		currentParams.color_1, 
		currentParams.rgb_anim_speed,
		2,
		255
	);
	npStrip.fadeInBrightness(
		currentParams.rgb_anim_brightness,
		currentParams.rgb_anim_speed
	);
}

void fade_out_wrapper() {
	npStrip.fadeOutBrightness(currentParams.rgb_anim_speed);
}

static bool do_handle_effect_call(http_connection conn, enum http_request_type type, char *path, void *context)
{
	// Because this isn't an endpoint-based differentiation, we check for a POST.
	if (type == HTTP_POST)
	{		
		for (;;) 
		{
			// Read the post to see which led is being set to which color.
			char *line = http_server_read_post_line(conn);
			// See the content of the POSTed line
			printf("The line is: %s\n", line);
			if (!line)
				break;
		
			// Determine which effect based on index
			// All effects contain "speed" except for initialization
			if (!strstr(line, "speed"))
			{
				char *init_effect_index = strchr(line, '=');
				init_effect_index++;
				int current_index = atoi(init_effect_index);
				printf("The current effect index is: %d (Initialize)\n", current_index);
				NeoPixelStrip::State_Settings_Struct& returnedState = npStrip.accessState();
				printf("Accessed index: %d\n", *returnedState.effect_index);
				http_write_handle reply = http_server_begin_write_reply(conn, "200 OK", "text/json");
				http_server_write_reply(reply, "{\"effect_type\": \"initial\"");
				// call to parse 
				parse_state_response(&returnedState, &reply);
			} else {
				char *effect_index_pos = strchr(line, '=');
				char effect_index[2];
				strncpy (effect_index, effect_index_pos+1, 1);
				effect_index[1] ='\0';
				int current_index = atoi(effect_index);
				printf("The current effect index is: %d\n", current_index);
				http_write_handle reply = http_server_begin_write_reply(conn, "200 OK", "text/json");
				http_server_write_reply(reply, "{\"effect_type\": \"pattern\"");
				switch (current_index) {
					//set-all
					case 0:
						// color speed brightness
						// ^     ^     ^  need to parse 'line' to These
						{
							char *remaining_params = strchr(line, '&') + 1;
							printf("remaining line: %s\n", remaining_params);
							char *effect_color_buffer = strchr(remaining_params, '=') + 1;
							char effect_color[8], speed_value[4], brightness_value[4];
							strncpy(effect_color, effect_color_buffer, 7);
							effect_color[7] = '\0';
							printf("effect color: %s\n", effect_color);
							char *speed_and_brightness = strchr(remaining_params, '&') + 1;
							char *speed_value_buffer = strchr(speed_and_brightness, '=') + 1;
							strncpy(speed_value, speed_value_buffer, 3);
							if (speed_value[1] == '&') {
								speed_value[1] = '\0';
							} else if (speed_value[2] == '&') {
								speed_value[2] = '\0';
							} else {
								speed_value[3] = '\0';
							}
							printf("Speed Value: %s\n", speed_value);
							char *brightness_value_buffer = strrchr(speed_and_brightness, '=') + 1;
							strcpy(brightness_value, brightness_value_buffer);
							printf("Brightness Value: %s\n", brightness_value);
			
							uint8_t r_comp, g_comp, b_comp;
							parse_string_to_colors(&r_comp, &g_comp, &b_comp, effect_color + 1);

							// Set params for effect call now
							currentParams.color_1 = npStrip.packColor(r_comp, g_comp, b_comp);
							currentParams.rgb_anim_speed = npStrip.parseSpeed(atoi(speed_value));
							currentParams.rgb_anim_brightness = npStrip.parseBrightness(atoi(brightness_value));
							
							// Launch Neopixel animation here
							set_all_same_wrapper();
							
							parse_state_response(&npStrip.accessState(), &reply);
							break;
						}
						
					//rainbow
					case 1:
						{
							// speed reps brightness
							char *remaining_params = strchr(line, '&') + 1;
							printf("remaining line: %s\n", remaining_params);
							char *speed_value_buffer = strchr(remaining_params, '=') + 1;
							char speed_value[4], reps_value[3], brightness_value[4];
							strncpy(speed_value, speed_value_buffer, 3);
							if (speed_value[1] == '&') {
								speed_value[1] = '\0';
							} else if (speed_value[2] == '&') {
								speed_value[2] = '\0';
							} else {
								speed_value[3] = '\0';
							}
							//printf("speed value: %s %d\n", speed_value, atoi(speed_value));
							char *reps_and_brightness = strchr(remaining_params, '&') + 1;
							//printf("reps_and_brightness: %s\n", reps_and_brightness);
							char *reps_value_buffer = strchr(reps_and_brightness, '=') + 1;
							strncpy(reps_value, reps_value_buffer, 2);
							if (reps_value[1] == '&') {
								reps_value[1] = '\0';
							} else {
								reps_value[2] = '\0';
							}
							//printf("reps value: %s %d\n", reps_value, atoi(reps_value));
							char *brightness_buffer = strrchr(reps_and_brightness, '=') + 1;
							strcpy(brightness_value, brightness_buffer);

							//Set params for effect call
							currentParams.rgb_anim_speed = npStrip.parseSpeed(atoi(speed_value));
							currentParams.rgb_anim_reps = atoi(reps_value);
							currentParams.rgb_anim_brightness = npStrip.parseBrightness(atoi(brightness_value));

							//Launch NeoPixel animation here
							rainbow_wrapper();
							
							parse_state_response(&npStrip.accessState(), &reply);
							break;
						}
					// //rainbow chase
					case 2:
						{
							// speed reps brightness
							char *remaining_params = strchr(line, '&') + 1;
							printf("remaining line: %s\n", remaining_params);
							char *speed_value_buffer = strchr(remaining_params, '=') + 1;
							char speed_value[4], reps_value[3], brightness_value[4];
							strncpy(speed_value, speed_value_buffer, 3);
							if (speed_value[1] == '&') {
								speed_value[1] = '\0';
							} else if (speed_value[2] == '&') {
								speed_value[2] = '\0';
							} else {
								speed_value[3] = '\0';
							}
							char *reps_and_brightness = strchr(remaining_params, '&') + 1;
							char *reps_value_buffer = strchr(reps_and_brightness, '=') + 1;
							strncpy(reps_value, reps_value_buffer, 2);
							if (reps_value[1] == '&') {
								reps_value[1] = '\0';
							} else {
								reps_value[2] = '\0';
							}
							char *brightness_buffer = strrchr(reps_and_brightness, '=') + 1;
							strcpy(brightness_value, brightness_buffer);

							// Set effect params
							currentParams.rgb_anim_speed = npStrip.parseSpeed(atoi(speed_value));
							currentParams.rgb_anim_reps = atoi(reps_value);
							currentParams.rgb_anim_brightness = npStrip.parseBrightness(atoi(brightness_value));
							
							// Launch NeoPixel animation here
							rainbow_chase_wrapper();

							parse_state_response(&npStrip.accessState(), &reply);
							break;
						}
					// //alt-fade
					case 3:
						{
							//color1 color2 speed reps brightness
							char *remaining_params = strchr(line, '&') + 1;
							printf("remaining line: %s\n", remaining_params);
							char *effect_color_1_buffer = strchr(remaining_params, '=') + 1;
							char effect_color_1[8], effect_color_2[8], speed_value[4], 
								 reps_value[3], brightness_value[4];
							strncpy(effect_color_1, effect_color_1_buffer, 7);
							effect_color_1[7] = '\0';
							char *four_remaining_params = strchr(remaining_params, '&') + 1;
							char *effect_color_2_buffer = strchr(four_remaining_params, '=') + 1;
							strncpy(effect_color_2, effect_color_2_buffer, 7);
							effect_color_2[7] = '\0';
							char *three_remaining_params = strchr(four_remaining_params, '&') + 1;
							char *speed_value_buffer = strchr(three_remaining_params, '=') + 1;
							strncpy(speed_value, speed_value_buffer, 3);
							if (speed_value[1] == '&') {
								speed_value[1] = '\0';
							} else if (speed_value[2] == '&') {
								speed_value[2] = '\0';
							} else {
								speed_value[3] = '\0';
							}
							char *reps_and_brightness = strchr(three_remaining_params, '&') + 1;
							char *reps_value_buffer = strchr(reps_and_brightness, '=') + 1;
							strncpy(reps_value, reps_value_buffer, 2);
							if (reps_value[1] == '&') {
								reps_value[1] = '\0';
							} else {
								reps_value[2] = '\0';
							}
							char *brightness_buffer = strrchr(reps_and_brightness, '=') + 1;
							strcpy(brightness_value, brightness_buffer);

							uint8_t r_comp_1, g_comp_1, b_comp_1, r_comp_2, g_comp_2, b_comp_2;
							parse_string_to_colors(&r_comp_1, &g_comp_1, &b_comp_1, effect_color_1 + 1);
							parse_string_to_colors(&r_comp_2, &g_comp_2, &b_comp_2, effect_color_2 + 1);
							
							//Set effect params
							currentParams.color_1 = npStrip.packColor(r_comp_1, g_comp_1, b_comp_1);
							currentParams.color_2 = npStrip.packColor(r_comp_2, g_comp_2, b_comp_2);
							currentParams.rgb_anim_speed = npStrip.parseSpeed(atoi(speed_value));
							currentParams.rgb_anim_reps = atoi(reps_value);
							currentParams.rgb_anim_brightness = npStrip.parseBrightness(atoi(brightness_value));

							//Launch NeoPixel animation	here
							alt_opp_fade_wrapper();

							parse_state_response(&npStrip.accessState(), &reply);
							break;
						}
					// //fade-in
					case 4:
						// color speed brightness
						{
							char *remaining_params = strchr(line, '&') + 1;
							printf("remaining line: %s\n", remaining_params);
							char *effect_color_buffer = strchr(remaining_params, '=') + 1;
							char effect_color[8], speed_value[4], brightness_value[4];
							strncpy(effect_color, effect_color_buffer, 7);
							effect_color[7] = '\0';
							char *speed_and_brightness = strchr(remaining_params, '&') + 1;
							char *speed_value_buffer = strchr(speed_and_brightness, '=') + 1;
							strncpy(speed_value, speed_value_buffer, 3);
							if (speed_value[1] == '&') {
								speed_value[1] = '\0';
							} else if (speed_value[2] == '&') {
								speed_value[2] = '\0';
							} else {
								speed_value[3] = '\0';
							}
							printf("Speed Value: %s\n", speed_value);
							char *brightness_value_buffer = strrchr(speed_and_brightness, '=') + 1;
							strcpy(brightness_value, brightness_value_buffer);
							printf("Brightness Value: %s\n", brightness_value);
			
							uint8_t r_comp, g_comp, b_comp;
							parse_string_to_colors(&r_comp, &g_comp, &b_comp, effect_color + 1);
							
							//set effect params
							currentParams.color_1 = npStrip.packColor(r_comp, g_comp, b_comp);
							currentParams.rgb_anim_speed = npStrip.parseSpeed(atoi(speed_value));
							currentParams.rgb_anim_brightness = npStrip.parseBrightness(atoi(brightness_value));

							//launch Neopixel animation here
							fade_in_wrapper();
							
							parse_state_response(&npStrip.accessState(), &reply);
							break;
						}
					// //fade-out
					case 5:
						{
							// speed
							char *remaining_params = strchr(line, '&') + 1;
							char *speed_value_buffer = strchr(remaining_params, '=') + 1;
							char speed_value[4];
							strcpy(speed_value, speed_value_buffer);

							//set effect params
							currentParams.rgb_anim_speed = npStrip.parseSpeed(atoi(speed_value));

							//launch NeoPixel animation here
							fade_out_wrapper();

							parse_state_response(&npStrip.accessState(), &reply);
							break;
						}
					// // Error?
					default:
						http_write_handle reply = http_server_begin_write_reply(conn, "200 OK", "text/json");
						http_server_write_reply(reply, "{");
						parse_state_response(&npStrip.accessState(), &reply);
						break;
				}
			}

			return true;
			
		}		
	}
	
	return false;
}

static void set_secondary_ip_address(int address)
{
	/************************************ !!! WARNING !!! ************************************
	 * If you get an 'undefined reference to ip4_secondary_ip_address' error here,			 *
	 * you need to patch your lwIP using the lwip_patch/lwip.patch file from this repository.*
	 * This ensures that this device can pretend to be a router redirecting requests to		 *
	 * external IPs to its login page, so the OS can automatically navigate there.			 *
	 *****************************************************************************************/
	
	extern int ip4_secondary_ip_address;
	ip4_secondary_ip_address = address;
}

void initial_anim_wrapper() {
	
	// Set system clock to default 125Mhz
    set_sys_clock_khz(125000, true);

	npStrip.test_loop();
	
	// Return system clock to 250Mhz for PicoBoot
    // Read Below!
	// // set_sys_clock_khz(250000, true);

	// May be causing Wifi issues, doesn't seem to hurt PicoBoot to leave 
	// it at the normal speed. Just in case, this initial anim is given a
	// 200-millisecond delay before executing, so that any time-sensitive
	// operations the PicoBoot code may need to execute are allowed to 
	// run to completion.
}

static void main_task(__unused void *params)
{
	if (cyw43_arch_init())
	{
		printf("failed to initialise\n");
		return;
	}
	
	extern void *_binary_www_fs_start;
	if (!simplefs_init(&s_SimpleFS, &_binary_www_fs_start))
	{
		printf("missing/corrupt FS image\n");
		return;
	}

	//power_led could be turned on here, if desired.
	//  if no power LED is desired, comment this line out
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

	// Delay to sync startup animation with GameCube
	// The current timing matches a GameCube that is NOT checking for discs
	// Add this delay if the system is checking for discs
	npStrip.delay(900);

	initial_anim_wrapper();
	
	const pico_server_settings *settings = get_pico_server_settings();

	cyw43_arch_enable_ap_mode(settings->network_name, settings->network_password, settings->network_password[0] ? CYW43_AUTH_WPA2_MIXED_PSK : CYW43_AUTH_OPEN);

	struct netif *netif = netif_default;
	ip4_addr_t addr = { .addr = settings->ip_address }, mask = { .addr = settings->network_mask };
	
	netif_set_addr(netif, &addr, &mask, &addr);
	
	// Start the dhcp server
	static dhcp_server_t dhcp_server;
	dhcp_server_init(&dhcp_server, &netif->ip_addr, &netif->netmask, settings->domain_name);
	dns_server_init(netif->ip_addr.addr, settings->secondary_address, settings->hostname, settings->domain_name, settings->dns_ignores_network_suffix);
	set_secondary_ip_address(settings->secondary_address);
	http_server_instance server = http_server_create(settings->hostname, settings->domain_name, 4, 4096);
	static http_zone zone1, zone2, zone3;
	http_server_add_zone(server, &zone1, "", do_retrieve_file, NULL);
	http_server_add_zone(server, &zone2, "/singleled", do_handle_single_call, NULL);
	http_server_add_zone(server, &zone3, "/effect", do_handle_effect_call, NULL);
	vTaskDelete(NULL);
}

xSemaphoreHandle s_PrintfSemaphore;

void debug_printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	xSemaphoreTake(s_PrintfSemaphore, portMAX_DELAY);
	vprintf(format, args);
	va_end(args);
	xSemaphoreGive(s_PrintfSemaphore);
}

// Avoids implicit declaration below
int _write(int file, const void *data, int size);

void debug_write(const void *data, int size)
{
	xSemaphoreTake(s_PrintfSemaphore, portMAX_DELAY);
	// Implicitly declared functions receive the int return type with no parameters
	_write(1, data, size);
	xSemaphoreGive(s_PrintfSemaphore);
}

void launch_server()
{
	// Delay to all PicoBoot code to finish
	npStrip.delay(200);
	stdio_init_all();
	TaskHandle_t task;
	s_PrintfSemaphore = xSemaphoreCreateMutex();
	xTaskCreate(main_task, "MainThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &task);
	vTaskStartScheduler();
}
}