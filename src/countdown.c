/**
 * This code is provided under the Apache 2.0 license.
 * Please read the LICENSE.md file for more information
 * 
 * Copyright (c) 2013 Matthew Congrove (http://github.com/mcongrove)
 * 
 * Utilizes the PDUtils library by Peter Hardy (http://github.com/phardy)
 */
#include <pebble.h>
#include <PDUtils.h>

static char THEME[6] = "dark";
static int EVENT_MONTH = 1;
static int EVENT_DAY = 1;
static int EVENT_YEAR = 14;
static int EVENT_HOUR = 12;
static int EVENT_MINUTE = 0;
static char EVENT_LABEL[255] = "the event";

Window *window;
TextLayer *label_layer_time;
TextLayer *label_layer_countdown;
TextLayer *label_layer_text_top;
TextLayer *label_layer_text_bottom;
InverterLayer *inverter_layer;
char timeText[] = "00:00";
char countText[4];
char* labelText;

enum {
	KEY_THEME = 0x0,
	KEY_LABEL = 0x1,
	KEY_MONTH = 0x2,
	KEY_DAY = 0x3,
	KEY_YEAR = 0x4,
	KEY_HOUR = 0x5,
	KEY_MINUTE = 0x6
};

static void set_theme() {
	if (persist_exists(KEY_THEME)) {
		persist_read_string(KEY_THEME, THEME, 6);
	}
	
	APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED THEME: %s", THEME);
	
	bool hide = strcmp(THEME, "light") == 0 ? true : false;
	
	layer_set_hidden(inverter_layer_get_layer(inverter_layer), hide);
}

static void set_label() {
	if (persist_exists(KEY_LABEL)) {
		persist_read_string(KEY_LABEL, EVENT_LABEL, 255);
	}
	
	APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED LABEL: %s", EVENT_LABEL);
	
	text_layer_set_text(label_layer_text_bottom, EVENT_LABEL);
}

static void set_date() {
	if (persist_exists(KEY_DAY)
			&& persist_exists(KEY_MONTH)
			&& persist_exists(KEY_YEAR)
			&& persist_exists(KEY_HOUR)
			&& persist_exists(KEY_MINUTE)) {
		EVENT_DAY = persist_read_int(KEY_DAY);
		EVENT_MONTH = persist_read_int(KEY_MONTH);
		EVENT_YEAR = persist_read_int(KEY_YEAR);
		EVENT_HOUR = persist_read_int(KEY_HOUR);
		EVENT_MINUTE = persist_read_int(KEY_MINUTE);
		
		/*
		APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED DAY: %d", EVENT_DAY);
		APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED MONTH: %d", EVENT_MONTH);
		APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED YEAR: %d", EVENT_YEAR);
		APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED HOUR: %d", EVENT_HOUR);
		APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED MINUTE: %d", EVENT_MINUTE);
		*/
	}
}

static void calculate_countdown() {
	time_t t = time(NULL);
	struct tm *now = localtime(&t);
	
	char *time_format;
	static char countText[] = "";
	
	// Set the current time display
	if(clock_is_24h_style()) {
		time_format = "%R";
	} else {
		time_format = "%I:%M";
	}
	
	strftime(timeText, sizeof(timeText), time_format, now);
	
	text_layer_set_text(label_layer_time, timeText);
	
	// Set the current time
	time_t seconds_now = p_mktime(now);
	
	// Set the event time
	set_date();
	
	now->tm_year = EVENT_YEAR + 100;
	now->tm_mon = EVENT_MONTH - 1;
	now->tm_mday = EVENT_DAY;
	now->tm_hour = EVENT_HOUR;
	now->tm_min = EVENT_MINUTE;
	now->tm_sec = 0;
	
	time_t seconds_event = p_mktime(now);
	
	// Determine the time difference
	int difference = ((((seconds_event - seconds_now) / 60) / 60) / 24);
	
	if(difference < 0) {
		difference = 0;
	}
	
	// Set the countdown display
	snprintf(countText, 100, "%d", difference);
	
	text_layer_set_text(label_layer_countdown, countText);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	Tuple *theme_tuple = dict_find(iter, KEY_THEME);
	Tuple *label_tuple = dict_find(iter, KEY_LABEL);
	Tuple *month_tuple = dict_find(iter, KEY_MONTH);
	Tuple *day_tuple = dict_find(iter, KEY_DAY);
	Tuple *year_tuple = dict_find(iter, KEY_YEAR);
	Tuple *hour_tuple = dict_find(iter, KEY_HOUR);
	Tuple *minute_tuple = dict_find(iter, KEY_MINUTE);
	
	theme_tuple ? persist_write_string(KEY_THEME, theme_tuple->value->cstring) : false;
	label_tuple ? persist_write_string(KEY_LABEL, label_tuple->value->cstring) : false;
	month_tuple ? persist_write_int(KEY_MONTH, month_tuple->value->uint8) : false;
	day_tuple ? persist_write_int(KEY_DAY, day_tuple->value->uint8) : false;
	year_tuple ? persist_write_int(KEY_YEAR, year_tuple->value->uint8) : false;
	hour_tuple ? persist_write_int(KEY_HOUR, hour_tuple->value->uint8) : false;
	minute_tuple ? persist_write_int(KEY_MINUTE, minute_tuple->value->uint8) : false;
	
	set_theme();
	set_label();
	set_date();
	calculate_countdown();
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
	
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	calculate_countdown();
}

static void init() {
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_open(128, 0);
	
	window = window_create();
	window_set_background_color(window, GColorWhite);
	window_set_fullscreen(window, true);
	window_stack_push(window, true);
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	// Add time layer
	label_layer_time = text_layer_create(GRect(0, 13, 144, 30));
	text_layer_set_text_color(label_layer_time, GColorBlack);
	text_layer_set_background_color(label_layer_time, GColorClear);
	text_layer_set_text_alignment(label_layer_time, GTextAlignmentCenter);
	text_layer_set_font(label_layer_time, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_time));
	
	// Add days remaining layer
	label_layer_countdown = text_layer_create(GRect(0, 56, 144, 55));
	text_layer_set_text_color(label_layer_countdown, GColorBlack);
	text_layer_set_background_color(label_layer_countdown, GColorClear);
	text_layer_set_text_alignment(label_layer_countdown, GTextAlignmentCenter);
	text_layer_set_font(label_layer_countdown, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_countdown));
	
	// Add top text layer
	label_layer_text_top = text_layer_create(GRect(0, 110, 144, 23));
	text_layer_set_text_color(label_layer_text_top, GColorBlack);
	text_layer_set_background_color(label_layer_text_top, GColorClear);
	text_layer_set_text_alignment(label_layer_text_top, GTextAlignmentCenter);
	text_layer_set_text(label_layer_text_top, "Days Until");
	text_layer_set_font(label_layer_text_top, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_text_top));
	
	// Add bottom text layer
	label_layer_text_bottom = text_layer_create(GRect(0, 130, 144, 23));
	text_layer_set_text_color(label_layer_text_bottom, GColorBlack);
	text_layer_set_background_color(label_layer_text_bottom, GColorClear);
	text_layer_set_text_alignment(label_layer_text_bottom, GTextAlignmentCenter);
	text_layer_set_font(label_layer_text_bottom, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_text_bottom));
	
	// Create the inverter layer
	inverter_layer = inverter_layer_create(bounds);
	layer_add_child(window_layer, inverter_layer_get_layer(inverter_layer));
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	
	set_theme();
	set_date();
	set_label();
}

static void deinit() {
	window_destroy(window);
	text_layer_destroy(label_layer_time);
	text_layer_destroy(label_layer_countdown);
	text_layer_destroy(label_layer_text_top);
	text_layer_destroy(label_layer_text_bottom);
	inverter_layer_destroy(inverter_layer);
	
	tick_timer_service_unsubscribe();
	app_message_deregister_callbacks();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}