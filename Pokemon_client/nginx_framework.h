﻿#ifndef _POKEMON_FRAMEWORK_HEADER_
#define _POKEMON_FRAMEWORK_HEADER_
#include "nginx_common.h"

//==============================================================================
// UTIL
//==============================================================================

#define lengthof(x)  (sizeof(x) / sizeof(x[0]))

//==============================================================================
// DEBUG
//==============================================================================

/*
	Logging macros.
	- log_message: for debug messages
	- log_warning: for non-fatal errors, i.e. continue program execution
	- log_error:   for fatal errors, program will be forcefully terminated
 */
#define log_message(...) write_logfile(LOG_MESSAGE, __VA_ARGS__);
#define log_warning(...) write_logfile(LOG_WARNING, __VA_ARGS__);
#define log_error(...)   write_logfile(LOG_ERROR, __VA_ARGS__);

/// <summary>
/// logging levels (used in write_logfile)
/// </summary>
enum { LOG_MESSAGE, LOG_WARNING, LOG_ERROR };

/*
	Writes a message to the logfile.
	Use the logging macros above instead of using this function directly.
 */
void write_logfile(int log_level, const char* format, ...);

//==============================================================================
// FRAMEWORK
//==============================================================================

/// <summary>
/// </summary>
/// <param name="title"></param>
/// <param name="window_width"></param>
/// <param name="window_height"></param>
/// <param name="fullscreen"></param>
/*
	Initializes the framework.
	This must be called before you use anything else in this framework!

	title: the window title
	window_width: the width of the window
	window_height: the height of the window
	fullscreen: whether to start in fullscreen or not
*/
void init_framework(const char* title, int window_width, int window_height, bool fullscreen);

/// <summary>
/// </summary>
/// <param name=""></param>
/*
	Destroys everything we need to clean up when it is time to quit the program.
	This function is called automatically at program exit.
*/
void destroy_framework(void);

/// <summary>
/// </summary>
/// <param name="update_proc"></param>
/// <param name="render_proc"></param>
/*
	Runs the game loop; the heart of the game!

	update_proc() and draw_proc() are function pointers you need to define yourself.
	Will call update_proc() 60 times per second.
	Will call render_proc() 60 times a second if there is no other events to deal with.
	If there is nothing else to do, the game loop will sleep.
*/
void run_game_loop(void (*update_proc)(), void (*render_proc)());

// Stops the game loop.
void quit();

/*
	Sets alt-tab behavior (switching out & in from an application).
	If enabled, the game logic and rendering will pause until the game
	becomes active again. This is enabled by default.
 */
void alt_tab_should_pause(bool true_or_false);

//==============================================================================
// GRAPHICS
//==============================================================================

// Returns the width of the window.
int get_window_width();

// Returns the height of the window.
int get_window_height();

// Returns a default font. Mainly used for debugging purposes.
ALLEGRO_FONT* get_default_font();
ALLEGRO_FONT* get_chatInput_font();
ALLEGRO_FONT* get_chatText_font();
ALLEGRO_FONT* get_menuPirnt_font();
ALLEGRO_FONT* get_pokemonmenuPirnt_font();
ALLEGRO_FONT* get_pokemonmenu_level_Print_font();
ALLEGRO_FONT* get_pokemonmenu_hp_Print_font();
ALLEGRO_FONT* get_convsPirnt_font();


//==============================================================================
// INPUT
//==============================================================================

// Returns true if a key on the keyboard is held down.
bool is_key_down(int keycode);

// Returns true if a key on the keyboard was pressed.
bool is_key_pressed(int keycode);

// Returns true if a key on the keyboard was released.
bool is_key_released(int keycode);

// Clear Keyboard Buffed
void clear_key_buffered();

// helper mouse input enum
enum {
	MOUSE_LEFT_BUTTON,
	MOUSE_RIGHT_BUTTON,
	MOUSE_MIDDLE_BUTTON,
	MAX_MOUSE_BUTTONS
};

// Returns mouse x coordinate.
int get_mouse_x();

// Returns mouse y coordinate.
int get_mouse_y();

// Returns mouse delta movement in x since last frame.
int get_mouse_dx();

// Returns mouse delta movement in y since last frame.
int get_mouse_dy();

// Returns true if a mouse button is held down.
bool is_mouse_button_down(int mouse_button);

// Returns true if a mouse button was pressed.
bool is_mouse_button_pressed(int mouse_button);

// Returns true if a mouse button was released.
bool is_mouse_button_released(int mouse_button);

/*
	Waits until a key is pressed on the keyboard.
	Returns the keycode of the key that was pressed.
 */
int wait_for_keypress();

#endif