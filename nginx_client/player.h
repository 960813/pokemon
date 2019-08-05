#ifndef _PLAYER_HEADER_
#define _PLAYER_HEADER_
#include "nginx_common.h"

#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 20



typedef struct _PLAYER_STATUS {
	ALLEGRO_BITMAP* _player;
	ALLEGRO_BITMAP* _hit_efftect;
	char cName[12];

	int iAction_type;
	int iPlayer_direction;
	int iAction_idx;
	bool bMoveFlag;

	int iPos_x;
	int iPos_y;

	int iHp;
	int iArmor;

	int iBuf;
} player_status;


static int character_movement[4][3][4] = {
	{{24,36,16,20},{8,36,16,20},{40,36,16,20}}, // ��
	{{24,100,16,20},{8,100,16,20},{40,100,16,20}}, // ��
	{{24,68,16,20},{8,68,16,20},{40,68,16,20}}, // ��
	{{24,100,16,20},{8,100,16,20},{40,100,16,20} }  // ��
};
static int character_attack[4][4][4] = {
	{{0,0,1,2},{1,0,1,2},{2,0,1,2},{3,0,1,2}},
	{{0,2,1,2},{1,2,1,2},{2,2,1,2},{3,2,1,2}},
	{{0,1,1,2},{1,1,1,2},{2,1,1,2},{3,1,1,2}},
	{{0,3,1,2},{1,3,1,2},{2,3,1,2},{3,3,1,2}}
};

static int character_hit_effect[10][4] = {
	{0,0,1,1},{1,0,1,1},{2,0,1,1},{3,0,1,1},{4,0,1,1},
	{0,1,1,1},{1,1,1,1},{2,1,1,1},{3,1,1,1},{4,1,1,1}
};

void sendPlayerStatus(const char* header, const player_status uPlayer);
void movement_character(ALLEGRO_BITMAP* bitmap, int dx, int dy, int action_type, int action_idx);
void attack_character(ALLEGRO_BITMAP* bitmap, int dx, int dy, int action_type, int action_idx);
void show_hit_effect(ALLEGRO_BITMAP* bitmap, int dx, int dy, int action_idx);

#endif