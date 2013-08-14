/**
 * This code is provided under the MIT License.
 * 
 * Copyright (c) 2013 Matthew Congrove (http://github.com/mcongrove)
 * 
 * Utilizes portions of code (PDUtils) by Peter Hardy (http://github.com/phardy)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "PDutils.h"

#define APP_UUID { 0x35, 0xBA, 0x1E, 0x8E, 0x25, 0x65, 0x4C, 0x78, 0x95, 0xF6, 0xF9, 0x33, 0x53, 0x30, 0x8B, 0x04 }
#define APP_TYPE APP_INFO_STANDARD_APP
#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 5
#define LABEL_ROW "Days Remaining"

PBL_APP_INFO(APP_UUID, "Countdown", "Matthew Congrove", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_TYPE);

Window window_root;
Window window_menu;
TextLayer label_time;
TextLayer label_countdown;
TextLayer label_row;
SimpleMenuLayer menu_layer;
SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
SimpleMenuItem menu_items[NUM_MENU_ITEMS];

char timeText[] = "00:00";
char countText[4];
char settingTextMonth[10];
char settingTextDay[10];
char settingTextYear[10];
char settingTextHour[10];
char settingTextMinute[10];

static int EVENT_MONTH = 1;
static int EVENT_DAY = 1;
static int EVENT_YEAR = 2013;
static int EVENT_HOUR = 12;
static int EVENT_MINUTE = 0;
static int EVENT_SECOND = 0;

bool calculate_countdown() {
	PblTm now;
	PblTm event;
	time_t seconds_now;
	time_t seconds_event;
	int difference;
	char *time_format;
	static char countText[] = "";
	
	get_time(&now);
	
	seconds_now = pmktime(&now);
	
	event.tm_year = EVENT_YEAR - 1900;
	event.tm_mon = EVENT_MONTH - 1;
	event.tm_mday = EVENT_DAY;
	event.tm_hour = EVENT_HOUR;
	event.tm_min = EVENT_MINUTE;
	event.tm_sec = EVENT_SECOND;
	
	seconds_event = pmktime(&event);
	
	difference = ((((seconds_event - seconds_now) / 60) / 60) / 24);
	
	snprintf(countText, 100, "%d", difference);
	
	text_layer_set_text(&label_countdown, countText);
	
	if(clock_is_24h_style()) {
		time_format = "%R";
	} else {
		time_format = "%I:%M";
	}
	
	string_format_time(timeText, sizeof(timeText), time_format, &now);
	
	text_layer_set_text(&label_time, timeText);
	
	return true;
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)ctx;
	(void)t;
	
	calculate_countdown();
}

char* menu_set_month(int value, int index) {
	if(EVENT_MONTH > 12) { EVENT_MONTH = 1; }
	
	snprintf(settingTextMonth, 100, "%d", EVENT_MONTH);
	
	menu_items[index].subtitle = settingTextMonth;
	layer_mark_dirty(simple_menu_layer_get_layer(&menu_layer));
	
	return settingTextMonth;
}

char* menu_set_day(int value, int index) {
	if(EVENT_DAY > 31) { EVENT_DAY = 1; }
	
	snprintf(settingTextDay, 100, "%d", EVENT_DAY);
	
	menu_items[index].subtitle = settingTextDay;
	layer_mark_dirty(simple_menu_layer_get_layer(&menu_layer));
	
	return settingTextDay;
}

char* menu_set_year(int value, int index) {
	if(EVENT_YEAR > 2020) { EVENT_YEAR = 2013; }
	
	snprintf(settingTextYear, 100, "%d", EVENT_YEAR);
	
	menu_items[index].subtitle = settingTextYear;
	layer_mark_dirty(simple_menu_layer_get_layer(&menu_layer));
	
	return settingTextYear;
}

char* menu_set_hour(int value, int index) {
	if(EVENT_HOUR > 23) { EVENT_HOUR = 0; }
	
	snprintf(settingTextHour, 100, "%d", EVENT_HOUR);
	
	menu_items[index].subtitle = settingTextHour;
	layer_mark_dirty(simple_menu_layer_get_layer(&menu_layer));
	
	return settingTextHour;
}

char* menu_set_minute(int value, int index) {
	if(EVENT_MINUTE > 59) { EVENT_MINUTE = 0; }
	
	snprintf(settingTextMinute, 100, "%d", EVENT_MINUTE);
	
	menu_items[index].subtitle = settingTextMinute;
	layer_mark_dirty(simple_menu_layer_get_layer(&menu_layer));
	
	return settingTextMinute;
}

void menu_select_callback(int index, void *ctx) {
	switch(index) {
		case 0:
			menu_set_month(EVENT_MONTH++, index);
			break;
		case 1:
			menu_set_day(EVENT_DAY++, index);
			break;
		case 2:
			menu_set_year(EVENT_YEAR++, index);
			break;
		case 3:
			menu_set_hour(EVENT_HOUR++, index);
			break;
		case 4:
			menu_set_minute(EVENT_MINUTE++, index);
			break;
	}
	
	calculate_countdown();
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
	window_set_background_color(&window_menu, GColorWhite);
	window_stack_push(&window_menu, true);
	window_set_click_config_provider(&window_menu, (ClickConfigProvider) menu_window_click_provider);
	
	menu_items[0] = (SimpleMenuItem){
		.title = "Month",
		.subtitle = menu_set_month(EVENT_MONTH, 0),
		.callback = menu_select_callback
	};
	
	menu_items[1] = (SimpleMenuItem){
		.title = "Day",
		.subtitle = menu_set_day(EVENT_DAY, 1),
		.callback = menu_select_callback
	};
	
	menu_items[2] = (SimpleMenuItem){
		.title = "Year",
		.subtitle = menu_set_year(EVENT_YEAR, 2),
		.callback = menu_select_callback
	};
	
	menu_items[3] = (SimpleMenuItem){
		.title = "Hour",
		.subtitle = menu_set_hour(EVENT_HOUR, 3),
		.callback = menu_select_callback
	};
	
	menu_items[4] = (SimpleMenuItem){
		.title = "Minute",
		.subtitle = menu_set_minute(EVENT_MINUTE, 4),
		.callback = menu_select_callback
	};
	
	menu_sections[0] = (SimpleMenuSection){
		.title = "Event Date Settings",
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
	
	text_layer_init(&label_row, window_root.layer.frame);
	text_layer_set_text_color(&label_row, GColorWhite);
	text_layer_set_background_color(&label_row, GColorClear);
	text_layer_set_text_alignment(&label_row, GTextAlignmentCenter);
	layer_set_frame(&label_row.layer, GRect(0, 110, 144, 25));
	text_layer_set_text(&label_row, LABEL_ROW);
	text_layer_set_font(&label_row, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(&window_root.layer, &label_row.layer);
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