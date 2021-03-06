﻿#include "portal.h"
// 사용자의 현재 위치가 Portal인지 확인.
// 마찬가지로 Bounding Box Algorithm
int isPortal(player _player)
{
	const int portalBoxLength = sizeof(mapPortal[GAME_STAGE]) / sizeof(mapPortal[GAME_STAGE][0][0]) / sizeof(int);
	//printf("%d\n", portalBoxLength);
	int _status = -4;
	int i = 0;
	for (i = 0; i < portalBoxLength; i++) {
		int px = _player.iPos_x;
		int py = _player.iPos_y;
		int pw = PLAYER_WIDTH * GAME_SCALE;
		int ph = PLAYER_HEIGHT * GAME_SCALE;

		int ox = 500 + mapPortal[GAME_STAGE][i][0] * GAME_SCALE;
		int oy = 500 + mapPortal[GAME_STAGE][i][1] * GAME_SCALE;
		int ow = mapPortal[GAME_STAGE][i][2] * GAME_SCALE;
		int oh = mapPortal[GAME_STAGE][i][3] * GAME_SCALE;

		if (GAME_MODE == GAME_ENV_DEVELOPMENT)
			al_draw_rectangle(ox, oy, ox + ow, oy + oh, al_map_rgb(0, 0, 255), 3);

		//printf("%d,%d,%d,%d\n%d,%d,%d,%d\n", px, py, pw, ph, ox, oy, ow, oh);

		if ((px + PLAYER_WIDTH / 2 > ox + ow) ||
			(py + PLAYER_HEIGHT > oy + oh) ||
			(ox > px - PLAYER_WIDTH / 2 + pw) ||
			(oy > py - PLAYER_HEIGHT / 2 + ph))
			_status = -4;
		else
			return mapPortal[GAME_STAGE][i][4];
	}
	return _status;
}