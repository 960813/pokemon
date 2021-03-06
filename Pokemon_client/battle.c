﻿#include "battle.h"
#include "object.h"
#include "book.h"
#include "bag.h"
#include "otherUtils.h"
#include <math.h>
#include "conversation.h"
#include "sound.h"

extern battleUIStatus battleUI_status;
extern bagUIStatus bagUI_status;
extern float camera_position_x;
extern float camera_position_y;

extern pokemonMenuStatus pokemonMenu_status;
extern conversationStatus conversation_status;

extern ALLEGRO_FONT* get_convsPirnt_font();
extern ALLEGRO_FONT* get_pokemonmenu_hp_Print_font();

extern pokemon myPokemonList[6];
extern pokemon pokemonBook[15];
extern pokemon enemy;

// battle UI를 그리는 함수
void drawBasicUI() {
	// battle 배경 렌더링
	al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 0, 107, 240, 112, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y, 3.3333333, GAME_SCALE, 0, 0);

	char tmp_crt_hp[255] = { 0 }, tmp_max_hp[255] = { 0 }, tmp_level[255] = { 0 };
	// enemy HPbar 출력
	al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 246, 107, 100, 29, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 6 * GAME_SCALE, camera_position_y + 8 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);

	// 포켓몬 포획 단계라면
	if (battleUI_status.battleUICatching) {
		int tmpIdx = 1 + battleUI_status.catchingIdx++ / 12;
		int ball_offset_x = bagUI_status.currentIndex * 16;
		int ball_offset_y = -1;

		// 포획 결과에 따라 다른 인터랙션 수행
		// 포획 성공
		if (battleUI_status.catchingResult) {
			printf("CATCHING SUCCESS!\n");
			ball_offset_y = 23;
			// 포켓볼 출력
			al_draw_tinted_scaled_rotated_bitmap_region(objectBitmap, ball_offset_x, ball_offset_y, 12, 16, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 141.5 * GAME_SCALE, camera_position_y + 56 * GAME_SCALE, 3.33333333, GAME_SCALE, 0, 0);
			if ((tmpIdx / 6) < 4) {
				if (tmpIdx % 6 == 1) {
					// 202: SFX_BALL_BOUNCE
					soundHandler(202);
				}
				if (tmpIdx == 19) {
					// 203: SFX_CATCH_SUCCESS
					soundHandler(203);
				}
				battleUI_status.battleUIConv = true;
				battleUI_status.currentMenu = 8 + tmpIdx / 6;
	
			}
		}
		// 포획 실패
		else {
			printf("CATCHING FAIL!\n");
			ball_offset_y = 23;
			// 포켓볼 출력
			al_draw_tinted_scaled_rotated_bitmap_region(objectBitmap, ball_offset_x, ball_offset_y, 12, 16, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 141.5 * GAME_SCALE, camera_position_y + 56 * GAME_SCALE, 3.33333333, GAME_SCALE, 0, 0);
			if ((tmpIdx / 6) < 3) {
				if (tmpIdx % 6 == 1) {
					// 202: SFX_BALL_BOUNCE
					soundHandler(202);
				}
				battleUI_status.battleUIConv = true;
				battleUI_status.currentMenu = 8 + tmpIdx / 6;
			}
			else if (tmpIdx < 21) {
				ball_offset_y = (tmpIdx) == 18 ? 23 : (tmpIdx) == 19 ? 40 : 57;
				al_draw_tinted_scaled_rotated_bitmap_region(objectBitmap, ball_offset_x, ball_offset_y, 12, 16, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 141.5 * GAME_SCALE, camera_position_y + 56 * GAME_SCALE, 3.33333333, GAME_SCALE, 0, 0);
				if (ball_offset_y == 57){
					// 204: SFX_CATCH_FAIL
					soundHandler(204);
				}
			}
			else {
				soundHandler(101);
				battleUI_status.battleUICatching = false;
				battleUI_status.battleUIConv = false;
				battleUI_status.currentMenu = 0;
				battleUI_status.catchingIdx = 1;
			}
		}
		//printf("%s\n", battleUI_status.catchingResult ? "TRUE" : "FALSE");
	}
	// 포켓몬 포획 단계가 아니라면 적 포켓몬 렌더링
	else {
		al_draw_bitmap(enemy.front, camera_position_x + 115 * GAME_SCALE, camera_position_y + 13 * GAME_SCALE, 0);
	}
	// 적 포켓몬 현재 체력, 최대 체력, 레벨 출력
	sprintf_s(tmp_crt_hp, sizeof(tmp_crt_hp), "%d", enemy.crt_hp);
	sprintf_s(tmp_max_hp, sizeof(tmp_max_hp), "%d", enemy.max_hp);
	sprintf_s(tmp_level, sizeof(tmp_level), "%d", enemy.level);

	// 적 포켓몬 이름, 레벨 출력
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 11 * GAME_SCALE, camera_position_y + 14.5 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, enemy.displayName);
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 65 * GAME_SCALE, camera_position_y + 14.5 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, tmp_level);

	// 적 포켓몬 hpbar 그래프 출력(0.0~100.0)
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


	// 플레이어 포켓몬 출력
	// 포켓몬마다 크기가 다르기에 다른 위치에 출력
	al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 246, 148, 104, 37, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 105 * GAME_SCALE, camera_position_y + 74 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
	switch (myPokemonList[battleUI_status.currentPokemonIdx].no) {
	case 1:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 64 * GAME_SCALE, 0);
		break;
	case 2:
	case 3:
	case 4:
	case 8:
	case 9:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 58 * GAME_SCALE, 0);
		break;
	case 5:
	case 14:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 55 * GAME_SCALE, 0);
		break;
	case 6:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 52 * GAME_SCALE, 0);
		break;
	case 7:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 63 * GAME_SCALE, 0);
		break;
	case 10:
	case 15:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 56 * GAME_SCALE, 0);
		break;
	case 11:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 60 * GAME_SCALE, 0);
		break;
	case 12:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 54 * GAME_SCALE, 0);
		break;
	case 13:
		al_draw_bitmap(myPokemonList[battleUI_status.currentPokemonIdx].back, camera_position_x + 20 * GAME_SCALE, camera_position_y + 61 * GAME_SCALE, 0);
		break;
	}

	// 플레이어 포켓몬 현재 체력, 최대 체력, 레벨 버퍼에 저장
	sprintf_s(tmp_crt_hp, sizeof(tmp_crt_hp), "%d", myPokemonList[battleUI_status.currentPokemonIdx].crt_hp);
	sprintf_s(tmp_max_hp, sizeof(tmp_max_hp), "%d", myPokemonList[battleUI_status.currentPokemonIdx].max_hp);
	sprintf_s(tmp_level, sizeof(tmp_level), "%d", myPokemonList[battleUI_status.currentPokemonIdx].level);

	// 플레이어 포켓몬 이름 출력
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 117 * GAME_SCALE, camera_position_y + 80.5 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, myPokemonList[battleUI_status.currentPokemonIdx].displayName);

	// 플레이어 포켓몬 현재 체력, 최대 체력, 레벨 출력
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 153.5 * GAME_SCALE, camera_position_y + 96 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, tmp_crt_hp);
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 171.5 * GAME_SCALE, camera_position_y + 96 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, tmp_max_hp);
	al_draw_text(get_pokemonmenu_hp_Print_font(), al_map_rgb(64, 64, 64), camera_position_x + 171 * GAME_SCALE, camera_position_y + 80.5 * GAME_SCALE, ALLEGRO_ALIGN_LEFT, tmp_level);

	// 플레이어 포켓몬 HPbar 출력
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

	// 플레이어 포켓몬 경험치 현황 출력
	int pokemon_exp_pos_x = camera_position_x + 131.5 * GAME_SCALE;
	int pokemon_exp_pos_y = camera_position_y + 108 * GAME_SCALE;
	int pokemon_prev_level_exp = pow((double)myPokemonList[battleUI_status.currentPokemonIdx].level, 3.0);
	int pokemon_crt_level_exp = myPokemonList[battleUI_status.currentPokemonIdx].exp;
	int pokemon_next_level_exp = pow((double)myPokemonList[battleUI_status.currentPokemonIdx].level + 1, 3.0);
	double exp_matrix = (pokemon_crt_level_exp - (double)pokemon_prev_level_exp) / ((double)pokemon_next_level_exp - pokemon_prev_level_exp) * 214;
	al_draw_line(pokemon_exp_pos_x, pokemon_exp_pos_y, pokemon_exp_pos_x + exp_matrix, pokemon_exp_pos_y, al_map_rgb(64, 200, 248), 8);
}

// 포켓몬 레벨에 따른 제공 경험치 계산 및 반환
int calculateEXP(int level) {
	return 40 * level / 7;
}


extern bool is_key_pressed(int keycode);
// battle UI의 초기 메뉴(스킬, 가방, 포켓몬, 도망치기) 선택에 따른 인터랙션
void fightHandler() {
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
// 적 포켓몬의 공격 프로세스
void CPUattackProcess() {	
	int skillCnt = 0;
	int i = 0;
	for (i = 0; i < 4; i++)
		if (enemy.skill[i].own) skillCnt++;

	// 보유한 스킬 중 랜덤 스킬을 선택하여 사용
	int selectSkill = rand() % skillCnt;
	attackProcess(&enemy, &myPokemonList[battleUI_status.currentPokemonIdx], &enemy.skill[selectSkill]);
}

// 공격 프로세스
void attackProcess(pokemon* attacker, pokemon* defender, pokemonSkill* skill) {
	// SFX_DAMAGE
	soundHandler(400); 
	/*
		스킬의 타입과 적의 타입에 따른 시너지 계산
		피해량 랜덤 계수 정의(1.0 ~ 1.6)
		피해랑 계산
	*/
	double synergy = convertSynergy(skill->type, defender->type);
	double randRate = (((double)rand() / RAND_MAX) * 0.6) + 1.0;
	int dmg = (skill->dmg_cf * attacker->dmg - defender->def) * synergy * randRate;

	if (dmg < 0) dmg = 1;
	printf("SKILL-DAMAGE: %lf, %d / %lf\n", randRate, dmg, synergy);

	// 대상자의 HP 감소
	defender->crt_hp -= dmg;
	if (defender->crt_hp < 0) defender->crt_hp = 0;

	// 스킬 pp 감소
	skill->crt_pp -= 1;

	/*
		대상자가 사망했다면,
		> 경험치 획독
		> 경험치가 레벨업에 필요한 수치를 넘겼다면 레벨업, 진화 확인
	*/
	if (isDead(defender)) {
		int pokemon_grade = defender->no == 14 ? 3 : ((defender->no == 1) || (defender->no == 4) || (defender->no == 7)) ? 2 : 1;
		int increase_exp = (pokemon_grade * 40 * defender->level) / 7;
		int pokemon_next_level_exp = pow((double)attacker->level + 1, 3.0);

		if ((attacker->exp + increase_exp) > pokemon_next_level_exp) {
			pokemon tmpPokemon = createPokemon(attacker->no - 1, attacker->level + 1);
			*attacker = tmpPokemon;
			isEvolution(attacker);
		}
		// 최대 레벨은 99레벨
		attacker->exp += increase_exp;
		if (attacker->level == 99) attacker->exp = pow(99.0, 3.0);
	}
}

// battleUI 렌더링
void showBattleUI() {
	// battleUI가 열려있지 않다면 렌더링 취소
	if (!battleUI_status.battleUIOpen)
		return;

	int convsX = camera_position_x + 12 * GAME_SCALE;
	int convsY = camera_position_y + (GAME_HEIGHT - 48 * GAME_SCALE) + 11 * GAME_SCALE;

	int arrowPosition_x = camera_position_x + 107 * GAME_SCALE;
	int arrowPosition_y = camera_position_y + 124 * GAME_SCALE;

	int arrowOffset_x = 0;
	int arrowOffset_y = 0;

	// 기본 UI 렌더링
	drawBasicUI();
	switch (battleUI_status.currentMenu) {
	case 0:
		// 기본 battleUI
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 0, 0, 120, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x + 400, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);

		switch (battleUI_status.currentIndex) {
		case 1:
			// BAG:: 선택 화살표 offeset X 수정
			arrowOffset_x = 46.5 * GAME_SCALE;
			break;
		case 2:
			// POKEMON:: 선택 화살표 offset Y 수정
			arrowOffset_y = 17 * GAME_SCALE;
			break;
		case 3:
			// RUN:: 선택 화살표 offset X, Y 수정
			arrowOffset_x = 46.5 * GAME_SCALE;
			arrowOffset_y = 17 * GAME_SCALE;
			break;
		}
		// 화살표 출력
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 123, 0, 6, 10, al_map_rgb(255, 255, 255), 0, 0, arrowPosition_x + arrowOffset_x, arrowPosition_y + arrowOffset_y, 3.3333333, GAME_SCALE, 0, 0);
		break;
	case 1:
		// 스킬 선택
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 0, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		fightHandler();
		break;
	case 2:
		// 가방 선택
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		//al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "BAG!");
		drawBagUI();
		break;
	case 3:
		// 포켓몬 메뉴 선택
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		//al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "POKEMON!");
		showPokemonMenu();
		break;
	case 4:
		// 도망가기 선택
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "RUN!");
		break;
	case 5:
		// 배틀 종료
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "BATTLE END!");
		break;
	case 6:
		// 플레이어 포켓몬 공격 종료
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "Your attack hit the enemy!");
		break;
	case 7:
		// 적 포켓몬 공격 종료
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "The enemy's attack hit you!");
		break;
	case 8:
		// 포켓몬 포획 중 - 1단계
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "Catching .");
		break;
	case 9:
		// 포켓몬 포획 중 - 2단계
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "Catching . .");
		break;
	case 10:
		// 포켓몬 포획 중 - 3단계
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "Catching . . .");
		break;
	case 11:
		// 포켓몬 포획 성공
		al_draw_tinted_scaled_rotated_bitmap_region(battleUIBitmap, 132, 52, 240, 48, al_map_rgb(255, 255, 255), 0, 0, camera_position_x, camera_position_y + 112 * GAME_SCALE, 3.3333333, GAME_SCALE, 0, 0);
		al_draw_text(get_convsPirnt_font(), al_map_rgb(255, 255, 255), convsX, convsY, ALLEGRO_ALIGN_LEFT, "You have successfully catched!");
		break;
	}
}