#include "map.h"
//ĳ���ʹ� ��ũ�� �߾ӿ� ����.
//���� ĳ������ �����ӿ� ���� scroll transform
void init_map(ALLEGRO_BITMAP* bitmap) {
	memset(map_tile, 0, sizeof(map_tile));

	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 38; j++) {
			switch (map_tile[0][i][j]) {
			case 0:
				al_draw_bitmap_region(bitmap, 0, 0, 16, 16, 16 * i, 16 * j, 0);
				break;
			}
		}
	}
}

void create_object(ALLEGRO_BITMAP * bitmap){
}