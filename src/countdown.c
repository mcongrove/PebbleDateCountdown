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

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 5

PBL_APP_INFO(APP_UUID, "Countdown", "Matthew Congrove", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_TYPE);

Window window_root;
Window window_menu;
TextLayer label_time;
TextLayer label_countdown;
TextLayer label_row_one;
TextLayer label_row_two;
SimpleMenuLayer menu_layer;
SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
SimpleMenuItem menu_items[NUM_MENU_ITEMS];

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
	
	text_layer_set_text(&label_countdown, countText);
	
	if(clock_is_24h_style()) {
		time_format = "%R";
	} else {
		time_format = "%I:%M";
	}
	
	string_format_time(timeText, sizeof(timeText), time_format, &now);
	text_layer_set_text(&label_time, timeText);
}

void menu_select_callback(int index, void *ctx) {
	switch(index) {
		case 0:
			menu_items[index].subtitle = "Hi";
			break;
	}
	
	layer_mark_dirty(simple_menu_layer_get_layer(&menu_layer));
}

void menu_button_back(ClickRecognizerRef recognizer, Window *win) {
	window_stack_pop(true);
}

void menu_window_click_provider(ClickConfig **config, Window *win) {
	config[BUTTON_ID_BACK]->click.handler = (ClickHandler) menu_button_back;
	config[BUTTON_ID_BACK]->long_click.handler = (ClickHandler) menu_button_back;
}

void menu_open() {
	window_init(&window_menu, "Menu");
	window_set_fullscreen(&window_menu, true);
	window_set_background_color(&window_menu, GColorBlack);
	window_stack_push(&window_menu, true);
	window_set_click_config_provider(&window_menu, (ClickConfigProvider) menu_window_click_provider);
	
	int counter = 0;
	
	menu_items[counter++] = (SimpleMenuItem){
		.title = "Month",
		.subtitle = "Hi",
		.callback = NULL
	};
	
	menu_items[counter++] = (SimpleMenuItem){
		.title = "Day",
		.subtitle = "Hi",
		.callback = NULL
	};
	
	menu_items[counter++] = (SimpleMenuItem){
		.title = "Year",
		.subtitle = "Hi",
		.callback = NULL
	};
	
	menu_items[counter++] = (SimpleMenuItem){
		.title = "Hour",
		.subtitle = "Hi",
		.callback = NULL
	};
	
	menu_items[counter++] = (SimpleMenuItem){
		.title = "Minute",
		.subtitle = "Hi",
		.callback = NULL
	};
	
	menu_sections[0] = (SimpleMenuSection){
		.num_items = NUM_MENU_ITEMS,
		.items = menu_items,
	};
	
	simple_menu_layer_init(&menu_layer, window_menu.layer.frame, &window_menu, menu_sections, NUM_MENU_SECTIONS, NULL);
	
	layer_add_child(&window_menu.layer, simple_menu_layer_get_layer(&menu_layer));
}

void root_button_select(ClickRecognizerRef recognizer, Window *win) {
	menu_open();
}

void root_window_click_provider(ClickConfig **config, Window *win) {
	config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) root_button_select;
	config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) root_button_select;
}

void window_load(Window *window) {
	text_layer_init(&label_time, window_root.layer.frame);
	text_layer_set_text_color(&label_time, GColorWhite);
	text_layer_set_background_color(&label_time, GColorClear);
	text_layer_set_text_alignment(&label_time, GTextAlignmentCenter);
	layer_set_frame(&label_time.layer, GRect(0, 13, 144, 30));
	text_layer_set_font(&label_time, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	layer_add_child(&window_root.layer, &label_time.layer);
	
	text_layer_init(&label_countdown, window_root.layer.frame);
	text_layer_set_text_color(&label_countdown, GColorWhite);
	text_layer_set_background_color(&label_countdown, GColorClear);
	text_layer_set_text_alignment(&label_countdown, GTextAlignmentCenter);
	layer_set_frame(&label_countdown.layer, GRect(0, 56, 144, 55));
	text_layer_set_font(&label_countdown, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(&window_root.layer, &label_countdown.layer);
	
	text_layer_init(&label_row_one, window_root.layer.frame);
	text_layer_set_text_color(&label_row_one, GColorWhite);
	text_layer_set_background_color(&label_row_one, GColorClear);
	text_layer_set_text_alignment(&label_row_one, GTextAlignmentCenter);
	layer_set_frame(&label_row_one.layer, GRect(0, 110, 144, 23));
	text_layer_set_text(&label_row_one, LABEL_ROW_1);
	text_layer_set_font(&label_row_one, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(&window_root.layer, &label_row_one.layer);
	
	text_layer_init(&label_row_two, window_root.layer.frame);
	text_layer_set_text_color(&label_row_two, GColorWhite);
	text_layer_set_background_color(&label_row_two, GColorClear);
	text_layer_set_text_alignment(&label_row_two, GTextAlignmentCenter);
	layer_set_frame(&label_row_two.layer, GRect(0, 130, 144, 23));
	text_layer_set_text(&label_row_two, LABEL_ROW_2);
	text_layer_set_font(&label_row_two, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(&window_root.layer, &label_row_two.layer);
}

void handle_init(AppContextRef ctx) {
	resource_init_current_app(&APP_RESOURCES);
	
	handle_minute_tick(ctx, NULL);
	
	window_init(&window_root, "Countdown");
	window_set_fullscreen(&window_root, true);
	window_set_background_color(&window_root, GColorBlack);
	window_stack_push(&window_root, true);
	window_set_click_config_provider(&window_root, (ClickConfigProvider) root_window_click_provider);
	window_set_window_handlers(&window_root, (WindowHandlers){
		.load = window_load
	});
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