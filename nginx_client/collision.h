#include "nginx_common.h"
#include "player.h"

//    sx,   sy, width, height
// X��ǥ, Y��ǥ, �ʺ�, ����
// ���� ���п���
// ��-�� : ��(8px) ��(8px)
// ��-�� : ��() ��()
static int colBoxArray[][4] =
{
	{0,0,32,288},     //�»�� ����
	{32,0,1,288},     //�»�� ��� ���� 1px
	{270,0,1,288},    //�»�� ��� ���� 1px
	{32,238,48,50},   //�»�� ��� ���� ����
	{80,230,1,16},    //�»�� ��� ���� ���� �� 16px
	{112,238,158,50}, //�»�� ��� ���� ����
	{112,230,1,16},   //�»�� ��� ���� ���� �� 16px
	
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

bool isHit(player_status _player, player_status user_list[], int player_idx);


