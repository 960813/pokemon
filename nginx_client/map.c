#include "map.h"
#include "stdio.h"

//ĳ���ʹ� ��ũ�� �߾ӿ� ����.
//���� ĳ������ �����ӿ� ���� scroll transform


void init_terrain(ALLEGRO_BITMAP* bitmap) {
	
	al_set_target_bitmap(world_map);

	int screenWidth  = al_get_display_width(al_get_current_display());
	int screenHeight = al_get_display_height(al_get_current_display());

	al_draw_tinted_scaled_rotated_bitmap_region(bitmap, 5,5, 176, 144, al_map_rgb(255, 255, 255), 0, 0, /*screenWidth/2-88*GAME_SCALE*/500, /*screenHeight / 2 - 72 * GAME_SCALE*/500, GAME_SCALE, GAME_SCALE, 0, 0);

	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
}