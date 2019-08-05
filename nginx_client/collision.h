#include "nginx_common.h"
#include "player.h"

//    sx,   sy, width, height
// X��ǥ, Y��ǥ, �ʺ�, ����
// ���� ���п���
// ��-�� : ��(8px) ��(8px)
// ��-�� : ��() ��()
static int colBoxArray[][4] ={
	{0,0,176,32},
	{0,0,1,144},
	{175,0,1,144},
	{0,142,176,2},
	{79,62,18,34}

};

typedef struct _COLLISION_BOX {
	int sx;
	int sy;
	int	width;
	int height;
}collisionBox;
static collisionBox* colBoxes = NULL;

bool isCollision(player_status _player);
void createCollision(collisionBox* colBox, int iPosX,int iPosY, int iWidth, int iHeight);
void initCollision();