﻿#include "bag.h"
#include "battle.h"
#include "catching.h"
#include "conversation.h"
#include "sound.h"
#include "object.h"
#include "otherUtils.h"

extern float camera_position_x;
extern float camera_position_y;

extern ALLEGRO_FONT* get_menuPirnt_font();


extern battleUIStatus battleUI_status;
extern bagUIStatus bagUI_status;
extern pokemonMenuStatus pokemonMenu_status;
extern conversationStatus conversation_status;

/*
	인벤토리 슬롯, 2개의 카테고리(회복제/ 포켓볼)
	한 카테고리당 최대 6개의 아이템 수록
*/
inventoryItem inventorySlots[2][6] = {
	// 회복제
	{
		{1, 0, 200, "POTION", {"It restores the HP of one Pokemon","by 20 points."}},
		{2, 0, 500, "SUPER POTION", {"It restores the HP of one Pokemon","by 50 points."}},
		{3, 0, 1500,"HYPER POTION", {"It restores the HP of one Pokemon","by 200 points."}},
		{4, 0, 2500, "MAX POTION", {"It fully restores the HP of one Poke", "mon."}},
		{5, 0, 2000, "ETHER", {"Restores All skills's PP by 10", "points for one Pokemon"}},
		{6, 0, 3000, "REVIVE", {"Revives a fainted Pokemon, restoring", "HP by half the maximum amount."}},
	},
	// 포켓볼
	{
		{7, 0, 1000, "Poke BALL", {"A BALL thrown to catch a wild Poke", "mon. It is designed in a capsule style."}},
		{8, 0, 1500, "GREAT BALL", {"A good, quality BALL that offers", "a higher Pokemon catch rate than", "a standard Poke BALL."}},
		{9, 0, 2500, "ULTRA BALL", {"A very high-grade BALL that offers", "a higher Pokemon catch rate", "a GREAT BALL."}},
		{10, 0, 50000, "MASTER BALL", {"The best BALL with the ultimate", "performance. It will catch any wild","Pokemon without fail."}}
	}
};

// 아이템 상호작용
void interactItem(int itemNo, pokemon* target) {
	inventoryItem* item = &inventorySlots[bagUI_status.currentMenu][bagUI_status.currentIndex];
	printf("CLICK:%s\n", item->itemName);

	// 아이템 재고가 없다면 상호작용 취소
	if (item->itemStock <= 0) return;

	// 가방 메뉴 인덱스가 0일 때(회복아이템)
	if (bagUI_status.currentMenu == 0) {
		switch (itemNo) {
		case COMMON_POTION:
			/*
				POTION 사용시
				포켓몬이 살아있다면 체력을 20회복하고 POTION 재고를 1 감소
			*/
			if (!isDead(target)) {
				target->crt_hp += 20;
				item->itemStock -= 1;
			}
			break;
		case COMMON_SUPER_POTION:
			/*
				SUPER POTION 사용시
				포켓몬이 살아있다면 체력을 50회복하고 SUPER POTION 재고를 1 감소
			*/
			if (!isDead(target)) {
				target->crt_hp += 50;
				item->itemStock -= 1;
			}
			break;
		case COMMON_HYPER_POTION:
			/*
				HYPER POTION 사용시
				포켓몬이 살아있다면 체력을 200회복하고 HYPER POTION 재고를 1 감소
			*/
			if (!isDead(target)) {
				target->crt_hp += 200;
				item->itemStock -= 1;
			}
			break;
		case COMMON_MAX_POTION:
			/*
				MAX POTION 사용시
				포켓몬이 살아있다면 체력을 모두 회복하고 MAX POTION 재고를 1 감소
			*/
			if (!isDead(target)) {
				target->crt_hp = target->max_hp;
				item->itemStock -= 1;
			}
			break;
		case COMMON_ETHER:
			/*
				ETHER 사용시
				포켓몬이 살아있다면 모든 스킬의 pp를 10씩 회복하고 ETHER 재고를 1 감소
			*/
			if (!isDead(target)) {
				for (int i = 0; i < 4; i++) {
					if (target->skill[i].own) {
						target->skill[i].crt_pp += 10;
						if (target->skill[i].crt_pp > target->skill[i].max_pp)
							target->skill[i].crt_pp = target->skill[i].max_pp;
					}
				}
			}
			item->itemStock -= 1;
			break;
		case COMMON_REVIVE:
			/*
				REVIVE 사용시
				포켓몬이 죽어있다면, 체력을 절반 회복하고 ETHER 재고를 1 감소
			*/
			if (isDead(target)) {
				target->crt_hp = target->max_hp / 2;
				item->itemStock -= 1;
			}
			break;
		}
		// 만약 현재 HP가 최대 HP를 초과한다면 초과분을 절삭한다.
		if (target->crt_hp > target->max_hp)
			target->crt_hp = target->max_hp;


		conversation_status.currentConvs = 4;
		conversation_status.maxIndex = 2;
		conversation_status.convsOpen = true;
	}
	// 가방 메뉴 인덱스가 1일 때(포켓볼)
	else {
		double catchingRate = 0.0;
		double pokemon_grade_rate = target->no == 14 ? 30.0 : ((target->no == 1) || (target->no == 4) || (target->no == 7)) ? 50.0 : 70.0;
		switch (itemNo) {
		case COMMON_POKEBALL:
			/*
				일반 포켓볼 사용시 CATCHING_MOD_POKEBALL에 따른 포획 확률 공식 적용
			*/
			catchingRate = ((1.0 - (2.0 / 3.0 * target->crt_hp / target->max_hp)) * pokemon_grade_rate * CATCHING_MOD_POKEBALL + 1) * 255.0 / 256.0;
			break;
		case COMMON_GREATBALL:
			/*
				GREAT BALL 사용시 CATCHING_MOD_GREATBALL에 따른 포획 확률 공식 적용
			*/
			catchingRate = ((1.0 - (2.0 / 3.0 * target->crt_hp / target->max_hp)) * pokemon_grade_rate * CATCHING_MOD_GREATBALL + 1) * 255.0 / 256.0;
			break;
		case COMMON_ULTRABALL:
			/*
				ULTRA BALL 사용시 CATCHING_MOD_ULTRABALL에 따른 포획 확률 공식 적용
			*/
			catchingRate = ((1.0 - (2.0 / 3.0 * target->crt_hp / target->max_hp)) * pokemon_grade_rate * CATCHING_MOD_ULTRABALL + 1) * 255.0 / 256.0;
			break;
		case COMMON_MASTERBALL:
			/*
				MASTER BALL 사용시 CATCHING_MOD_MASTERBALL에 따른 포획 확률 공식 적용
			*/
			catchingRate = ((1.0 - (2.0 / 3.0 * target->crt_hp / target->max_hp)) * pokemon_grade_rate * CATCHING_MOD_MASTERBALL + 1) * 255.0 / 256.0;
			break;
		}

		// 계산된 포획 확률을 기반으로 포획 성공 여부 계산
		if (catchingRate >= rand() % 100)
			battleUI_status.catchingResult = true;
		else
			battleUI_status.catchingResult = false;

		// catching에 따른 UI 출력
		battleUI_status.battleUICatching = true;
		inventorySlots[bagUI_status.currentMenu][bagUI_status.currentIndex].itemStock -= 1;
		bagUI_status.currentIndex = itemNo - 9;
		bagUI_status.bagUIOpen = false;
		bagUI_status.currentMenu = 0;
	}
}
extern pokemon enemy;

// 인벤토리 상호작용
void interactInventory() {
	// 배틀 중이 아닌데, 포켓볼을 사용했다면 상호작용 취소
	if (!battleUI_status.battleUIOpen && bagUI_status.currentMenu != 0)
		return;

	// 아이템 재고가 없다면 상호작용 취소
	if (inventorySlots[bagUI_status.currentMenu][bagUI_status.currentIndex].itemStock <= 0)
		return;

	// 가방 메뉴 인덱스 0일 때(회복)
	if (bagUI_status.currentMenu == 0) {
		pokemonMenu_status.pokemonMenuOpen = true;
		pokemonMenu_status.currentIndex = 0;
	}
	// 가방 메뉴 인덱스 1일 때(포켓볼)
	else {
		interactItem(bagUI_status.currentIndex + 9, &enemy);
	}
}



void drawBagUI() {
	// 가방이 닫겨있다면 가방 렌더링 취소
	if (!bagUI_status.bagUIOpen) return;

	// 가방 배경 렌더링
	al_draw_tinted_scaled_rotated_bitmap_region(bagUIBitmap, 0, 0, 240, 160, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y, 3.3333333, GAME_SCALE, 0, 0);

	// 가방 오프셋(X) Y는 163 가로 58 높이 63
	int bagSprite_offsetX = -1;
	// 렌더링 위치
	int bagSprite_posX = 11;
	int bagSprite_posY = 40;

	// 메뉴 오프셋(Y), X는 0으로 고정
	int itemMenuSprite_offsetY = -1;


	switch (bagUI_status.currentMenu) {
	case 0:
		itemMenuSprite_offsetY = 163;
		bagSprite_offsetX = 105;
		break;
	case 1:
		itemMenuSprite_offsetY = 195;
		bagSprite_offsetX = 167;
		break;
	}

	// 가방
	al_draw_tinted_scaled_rotated_bitmap_region(bagUIBitmap, bagSprite_offsetX, 163, 58, 63, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + bagSprite_posX * 3.3333333, camera_position_y + bagSprite_posY * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);

	// 메뉴
	al_draw_tinted_scaled_rotated_bitmap_region(bagUIBitmap, 0, itemMenuSprite_offsetY, 77, 29, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 3 * 3.3333333, camera_position_y + 1 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);

	// 하단 대화창 아이콘 렌더링 위치
	//int bottomIcon_posX = 10;
	//int bottomIcon_posY = 126;
	//al_draw_tinted_scaled_rotated_bitmap_region(bagUIBitmap, 80, 181, 20, 20, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + bottomIcon_posX * 3.3333333, camera_position_y + bottomIcon_posY * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);

	int bottomIcon_posX = -1;
	int bottomIcon_posY = -1;

	switch (bagUI_status.currentMenu) {
	case 0:
		bottomIcon_posX = 12;
		bottomIcon_posY = 125;
		break;
	case 1:
		bottomIcon_posX = 14;
		bottomIcon_posY = 130;
		break;
	}

	inventoryItem item = inventorySlots[bagUI_status.currentMenu][bagUI_status.currentIndex];
	al_draw_tinted_scaled_rotated_bitmap_region(objectBitmap, objectItem[item.itemType][0], objectItem[item.itemType][1], objectItem[item.itemType][2], objectItem[item.itemType][3], al_map_rgb(255, 255, 255), 0, 0, camera_position_x + bottomIcon_posX * 3.3333333, camera_position_y + bottomIcon_posY * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);

	// 하단 Description
	int bottomDesc_posX = 33;
	int bottomDesc_posY = 122;
	for (int i = 0; i < 3; i++)
		al_draw_text(get_menuPirnt_font(), al_map_rgb(255, 255, 255), camera_position_x + bottomDesc_posX * GAME_SCALE, camera_position_y + (bottomDesc_posY + i * 9) * GAME_SCALE, ALLEGRO_ALIGN_LEFT, item.itemDesc[i]);

	// 가로 9 세로14
	// 메뉴 좌우 화살표(적색) 오프셋, Y는 163고정
	int redArrow_Left_offsetX = 80;
	int redArrow_Right_offsetX = 92;

	// 메뉴 좌우 화살표(적색) 렌더링 위치, Y는 4 고정
	int redArrow_Left_posX = 3;
	int redArrow_Right_posX = 68;

	al_draw_tinted_scaled_rotated_bitmap_region(bagUIBitmap, redArrow_Left_offsetX, 163, 9, 14, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + redArrow_Left_posX * 3.3333333, camera_position_y + 70 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
	al_draw_tinted_scaled_rotated_bitmap_region(bagUIBitmap, redArrow_Right_offsetX, 163, 9, 14, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + redArrow_Right_posX * 3.3333333, camera_position_y + 70 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);

	// 메뉴 선택 화살표(흑색) 렌더링 위치, X는 90 고정
	int blackArrow_posY = bagUI_status.currentIndex * 15.5 + 13;

	// 메뉴 선택 화살표(흑색) 오프셋 위치
	int blackArrow_offsetX = 81;
	int blackArrow_offsetY = 204;
	al_draw_tinted_scaled_rotated_bitmap_region(bagUIBitmap, blackArrow_offsetX, blackArrow_offsetY, 6, 10, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 90 * 3.3333333, camera_position_y + blackArrow_posY * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);


	// 회복약 계열: 가로(17), 세로(23)
	// 몬스터볼 계열: 가로(12), 세로(12)
	int item_offsetX = -1;
	int item_offsetY = -1;
	int item_posX = 82;
	int item_posY = -1;
	int item_img_width = -1;
	int item_img_height = -1;

	for (int i = 0; i <= bagUI_status.lastIndex; i++) {
		// 보유 아이템 목록
		item = inventorySlots[bagUI_status.currentMenu][i];
		item_posY = i * 15.5 + 13;
		al_draw_text(get_menuPirnt_font(), al_map_rgb(90, 90, 90), camera_position_x + item_posX * GAME_SCALE, camera_position_y + item_posY * GAME_SCALE, ALLEGRO_ALIGN_LEFT, item.itemName);
		char stockMSG[255] = { 0 };
		// 디버그용 아이템 개수 99개 설정
		//inventorySlots[bagUI_status.currentMenu][i].itemStock = 99;
		sprintf_s(stockMSG, sizeof(stockMSG), "X %d", item.itemStock);
		al_draw_text(get_menuPirnt_font(), al_map_rgb(90, 90, 90), camera_position_x + (item_posX + 84) * GAME_SCALE, camera_position_y + item_posY * GAME_SCALE, ALLEGRO_ALIGN_LEFT, stockMSG);
	}
}

// 가방 메뉴를 닫는 함수
void closeBagMenu() {
	// 배틀중이라면, 배틀 Conv 닫기
	if (battleUI_status.battleUIOpen) {
		battleUI_status.battleUIConv = false;
		battleUI_status.currentIndex = battleUI_status.currentMenu - 1;
		battleUI_status.currentMenu = 0;
	}
	// 가방 닫기
	bagUI_status.currentIndex = -1;
	bagUI_status.lastIndex = -1;
	bagUI_status.bagUIOpen = false;
}