﻿#include "nginx_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "player.h"
#include "menu.h"

static ALLEGRO_EVENT_QUEUE* event_queue = NULL;
static ALLEGRO_DISPLAY* display = NULL;
static ALLEGRO_TIMER* timer = NULL;
static ALLEGRO_FILE* logfile = NULL;
static ALLEGRO_FONT* default_font = NULL;
static ALLEGRO_FONT* chatInput_font = NULL;
static ALLEGRO_FONT* chatText_font = NULL;

static ALLEGRO_FONT* menuPrint_font = NULL;
static ALLEGRO_FONT* pokemonMenuPrint_font = NULL;
static ALLEGRO_FONT* convsPrint_font = NULL;
static ALLEGRO_FONT* pokemonMenu_level_Print_font = NULL;
static ALLEGRO_FONT* pokemonMenu_hp_Print_font = NULL;

static ALLEGRO_FONT* pokemonSkill_list_font = NULL;


static bool is_done = false;
static bool is_paused = false;
static bool should_alt_tab_pause = true;

static bool keys[ALLEGRO_KEY_MAX] = { false };
static bool keys_pressed[ALLEGRO_KEY_MAX] = { false };
static bool keys_released[ALLEGRO_KEY_MAX] = { false };

static int mouse_x = 0, mouse_y = 0;
static int mouse_old_x = 0, mouse_old_y = 0;
static bool mouse_buttons[MAX_MOUSE_BUTTONS] = { false };
static bool mouse_buttons_pressed[MAX_MOUSE_BUTTONS] = { false };
static bool mouse_buttons_released[MAX_MOUSE_BUTTONS] = { false };

extern ALLEGRO_USTR* transferUserNickInput;

extern player user_player;
extern menuStatus menu_status;

void write_logfile(int log_level, const char* format, ...) {
	char buffer[4096];

	if (!logfile) {
		logfile = al_fopen("log.txt", "w");
	}

	if (log_level == LOG_WARNING) {
		al_fputs(logfile, "WARNING: ");
	}
	else if (log_level == LOG_ERROR) {
		al_fputs(logfile, "ERROR: ");
	}

	// va_list에 기반하여 데이터 작성 후 fputs
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	al_fputs(logfile, buffer);
	al_fputs(logfile, "\n");

	if (log_level == LOG_ERROR) {
		exit(1);
	}
}

void init_framework(const char* title, int window_width, int window_height, bool fullscreen) {
	if (!al_init()) {
		log_error("Failed to initialize allegro");
	}
	//Zero-Warning를 위해 destory_framework를 (void)형식으로 선언.
	atexit(destroy_framework);

	al_set_exe_name(title);
	al_set_app_name(title);

	// set correct resource directory
	ALLEGRO_PATH* path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_change_directory(al_path_cstr(path, '/'));
	al_destroy_path(path);

	// 키보드 초기화
	if (!al_install_keyboard()) {
		log_error("Failed to install keyboard");
	}

	// 마우스 초기화
	if (!al_install_mouse()) {
		log_error("Failed to install mouse");
	}

	// primitives addon 초기화
	if (!al_init_primitives_addon()) {
		log_error("Failed to init primitives addon");
	}

	// image addon 초기화
	if (!al_init_image_addon()) {
		log_error("Failed to init image addon");
	}

	// audio install
	if (!al_install_audio()) {
		log_error("Failed to init install audio");
	}

	// acodec addon 초기화
	if (!al_init_acodec_addon()) {
		log_error("Failed to init init acodec");
	}

	// font 및 ttf addon 초기화
	al_init_font_addon();
	al_init_ttf_addon();

	/*
		각종 font를 사전의 초기화
	*/
	default_font = al_load_ttf_font("fonts/NanumGothic.ttf", 50, ALLEGRO_TTF_NO_KERNING);
	if (!default_font) {
		log_error("Failed to load ttf font");
	}

	chatInput_font = al_load_ttf_font("fonts/Roboto-Medium.ttf", 20, ALLEGRO_TTF_NO_KERNING);
	if (!chatInput_font) {
		log_error("Failed to load ttf font");
	}

	chatText_font = al_load_ttf_font("fonts/Roboto-Medium.ttf", 20, ALLEGRO_TTF_NO_KERNING);
	if (!chatText_font) {
		log_error("Failed to load ttf font");
	}

	menuPrint_font = al_load_ttf_font("fonts/pkm.ttf", 27, ALLEGRO_TTF_NO_KERNING);
	if (!menuPrint_font) {
		log_error("Failed to load ttf font");
	}

	pokemonMenuPrint_font = al_load_ttf_font("fonts/pkm.ttf", 21, ALLEGRO_TTF_NO_KERNING);
	if (!pokemonMenuPrint_font) {
		log_error("Failed to load ttf font");
	}

	pokemonMenu_level_Print_font = al_load_ttf_font("fonts/pkm.ttf", 35, ALLEGRO_TTF_NO_KERNING);
	if (!pokemonMenu_level_Print_font) {
		log_error("Failed to load ttf font");
	}

	pokemonMenu_hp_Print_font = al_load_ttf_font("fonts/pkm.ttf", 21, ALLEGRO_TTF_NO_KERNING);
	if (!pokemonMenu_hp_Print_font) {
		log_error("Failed to load ttf font");
	}

	pokemonSkill_list_font = al_load_ttf_font("fonts/pkm.ttf", 23, ALLEGRO_TTF_NO_KERNING);
	if (!pokemonSkill_list_font) {
		log_error("Failed to load ttf font");
	}

	convsPrint_font = al_load_ttf_font("fonts/pkm.ttf", 35, ALLEGRO_TTF_NO_KERNING);
	if (!convsPrint_font) {
		log_error("Failed to load ttf font");
	}

	event_queue = al_create_event_queue();
	if (!event_queue) {
		log_error("Failed to create event queue");
	}

	if (fullscreen) {
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	}
	else {
		al_set_new_display_flags(ALLEGRO_WINDOWED);
	}

	display = al_create_display(window_width, window_height);
	if (!display) {
		log_error("Failed to create display @ %dx%d", window_width, window_height);
	}
	al_set_window_title(display, title);

	timer = al_create_timer(1.0 / GAME_FPS);
	if (!timer) {
		log_error("Failed to create timer");
	}

	transferUserNickInput = al_ustr_new("");


	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	srand(time(NULL));
}

void destroy_framework(void) {
	if (default_font) {
		al_destroy_font(default_font);
		default_font = NULL;
	}

	if (timer) {
		al_destroy_timer(timer);
		timer = NULL;
	}

	if (display) {
		al_destroy_display(display);
		display = NULL;
	}

	if (event_queue) {
		al_destroy_event_queue(event_queue);
		event_queue = NULL;
	}

	if (logfile) {
		al_fclose(logfile);
		logfile = NULL;
	}
}

void run_game_loop(void (*update_proc)(), void (*render_proc)()) {
	bool should_redraw = true;
	al_start_timer(timer);

	while (!is_done) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		switch (event.type) {
		case ALLEGRO_EVENT_TIMER:
			should_redraw = true;
			if (!is_paused) {
				update_proc();
			}

			// clear input state
			memset(keys_pressed, false, sizeof(keys_pressed));
			memset(keys_released, false, sizeof(keys_pressed));
			memset(mouse_buttons_pressed, false, sizeof(mouse_buttons_pressed));
			memset(mouse_buttons_released, false, sizeof(mouse_buttons_released));
			mouse_old_x = mouse_x;
			mouse_old_y = mouse_y;
			break;

		case ALLEGRO_EVENT_KEY_DOWN:
			keys[event.keyboard.keycode] = true;
			keys_pressed[event.keyboard.keycode] = true;
			break;

		case ALLEGRO_EVENT_KEY_UP:
			keys[event.keyboard.keycode] = false;
			keys_released[event.keyboard.keycode] = true;
			break;

		case ALLEGRO_EVENT_KEY_CHAR:
			// handle alt-tab
			//if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_ALT) &&
			//	event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
			//	al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW));
			//}
			if (menu_status.currentMenu == COMPUTER_SYSTEM_MENU_TRANSFER_NICK_INPUT) {
				int unichar = event.keyboard.unichar;
				if (unichar == 8 && transferUserNickInput->slen >= 1)
					al_ustr_remove_chr(transferUserNickInput, (int)al_ustr_length(transferUserNickInput) - 1);
				if ((('a' <= unichar && unichar <= 'z') || ('A' <= unichar && unichar <= 'Z')) && transferUserNickInput->slen < 12) {
					printf("%d\n", unichar);
					al_ustr_append_chr(transferUserNickInput, unichar);
				}
			}
			break;

		case ALLEGRO_EVENT_MOUSE_AXES:
			mouse_x = event.mouse.x;
			mouse_y = event.mouse.y;
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			mouse_buttons[event.mouse.button] = true;
			mouse_buttons_pressed[event.mouse.button] = true;
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			mouse_buttons[event.mouse.button] = false;
			mouse_buttons_released[event.mouse.button] = true;
			break;

		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			is_done = true;
			break;

		case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
			if (should_alt_tab_pause) {
				is_paused = true;
			}
			break;

		case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
			if (should_alt_tab_pause) {
				is_paused = false;
			}
			break;
		}
		//printf("%d, %d, %d\n", should_redraw, al_is_event_queue_empty(event_queue), is_paused);
		is_paused = false;
		if (should_redraw && al_is_event_queue_empty(event_queue) && !is_paused) {
			should_redraw = false;
			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
			render_proc();
		}
	}
}


void quit() {
	is_done = true;
}

void alt_tab_should_pause(bool true_or_false) {
	should_alt_tab_pause = true_or_false;
}

int get_window_width() {
	assert(display != NULL);
	return al_get_display_width(display);
}

int get_window_height() {
	assert(display != NULL);
	return al_get_display_height(display);
}

bool is_key_down(int keycode) {
	assert(keycode >= 0 && keycode < ALLEGRO_KEY_MAX);
	return keys[keycode];
}

bool is_key_pressed(int keycode) {
	assert(keycode >= 0 && keycode < ALLEGRO_KEY_MAX);
	return keys_pressed[keycode];
}

bool is_key_released(int keycode) {
	assert(keycode >= 0 && keycode < ALLEGRO_KEY_MAX);
	return keys_released[keycode];
}

void clear_key_buffered() {
	memset(keys, false, sizeof(keys));
	memset(keys_pressed, false, sizeof(keys_pressed));
	memset(keys_released, false, sizeof(keys_pressed));
}

int get_mouse_x() {
	return mouse_x;
}

int get_mouse_y() {
	return mouse_y;
}

int get_mouse_dx() {
	return mouse_x - mouse_old_x;
}

int get_mouse_dy() {
	return mouse_y - mouse_old_y;
}

bool is_mouse_button_down(int mouse_button) {
	assert(mouse_button >= 0 && mouse_button < (int)al_get_mouse_num_buttons());
	return mouse_buttons[mouse_button];
}

bool is_mouse_button_pressed(int mouse_button) {
	assert(mouse_button >= 0 && mouse_button < (int)al_get_mouse_num_buttons());
	return mouse_buttons_pressed[mouse_button];
}

bool is_mouse_button_released(int mouse_button) {
	assert(mouse_button >= 0 && mouse_button < (int)al_get_mouse_num_buttons());
	return mouse_buttons_released[mouse_button];
}

int wait_for_keypress() {
	ALLEGRO_EVENT event;
	do {
		al_wait_for_event(event_queue, &event);
	} while (event.type != ALLEGRO_EVENT_KEY_DOWN);
	return event.keyboard.keycode;
}

ALLEGRO_FONT* get_default_font() {
	return default_font;
}
ALLEGRO_FONT* get_chatInput_font() {
	return chatInput_font;
}
ALLEGRO_FONT* get_chatText_font() {
	return chatText_font;
}
ALLEGRO_FONT* get_menuPirnt_font() {
	return menuPrint_font;
}
ALLEGRO_FONT* get_pokemonmenuPirnt_font() {
	return pokemonMenuPrint_font;
}
ALLEGRO_FONT* get_pokemonmenu_level_Print_font() {
	return pokemonMenu_level_Print_font;
}
ALLEGRO_FONT* get_pokemonmenu_hp_Print_font() {
	return pokemonMenu_hp_Print_font;
}
ALLEGRO_FONT* get_convsPirnt_font() {
	return convsPrint_font;
}

ALLEGRO_FONT* get_pokemonSkill_list_font() {
	return pokemonSkill_list_font;
}