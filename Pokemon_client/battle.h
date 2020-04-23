﻿#pragma once
#ifndef _NGINX_BATTLE_HEADER_
#define _NGINX_BATTLE_HEADER_
#include "nginx_common.h"
#include "book.h"

typedef struct BATTLEUI_STATUS {
	bool battleUIOpen;
	bool battleUIConv;
	bool battleUISkill;
	int currentMenu;
	int currentIndex;
	int currentPokemonIdx;
	int enemyPokemonIdx;
} battleUIStatus;

#define TOTAL_APPEAR_RATE 0.1
#define GRADE_1_APPEAR_RATE 0.65
#define GRADE_2_APPEAR_RATE 0.3
#define GRADE_3_APPEAR_RATE 0.05

ALLEGRO_BITMAP* battleUIBitmap;

void showBattleUI();
#endif