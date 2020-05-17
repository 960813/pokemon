﻿#include "nginx_framework.h"
#include "player.h"
#include "map.h"
#include "object.h"
#include "screen.h"
#include "sock_client_framework.h"
#include "collision.h"
#include "portal.h"
#include "bush.h"
#include "chat.h"
#include "book.h"
#include "menu.h"
#include "battle.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conversation.h"
#include "environment.h"

int GAME_SPEED = 1;
int GAME_STAGE = 0;

player user_player = { NULL, "", 0,1,0,0,false,0,0 };
pokemon myPokemonList[6] = {
	 { -1,"",0,0,0,0,0,0,0,0},
	 { -1,"",0,0,0,0,0,0,0,0},
	 { -1,"",0,0,0,0,0,0,0,0},
	 { -1,"",0,0,0,0,0,0,0,0},
	 { -1,"",0,0,0,0,0,0,0,0},
	 { -1,"",0,0,0,0,0,0,0,0}
};

menuStatus menu_status = { false, -1, 0 ,0 };
conversationStatus conversation_status = { false, -1, 0,0 };
pokemonThumbStatus pokemonThumb_status = { false, -1 };
pokemonMenuStatus pokemonMenu_status = { false, -1 };
battleUIStatus battleUI_status = { false, false, false, false, -1, -1, 0 };

// player_bitmap*, action_bitmap*, name, action_type, player_direction, action_idx, pos_x, pos_y, hp, armor, buf
extern ALLEGRO_BITMAP* _map[3] = { NULL };
ALLEGRO_BITMAP* _object = NULL;

pokemon enemy;

extern ALLEGRO_USTR* chatInput;
extern bool onChat;

extern pokemon pokemonBook[15];
void update()
{
	// 포켓몬 메뉴
	if (pokemonMenu_status.pokemonMenuOpen) {
		if (is_key_pressed(ALLEGRO_KEY_UP) || is_key_pressed(ALLEGRO_KEY_LEFT)) {
			if (pokemonMenu_status.currentIndex > 0) {
				pokemonMenu_status.currentIndex--;
				if (myPokemonList[pokemonMenu_status.currentIndex].no == -1)
					pokemonMenu_status.currentIndex = 0;
			}
			else
				pokemonMenu_status.currentIndex = 6;
		}
		if (is_key_pressed(ALLEGRO_KEY_DOWN) || is_key_pressed(ALLEGRO_KEY_RIGHT)) {
			if (pokemonMenu_status.currentIndex < 6) {
				pokemonMenu_status.currentIndex++;
				if (myPokemonList[pokemonMenu_status.currentIndex].no == -1)
					pokemonMenu_status.currentIndex = 6;
			}
			else
				pokemonMenu_status.currentIndex = 0;
		}
		if (is_key_pressed(ALLEGRO_KEY_Z) || is_key_pressed(ALLEGRO_KEY_ENTER)) {
			switch (pokemonMenu_status.currentIndex) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			{
				pokemon tmp = myPokemonList[0];
				myPokemonList[0] = myPokemonList[pokemonMenu_status.currentIndex];
				myPokemonList[pokemonMenu_status.currentIndex] = tmp;
				if (battleUI_status.battleUIOpen)
					closePokemonMenu();
			}
			break;
			case 6:
				closePokemonMenu();
				break;
			}
		}
		if (is_key_pressed(ALLEGRO_KEY_ESCAPE) || is_key_pressed(ALLEGRO_KEY_X)) {
			closePokemonMenu();
		}

	}
	// 배틀메뉴 초기화면(4개 메뉴)
	else if (battleUI_status.battleUIOpen) {
		if (battleUI_status.battleUIConv) {
			if (is_key_pressed(ALLEGRO_KEY_Z) || is_key_pressed(ALLEGRO_KEY_ENTER) || is_key_pressed(ALLEGRO_KEY_X)) {
				battleUI_status.battleUIConv = false;
				if (battleUI_status.currentMenu == 4 || battleUI_status.currentMenu == 5) {
					fadeOut(0.05);
					battleUI_status.battleUIOpen = false;
					battleUI_status.currentMenu = -1;
					battleUI_status.currentIndex = -1;
					battleUI_status.currentPokemonIdx = -1;
					fadeIn(0.05);
				}
				else if (battleUI_status.currentMenu == 6) {
					if (isDead(&enemy)) {
						battleUI_status.battleUISkill = false;
						battleUI_status.currentMenu = 0;
						battleUI_status.currentIndex = 0;
					}
					else {
						CPUattackProcess();
						battleUI_status.battleUIConv = true;
						battleUI_status.currentMenu = 7;
						battleUI_status.currentIndex = 0;
					}
				}
				else if (battleUI_status.currentMenu == 7) {
					battleUI_status.battleUISkill = false;
					battleUI_status.currentMenu = 0;
					battleUI_status.currentIndex = 0;
				}
				else {
					battleUI_status.currentIndex = battleUI_status.currentMenu - 1;
					battleUI_status.currentMenu = 0;
				}
			}
		}
		// 스킬 선택 메뉴
		else if (battleUI_status.battleUISkill) {
			if (is_key_pressed(ALLEGRO_KEY_UP) || is_key_pressed(ALLEGRO_KEY_LEFT)) {
				if (battleUI_status.currentIndex > 0 && myPokemonList[battleUI_status.currentPokemonIdx].skill[battleUI_status.currentIndex - 1].own == true)
					battleUI_status.currentIndex--;
				else {
					for (int i = 3; i >= 1; i--) {
						if (myPokemonList[battleUI_status.currentPokemonIdx].skill[i].own == true) {
							battleUI_status.currentIndex = i;
							break;
						}
					}
				}
			}
			if (is_key_pressed(ALLEGRO_KEY_DOWN) || is_key_pressed(ALLEGRO_KEY_RIGHT)) {
				if (battleUI_status.currentIndex < 3 && myPokemonList[battleUI_status.currentPokemonIdx].skill[battleUI_status.currentIndex + 1].own == true)
					battleUI_status.currentIndex++;
				else
					battleUI_status.currentIndex = 0;
			}

			// 스킬 선택
			if (is_key_pressed(ALLEGRO_KEY_Z) || is_key_pressed(ALLEGRO_KEY_ENTER)) {
				attackProcess(&myPokemonList[battleUI_status.currentPokemonIdx], &enemy, &myPokemonList[battleUI_status.currentPokemonIdx].skill[battleUI_status.currentIndex]);
				printf("select Skill's displayName : %s\n", myPokemonList[battleUI_status.currentPokemonIdx].skill[battleUI_status.currentIndex].displayName);
				battleUI_status.battleUIConv = true;
				battleUI_status.currentMenu = 6;
				battleUI_status.currentIndex = 0;
			}

			if (is_key_pressed(ALLEGRO_KEY_X) || is_key_pressed(ALLEGRO_KEY_ESCAPE)) {
				battleUI_status.battleUISkill = false;
				battleUI_status.currentMenu = 0;
				battleUI_status.currentIndex = 0;
			}
		}
		// 배틀 종료
		else if (battleUI_status.battleUIEnd) {
			battleUI_status.battleUIEnd = false;
			battleUI_status.battleUIConv = true;
			battleUI_status.currentMenu = 5;
		}
		// 그 외
		else {
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
			if (is_key_pressed(ALLEGRO_KEY_Z) || is_key_pressed(ALLEGRO_KEY_ENTER)) {
				battleUI_status.currentMenu = battleUI_status.currentIndex + 1;
				switch (battleUI_status.currentMenu) {
				case 1:
					battleUI_status.battleUISkill = true;
					battleUI_status.currentMenu = battleUI_status.currentIndex + 1;
					battleUI_status.currentIndex = 0;
					break;
				case 2:
					battleUI_status.battleUIConv = true;
					break;
				case 3:
					pokemonMenu_status.pokemonMenuOpen = true;
					pokemonMenu_status.currentIndex = 0;
					battleUI_status.battleUIConv = true;
					break;
				case 4:
					battleUI_status.battleUIConv = true;
					break;
				}

			}
			if (battleUI_status.currentMenu != -1 && is_key_pressed(ALLEGRO_KEY_X)) {
				battleUI_status.currentMenu = 0;
				battleUI_status.currentIndex = 0;
			}
		}
	}
	// 일반 메뉴
	else if (menu_status.menuOpen) {
		if (is_key_pressed(ALLEGRO_KEY_UP)) {
			if (menu_status.menuIndex > 0)
				menu_status.menuIndex--;
			else
				menu_status.menuIndex = menu_status.maxMenuIndex - 1;
		}
		if (is_key_pressed(ALLEGRO_KEY_DOWN)) {
			if (menu_status.menuIndex < menu_status.maxMenuIndex - 1)
				menu_status.menuIndex++;
			else
				menu_status.menuIndex = 0;
		}
		if (is_key_pressed(ALLEGRO_KEY_Z) || is_key_pressed(ALLEGRO_KEY_ENTER)) {
			menuHandler();
		}

		if (is_key_pressed(ALLEGRO_KEY_ESCAPE) || is_key_pressed(ALLEGRO_KEY_X)) {
			closeMenu();
			closeConversation();
			closePokemonThumb();
		}
	}
	// 대화창
	else if (conversation_status.convsOpen) {
		if (is_key_pressed(ALLEGRO_KEY_Z) || is_key_pressed(ALLEGRO_KEY_ENTER) || is_key_pressed(ALLEGRO_KEY_ESCAPE) || is_key_pressed(ALLEGRO_KEY_X)) {
			closeConversation();
		}
	}
	// 기타(아무것도 안함)
	else {
		if (is_key_pressed(ALLEGRO_KEY_ESCAPE))
			quit();
		if (user_player.bMoveFlag == false) {
			if (is_key_down(ALLEGRO_KEY_DOWN)) {
				user_player.bMoveFlag = true;
				user_player.iAction_type = 1;
				user_player.iPlayer_direction = CHARACTER_DOWN;
			}
			if (is_key_down(ALLEGRO_KEY_RIGHT)) {
				user_player.bMoveFlag = true;
				user_player.iAction_type = 1;
				user_player.iPlayer_direction = CHARACTER_RIGHT;
			}
			if (is_key_down(ALLEGRO_KEY_UP)) {
				user_player.bMoveFlag = true;
				user_player.iAction_type = 1;
				user_player.iPlayer_direction = CHARACTER_UP;
			}
			if (is_key_down(ALLEGRO_KEY_LEFT)) {
				user_player.bMoveFlag = true;
				user_player.iAction_type = 1;
				user_player.iPlayer_direction = CHARACTER_LEFT;
			}

			if (is_key_pressed(ALLEGRO_KEY_Z)) {
				int objId = isObject(user_player);
				if (objId != -1)
					interactObject(objId);
			}
		}
		if (is_key_pressed(ALLEGRO_KEY_ENTER)) {
			menu_status.currentMenu = GLOBAL_MENU;
			menu_status.menuOpen = true;
		}
	}


	if (user_player.bMoveFlag) {
		user_player.iAction_idx++;
		switch (user_player.iPlayer_direction)
		{
		case CHARACTER_DOWN:
			user_player.iPos_y += GAME_MOVE_TICK * GAME_SCALE;
			if (isCollision(user_player))
				user_player.iPos_y -= GAME_MOVE_TICK * GAME_SCALE;
			break;
		case CHARACTER_RIGHT:
			user_player.iPos_x += GAME_MOVE_TICK * GAME_SCALE;
			if (isCollision(user_player))
				user_player.iPos_x -= GAME_MOVE_TICK * GAME_SCALE;
			break;
		case CHARACTER_UP:
			user_player.iPos_y -= GAME_MOVE_TICK * GAME_SCALE;
			if (isCollision(user_player))
				user_player.iPos_y += GAME_MOVE_TICK * GAME_SCALE;
			break;
		case CHARACTER_LEFT:
			user_player.iPos_x -= GAME_MOVE_TICK * GAME_SCALE;
			if (isCollision(user_player))
				user_player.iPos_x += GAME_MOVE_TICK * GAME_SCALE;
			break;
		}

		if (user_player.iAction_idx >= 16)
		{
			user_player.iAction_type = 0;
			user_player.iAction_idx = 0;
			user_player.iAction_mode = user_player.iAction_mode == 1 ? 2 : 1;

			int stageChanges = isPortal(user_player);
			if (stageChanges != -4) {
				GAME_STAGE = stageChanges;
				printf("main.c->GAME_STAGE%d\n", GAME_STAGE);
				fadeOut(0.05);
				init_terrain(_map[mapOffset[GAME_STAGE][0]]);
				user_player.iPos_x = mapOffset[GAME_STAGE][1] + mapOffset[GAME_STAGE][7] * GAME_SCALE;
				user_player.iPos_y = mapOffset[GAME_STAGE][2] + mapOffset[GAME_STAGE][8] * GAME_SCALE - 16;
				updateCamera(user_player);
				fadeIn(0.05);
				clear_key_buffered();
				initCollision();
			}

			if (GAME_STAGE == 2 || GAME_STAGE == 5 || GAME_STAGE == 7) {
				int bushJoins = isBush(user_player);
				if (bushJoins != -4) {
					double randItem = ((double)rand() / RAND_MAX * 1.0);
					if (randItem <= TOTAL_APPEAR_RATE) {
						fadeOut(0.02);
						fadeIn(0.02);
						fadeOut(0.02);
						fadeIn(0.02);
						fadeOut(0.02);

						battleUI_status.battleUIOpen = true;
						battleUI_status.currentMenu = 0;
						battleUI_status.currentIndex = 0;
						battleUI_status.currentPokemonIdx = 0;

						randItem = ((double)rand() / RAND_MAX * 1.0);
						if (randItem <= GRADE_3_APPEAR_RATE) {
							int idxArr[] = { 13 };
							int randIdx = rand() % (sizeof(idxArr) / sizeof(int));
							battleUI_status.enemyPokemonIdx = idxArr[randIdx];
						}
						else if (randItem <= GRADE_3_APPEAR_RATE + GRADE_2_APPEAR_RATE) {
							int idxArr[] = { 0,3,6 };
							int randIdx = rand() % (sizeof(idxArr) / sizeof(int));
							battleUI_status.enemyPokemonIdx = idxArr[randIdx];
						}
						else if (randItem <= GRADE_3_APPEAR_RATE + GRADE_2_APPEAR_RATE + GRADE_1_APPEAR_RATE) {
							int idxArr[] = { 9, 12 };
							int randIdx = rand() % (sizeof(idxArr) / sizeof(int));
							battleUI_status.enemyPokemonIdx = idxArr[randIdx];
						}
						enemy = createPokemon(battleUI_status.enemyPokemonIdx, 5);
						showBattleUI();
						fadeIn(0.03);
						clear_key_buffered();
					}
				}
			}
			user_player.bMoveFlag = false;

			sendPlayerStatus("PLAYER", user_player);
		}
	}

	updateCamera(user_player);


	//if (is_key_pressed(ALLEGRO_KEY_ENTER))
	//{
	//	if (onChat && chatInput->slen > 0)
	//	{
	//		const char* txt = al_cstr(chatInput);
	//		json_t* pHeader = json_array();
	//		json_t* pData = json_array();
	//		json_array_append_new(pHeader, json_string("CHAT"));
	//		json_array_append_new(pData, json_string(user_player.cName));
	//		json_array_append_new(pData, json_string(txt));

	//		json_t* pMessage = htonJson(pHeader, pData);

	//		sendMessage(pMessage);
	//		chatInput = al_ustr_new("");
	//	}
	//	onChat = !onChat;
	//}

	//showChat(camera_position_x, camera_position_y + (GAME_HEIGHT - 120));
}

int move_tick = 0;
void render()
{
	al_draw_bitmap(world_map, 0, 0, 0);
	switch (user_player.iAction_type)
	{
	case 0:
		movement_character(user_player._player, user_player.iPos_x, user_player.iPos_y, user_player.iPlayer_direction, user_player.iAction_mode, user_player.iAction_idx);
		break;
	case 1:
		movement_character(user_player._player, user_player.iPos_x, user_player.iPos_y, user_player.iPlayer_direction, user_player.iAction_mode, user_player.iAction_idx / 8);
		break;
	}
	showMenu(menu_status.currentMenu);
	showConversation(conversation_status.currentConvs);
	showPokemonThumb(pokemonThumb_status.currentThumb);
	showPoekmonMenu();

	showBattleUI();
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));
	//gets_s(user_player.cName, sizeof(user_player.cName));
	bind_sock_clnt();

	// must be called first!l
	init_framework("Pokemon By JupiterFlow.com", GAME_WIDTH, GAME_HEIGHT, false);
	_map[0] = al_load_bitmap("gfx/Pallet_Town_Interiors.png");
	_map[1] = al_load_bitmap("gfx/Pallet_Town_Full.png");
	_map[2] = al_load_bitmap("gfx/Gym_Inner.png");
	for (int i = 0; i < 3; i++) {
		if (_map[i] == NULL)
			printf("%d _map not load\n", i);
	}
	menuFrame = al_load_bitmap("gfx/MenuFrame.png");
	if (menuFrame == NULL)
		printf("menuFrame not load\n");
	pokemonBitmap = al_load_bitmap("gfx/pokemonBook.png");
	if (pokemonBitmap == NULL)
		printf("pokemonBitmap not load\n");
	pokemonMenuBitmap = al_load_bitmap("gfx/pokemonMenu.png");
	al_convert_mask_to_alpha(pokemonMenuBitmap, al_map_rgb(0, 128, 0));
	if (pokemonMenuBitmap == NULL)
		printf("pokemonMenuBitmap not load\n");
	user_player._player = al_load_bitmap("gfx/Hero&Heroine.png");
	al_convert_mask_to_alpha(user_player._player, al_map_rgb(255, 200, 106));

	battleUIBitmap = al_load_bitmap("gfx/BattleUI.png");
	al_convert_mask_to_alpha(battleUIBitmap, al_map_rgb(163, 73, 164));
	if (battleUIBitmap == NULL)
		printf("battleUIBitmap not load\n");

	// 디버깅용, 스테이지 임의 이동
	//user_player.iPos_x = mapOffset[GAME_STAGE][1] + mapOffset[GAME_STAGE][7] * GAME_SCALE;
	//user_player.iPos_y = mapOffset[GAME_STAGE][2] + mapOffset[GAME_STAGE][8] * GAME_SCALE - 16;

	world_map = al_create_bitmap(GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

	initPokemonThumb();
	initPokemonSkill();

	environmentLoad();

	init_terrain(_map[mapOffset[GAME_STAGE][0]]);
	initCollision();



	sendPlayerStatus("JOIN_GAME", user_player);

	// the game loop runs until we call quit()
	run_game_loop(update, render);
	return 0;
}