﻿#include "battle.h"
#include "book.h"
#include <math.h>

extern battleUIStatus battleUI_status;
extern float camera_position_x;
extern float camera_position_y;

extern ALLEGRO_FONT* get_convsPirnt_font();
extern ALLEGRO_FONT* get_pokemonmenu_hp_Print_font();

extern pokemon myPokemonList[6];
extern pokemon pokemonBook[15];
extern pokemon enemy;
void drawBasicUI() {
	al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 0, 107, 240, 112, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y, 3.3333333, GAME_SCALE, 0, 0);

	char tmp_crt_hp[4], tmp_max_hp[4], tmp_level[4];
	// enemy
	al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 246, 107, 100, 29, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 6 * GAME_SCALE, camera_position_y + 8 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
	al_draw_bitmap(enemy.front, camera_position_x + 115 * GAME_SCALE, camera_position_y + 15 * GAME_SCALE, 0);
	sprintf_s(tmp_crt_hp, 4, "%d", enemy.crt_hp);
	sprintf_s(tmp_max_hp, 4, "%d", enemy.max_hp);
	sprintf_s(tmp_level, 4, "%d", enemy.level);

	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 11 * GAME_SCALE, camera_position_y + 14.5 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, enemy.displayName);
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 65 * GAME_SCALE, camera_position_y + 14.5 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, tmp_level);

	int pokemon_hp_pos_x = camera_position_x + 38.5 * GAME_SCALE;
	int pokemon_hp_pos_y = camera_position_y + 25 * GAME_SCALE;
	double hp_matrix = enemy.crt_hp / (double)enemy.max_hp * 100;
	for (int i = 0; i < hp_matrix; i++) {
		if (hp_matrix <= 25)
			al_draw_tinted_scaled_rotated_bitmap_region(pokemonMenuBitmap, 360, 235, 1, 3, al_map_rgb(255, 255, 255), 0, 0, pokemon_hp_pos_x + 0.3945 * i * GAME_SCALE, pokemon_hp_pos_y, 3.33333333, GAME_SCALE, 0, 0);
		else if (hp_matrix <= 50)
			al_draw_tinted_scaled_rotated_bitmap_region(pokemonMenuBitmap, 366, 235, 1, 3, al_map_rgb(255, 255, 255), 0, 0, pokemon_hp_pos_x + 0.3945 * i * GAME_SCALE, pokemon_hp_pos_y, 3.33333333, GAME_SCALE, 0, 0);
		else
			al_draw_tinted_scaled_rotated_bitmap_region(pokemonMenuBitmap, 384, 235, 1, 3, al_map_rgb(255, 255, 255), 0, 0, pokemon_hp_pos_x + 0.3945 * i * GAME_SCALE, pokemon_hp_pos_y, 3.33333333, GAME_SCALE, 0, 0);
	}


	// my pokemon
	al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 246, 148, 104, 37, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 105 * GAME_SCALE, camera_position_y + 74 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
	al_draw_bitmap(myPokemonList[0].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 58 * GAME_SCALE, 0);

	sprintf_s(tmp_crt_hp, 4, "%d", myPokemonList[battleUI_status.currentPokemonIdx].crt_hp);
	sprintf_s(tmp_max_hp, 4, "%d", myPokemonList[battleUI_status.currentPokemonIdx].max_hp);
	sprintf_s(tmp_level, 4, "%d", myPokemonList[battleUI_status.currentPokemonIdx].level);

	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 117 * GAME_SCALE, camera_position_y + 80.5 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, myPokemonList[battleUI_status.currentPokemonIdx].displayName);

	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 153.5 * GAME_SCALE, camera_position_y + 96 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, tmp_crt_hp);
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 171.5 * GAME_SCALE, camera_position_y + 96 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, tmp_max_hp);
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 171 * GAME_SCALE, camera_position_y + 80.5 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, tmp_level);

	pokemon_hp_pos_x = camera_position_x + 145 * GAME_SCALE;
	pokemon_hp_pos_y = camera_position_y + 91 * GAME_SCALE;
	hp_matrix = myPokemonList[battleUI_status.currentPokemonIdx].crt_hp / (double)myPokemonList[battleUI_status.currentPokemonIdx].max_hp * 100;
	for (int i = 0; i < hp_matrix; i++) {
		if (hp_matrix <= 25)
			al_draw_tinted_scaled_rotated_bitmap_region(pokemonMenuBitmap, 360, 235, 1, 3, al_map_rgb(255, 255, 255), 0, 0, pokemon_hp_pos_x + 0.3945 * i * GAME_SCALE, pokemon_hp_pos_y, 3.33333333, GAME_SCALE, 0, 0);
		else if (hp_matrix <= 50)
			al_draw_tinted_scaled_rotated_bitmap_region(pokemonMenuBitmap, 366, 235, 1, 3, al_map_rgb(255, 255, 255), 0, 0, pokemon_hp_pos_x + 0.3945 * i * GAME_SCALE, pokemon_hp_pos_y, 3.33333333, GAME_SCALE, 0, 0);
		else
			al_draw_tinted_scaled_rotated_bitmap_region(pokemonMenuBitmap, 384, 235, 1, 3, al_map_rgb(255, 255, 255), 0, 0, pokemon_hp_pos_x + 0.3945 * i * GAME_SCALE, pokemon_hp_pos_y, 3.33333333, GAME_SCALE, 0, 0);
	}

	int pokemon_exp_pos_x = camera_position_x + 131.5 * GAME_SCALE;
	int pokemon_exp_pos_y = camera_position_y + 108 * GAME_SCALE;
	int pokemon_prev_level_exp = pow((double)myPokemonList[battleUI_status.currentPokemonIdx].level, 3.0);
	int pokemon_crt_level_exp = myPokemonList[battleUI_status.currentPokemonIdx].exp;
	int pokemon_next_level_exp = pow((double)myPokemonList[battleUI_status.currentPokemonIdx].level + 1, 3.0);
	double exp_matrix = (pokemon_crt_level_exp - (double)pokemon_prev_level_exp) / ((double)pokemon_next_level_exp - pokemon_prev_level_exp) * 214;
	al_draw_line(pokemon_exp_pos_x, pokemon_exp_pos_y, pokemon_exp_pos_x + exp_matrix, pokemon_exp_pos_y, al_map_rgb(64, 200, 248), 8);
}

int calculateEXP(int level) {
	return 40 * level / 7;
}

extern bool is_key_pressed(int keycode);
void fightHandler() {
	pokemon user = myPokemonList[battleUI_status.currentPokemonIdx];
	showSkillList();
	if (is_key_pressed(ALLEGRO_KEY_UP) || is_key_pressed(ALLEGRO_KEY_LEFT)) {
		if (battleUI_status.currentIndex > 0)
			battleUI_status.currentIndex--;
		else
			battleUI_status.currentIndex = 3;
	}
	if (is_key_pressed(ALLEGRO_KEY_DOWN) || is_key_pressed(ALLEGRO_KEY_RIGHT)) {
		if (battleUI_status.currentIndex < 3)
			battleUI_status.currentIndex++;
		else
			battleUI_status.currentIndex = 0;
	}
}

void showBattleUI() {
	if (!battleUI_status.battleUIOpen)
		return;

	int convsX = camera_position_x + 12 * GAME_SCALE;
	int convsY = camera_position_y + (GAME_HEIGHT - 48 * GAME_SCALE) + 11 * GAME_SCALE;

	int arrowPosition_x = camera_position_x + 107 * GAME_SCALE;
	int arrowPosition_y = camera_position_y + 124 * GAME_SCALE;

	int arrowOffset_x = 0;
	int arrowOffset_y = 0;


	drawBasicUI();
	switch (battleUI_status.currentMenu) {
	case 0:
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 0, 0, 120, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 400, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);

		switch (battleUI_status.currentIndex) {
		case 1:
			arrowOffset_x = 46.5 * GAME_SCALE;
			break;
		case 2:
			arrowOffset_y = 17 * GAME_SCALE;
			break;
		case 3:
			arrowOffset_x = 46.5 * GAME_SCALE;
			arrowOffset_y = 17 * GAME_SCALE;
			break;
		}
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 123, 0, 6, 10, al_map_rgb(255, 255, 255), 0, 0, arrowPosition_x + arrowOffset_x, arrowPosition_y + arrowOffset_y, 3.3333333, GAME_SCALE, 0, 0);
		break;
	case 1:
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 0, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		fightHandler();
		break;
	case 2:
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "BAG!");
		break;
	case 3:
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "POKEMON!");
		break;
	case 4:
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "RUN!");
		break;
	}
}