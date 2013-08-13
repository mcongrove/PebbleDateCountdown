/**
 * Copyright 2013 Matthew Congrove (http://github.com/mcongrove)
 * 
 * Please refer to config.h for license information
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "PDutils.h"
#include "config.h"

PBL_APP_INFO(APP_UUID, "Countdown", "Matthew Congrove", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_TYPE);

Window window;
TextLayer timeLayer;
TextLayer countdownLayer;
TextLayer labelLayerOne;
TextLayer labelLayerTwo;

char timeText[] = "00:00";
char countText[] = "00";

void itoa2(int num, char* buffer) {
	const char digits[10] = "0123456789";
	int iSize = 0;
	
	buffer[iSize] = ' ';
	
	if(num > 999) {
		buffer[0] = '9';
		buffer[1] = '9';
		buffer[2] = '9';
		
		iSize = 3;
	} else if(num > 99) {
		buffer[0] = digits[num / 100];
		
		if(num%100 > 9) {
			buffer[1] = digits[(num % 100) / 10];
		} else {
			buffer[1] = '0';
		}
		
		buffer[2] = digits[num % 10];
		
		iSize = 3;
	} else if(num > 9) {
		buffer[0] = digits[num / 10];
		buffer[1] = digits[num % 10];
		
		iSize = 2;
	} else {
		buffer[0] = digits[num % 10];
		
		iSize = 1;
	}
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)ctx;
	(void)t;
	
	PblTm now;
	PblTm event;
	
	time_t seconds_now;
	time_t seconds_event;
	int difference;
	char *time_format;
	static char countText[] = "00";
	
	get_time(&now);
	
	seconds_now = pmktime(&now);
	
	event.tm_year = EVENT_YEAR - 1900;
	event.tm_mon = EVENT_MONTH - 1;
	event.tm_mday = EVENT_DAY;
	event.tm_hour = EVENT_HOUR;
	event.tm_min = EVENT_MINUTE;
	event.tm_sec = EVENT_MINUTE;
	
	seconds_event = pmktime(&event);
	
	difference = ((((seconds_event - seconds_now) / 60) / 60) / 24);
	
	itoa2(difference, &countText[0]);
	
	text_layer_set_text(&countdownLayer, countText);
	
	if(clock_is_24h_style()) {
		time_format = "%R";
	} else {
		time_format = "%I:%M";
	}
	
	string_format_time(timeText, sizeof(timeText), time_format, &now);
	text_layer_set_text(&timeLayer, timeText);
}

void handle_init(AppContextRef ctx) {
	window_init(&window, "Countdown");
	window_set_fullscreen(&window, true);
	window_set_background_color(&window, GColorBlack);
	window_stack_push(&window, true);
	
	resource_init_current_app(&APP_RESOURCES);
	
	text_layer_init(&timeLayer, window.layer.frame);
	text_layer_set_text_color(&timeLayer, GColorWhite);
	text_layer_set_background_color(&timeLayer, GColorClear);
	text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
	layer_set_frame(&timeLayer.layer, GRect(0, 13, 144, 30));
	text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	layer_add_child(&window.layer, &timeLayer.layer);
	
	text_layer_init(&countdownLayer, window.layer.frame);
	text_layer_set_text_color(&countdownLayer, GColorWhite);
	text_layer_set_background_color(&countdownLayer, GColorClear);
	text_layer_set_text_alignment(&countdownLayer, GTextAlignmentCenter);
	layer_set_frame(&countdownLayer.layer, GRect(0, 56, 144, 55));
	text_layer_set_font(&countdownLayer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(&window.layer, &countdownLayer.layer);
	
	text_layer_init(&labelLayerOne, window.layer.frame);
	text_layer_set_text_color(&labelLayerOne, GColorWhite);
	text_layer_set_background_color(&labelLayerOne, GColorClear);
	text_layer_set_text_alignment(&labelLayerOne, GTextAlignmentCenter);
	layer_set_frame(&labelLayerOne.layer, GRect(0, 110, 144, 23));
	text_layer_set_text(&labelLayerOne, LABEL_ROW_1);
	text_layer_set_font(&labelLayerOne, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(&window.layer, &labelLayerOne.layer);
	
	text_layer_init(&labelLayerTwo, window.layer.frame);
	text_layer_set_text_color(&labelLayerTwo, GColorWhite);
	text_layer_set_background_color(&labelLayerTwo, GColorClear);
	text_layer_set_text_alignment(&labelLayerTwo, GTextAlignmentCenter);
	layer_set_frame(&labelLayerTwo.layer, GRect(0, 130, 144, 23));
	text_layer_set_text(&labelLayerTwo, LABEL_ROW_2);
	text_layer_set_font(&labelLayerTwo, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(&window.layer, &labelLayerTwo.layer);
	
	handle_minute_tick(ctx, NULL);
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.tick_info = {
			.tick_handler = &handle_minute_tick,
			.tick_units = MINUTE_UNIT
		}
	};
	
	app_event_loop(params, &handlers);
}