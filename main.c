#include <avr/io.h>
#include <stdio.h>
#include <bit.h>
#include <timer.h>
#include <ledmatrix.h>
#include <stdlib.h>
#include "io.c"
#include <string.h>
unsigned char enemies_1_temp;
unsigned char enemies_2_temp;
unsigned char enemies_3_temp;
unsigned char enemies_4_temp;
void EEPROM_write(unsigned int uiAddress, unsigned char ucData){
	while(EECR & (1<<EEPE));
	EEAR = uiAddress;
	EEDR = ucData;
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
}
unsigned char EEPROM_read(unsigned int uiAddress){
	while(EECR & (1<<EEPE));
	EEAR = uiAddress;
	EECR |= (1<<EERE);
	return EEDR;
}

typedef struct object {
	signed char position[2];
	unsigned char area[2];
	unsigned char shape[8];
	signed short health;
} object;

enum Restart {Restart_Start, Restart_do};
int TickFct_Restart(int state);
enum Clear {Clear_Start};
int TickFct_Clear(int state);
enum Kill {Kill_Start};
int TickFct_Kill(int state);
enum Show {Show_Start, You_Win, You_Died};
int TickFct_Show(int state);
enum Player_control_left {PCL_Start, PC_left};
int TickFct_PC_L(int state);
enum Player_control_right {PCR_Start, PC_right};
int TickFct_PC_R(int state);
enum Player_control_up {PCU_Start, PC_up};
int TickFct_PC_U(int state);
enum Player_control_down {PCD_Start, PC_down};
int TickFct_PC_D(int state);
enum Bullet {fire_Start, fire_bullet};
int TickFct_fire(int state);
enum enemies_1_generate {E1G_Start};
int TickFct_E1G(int state); //random generate enemies
enum enemies_2_generate {E2G_Start};
int TickFct_E2G(int state);
enum enemies_3_generate {E3G_Start};
int TickFct_E3G(int state);
enum enemies_1_move {E1M_Start};
int TickFct_E1M(int state);
enum enemies_2_move {E2M_Start};
int TickFct_E2M(int state);
enum enemies_3_move {E3M_Start};
int TickFct_E3M(int state);
enum enemies_4_move {E4M_Start};
int TickFct_E4M(int state);
enum attack_state {A_Start};
int TickFct_Attack(int state);
enum Gotshot_state {GS_Start};
int TickFct_Gotshot(int state);
enum Contact {Contact_Start};
int TickFct_Contact(int state);
enum Boss {Boss_Start, Boss_stop};
int TickFct_Boss(int state);
enum BossMove {BM_right, BM_left};
int TickFct_BM(int state);
enum ChangeWeapon {CW_Start, CW_bullet, CW_bomb};
int TickFct_CW(int state);
enum BombMove {BombM_Start};
int TickFct_BombM(int state);
void display_highest_score(unsigned long score);
/////////////////////////////////////////// timer
unsigned long alltime = 0;
unsigned long stage_1 = 0;
unsigned long stage_2 = 750;
unsigned long stage_3 = 1500;
unsigned long stage_4 = 2500; // boss
/////////////////////////////////////////// LCD display 
signed char player_1_life_last = 3;
signed char player_1_life = 3;
unsigned char life_str[3];
unsigned char health_last = 99;
unsigned char health_str[3];
unsigned long score = 0;
unsigned long score_highest = 0;
unsigned long score_last = 0;
unsigned long score_str[4];
unsigned char bomb_num = 3;
unsigned char bomb_num_last = 3;
unsigned char bomb_num_str[3];
//////////////////////////////////////// Period
const unsigned char BulletsSpeed = 2; // bullets flying speed
const unsigned char AttackFrequnce = 10;
const unsigned char AttackSpeed = 10;
const unsigned short Enemies_1Frequnce = 200;
const unsigned short Enemies_2Frequnce = 500;
const unsigned short Enemies_3Frequnce = 500;
const unsigned short Enemies_1Speed = 30;
const unsigned short Enemies_2Speed = 60;
const unsigned short Enemies_3Speed = 100;
const unsigned short Enemies_4Speed = 13;
const unsigned short Boss_Speed = 30;
//////////////////////////////////////// Number
const unsigned char playersNum = 1; // number of players
const unsigned char MaxBullets = 20; // maximum number of bullets in the screen
const unsigned char MaxBombs = 20; // maximum number of bullets in the screen
const unsigned char MaxAttacks = 20;// maximum number of attacks in the screen
const unsigned char MaxEnemies_1 = 5; // maximum number of enemies in the screen
const unsigned char MaxEnemies_2 = 2; // maximum number of enemies in the screen
const unsigned char MaxEnemies_3 = 2; // maximum number of enemies in the screen
const unsigned char MaxEnemies_4 = 8; // maximum number of enemies in the screen
const unsigned char MaxFeedback = 30;
const unsigned char Maxshield = 2;
//////////////////////////////////////// Score
const unsigned char enemies_1_score = 3;
const unsigned char enemies_2_score = 10;
const unsigned char enemies_3_score = 10;
const unsigned char enemies_4_score = 5;
//////////////////////////////////////// Damage
const unsigned char BulletDamage = 1;
const unsigned char BombDamage = 20;
const unsigned char Attack_123_Damage = 11;
////////////////////////////////////////
unsigned char Attacks_1Num = 0; 
unsigned char Attacks_2Num = 0;
unsigned char Attacks_3Num = 0;
unsigned char BulletsNum = 0;
unsigned char BombsNum = 0;
unsigned char enemies_1Num = 0;
unsigned char enemies_2Num = 0;
unsigned char enemies_3Num = 0;
unsigned char enemies_4Num = 0;
unsigned char feedbackNum = 0;
unsigned char shieldNum = 0;
unsigned char boss_hp = 0;
object players[1];
object enemies_1[20]; // maximum number of enemies_1
object enemies_2[5];
object enemies_3[5];
object enemies_4[20];
object boss[6];
object attacks_1[20]; // maximum number of attacks_1
object attacks_2[20];
object attacks_3[20];
object BOSS_HP[2];
unsigned char attack_1_slope_timer = 0;
unsigned char enemies_4_rate_timer = 0;
unsigned char attack_1_rate_timer = 0;
unsigned char attack_2_rate_timer = 0;
unsigned char attack_3_rate_timer = 0;
object bullets[20]; // maximum number of bullets
object bombs[20];
unsigned char WeaponFlag = 0;
object feedback[30];
object shield[2];
int main(void) {
	DDRB = 0x08; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	SPI_MasterInit();
	MAX7219_init();
	PWM_on();
	LCD_init();
	LCD_ClearScreen();
	LCD_DisplayString(0, "HP");
	itoa(health_last, health_str, 10);
	LCD_DisplayString(2, health_str);
	LCD_DisplayString(4, "Life");
	itoa(player_1_life, life_str, 10);
	LCD_DisplayString(8, life_str);
	LCD_DisplayString(10, "BULLET");
	LCD_DisplayString(16, "Bomb");
	itoa(bomb_num, bomb_num_str, 10);
	LCD_DisplayString(20, bomb_num_str);
	LCD_DisplayString(22, "Score");
	itoa(score, score_str, 10);
	LCD_DisplayString(27, score_str);
	LCD_DisplayString(32, "0");
	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	// player object
	int i = 0;
	players[i].position[0] = 6;
	players[i].position[1] = 28;
	players[i].area[0] = 3;
	players[i].area[1] = 4;
	players[i].shape[0] = 0b00001100;
	players[i].shape[1] = 0b00000111;
	players[i].shape[2] = 0b00001100;
	players[i].health = health_last;

	i = 0;
	tasks[i].state = Clear_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Clear;
	i++;//
	tasks[i].state = A_Start;
	tasks[i].period = AttackFrequnce;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Attack;
	i++;
	tasks[i].state = GS_Start;
	tasks[i].period = AttackSpeed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Gotshot;//
	i++;
	tasks[i].state = Contact_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Contact;//
	i++;
	tasks[i].state = BombM_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BombM;
	i++;
	tasks[i].state = Kill_Start;
	tasks[i].period = BulletsSpeed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Kill;
	i++;
	tasks[i].state = Show_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Show;
	i++;
	tasks[i].state = PCL_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_PC_L;
	i++;
	tasks[i].state = PCR_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_PC_R;
	i++;
	tasks[i].state = PCU_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_PC_U;
	i++;
	tasks[i].state = PCD_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_PC_D;
	i++;
	tasks[i].state = fire_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_fire;
	i++;
	tasks[i].state = CW_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_CW;
	i++;
	tasks[i].state = E1G_Start;
	tasks[i].period = Enemies_1Frequnce;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_E1G;
	i++;
	tasks[i].state = E2G_Start;
	tasks[i].period = Enemies_2Frequnce;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_E2G;
	i++;
	tasks[i].state = E3G_Start;
	tasks[i].period = Enemies_3Frequnce;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_E3G;
	i++;
	tasks[i].state = E1M_Start;
	tasks[i].period = Enemies_1Speed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_E1M;
	i++;
	tasks[i].state = E2M_Start;
	tasks[i].period = Enemies_2Speed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_E2M;
	i++;
	tasks[i].state = E3M_Start;
	tasks[i].period = Enemies_3Speed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_E3M;
	i++;
	tasks[i].state = E4M_Start;
	tasks[i].period = Enemies_4Speed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_E4M;
	i++;
	tasks[i].state = Boss_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Boss;
	i++;
	tasks[i].state = BM_right;
	tasks[i].period = Boss_Speed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BM;
	i++;
	tasks[i].state = Restart_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Restart;
	i++;
	while(1){}
}
int TickFct_CW(int state){
	switch(state){
		case CW_Start:
		if (!GetBit(PINA, 5)) {
			if (WeaponFlag == 0) state = CW_bullet;
			else if (WeaponFlag == 1) state = CW_bomb;
		}
		break;
		case CW_bullet:
		if (GetBit(PINA, 5)) {
			LCD_DisplayString(10, "BOMB  ");
			LCD_DisplayString(32, "0");
			WeaponFlag = 1;
			state = CW_Start;
		}
		break;
		case CW_bomb:
		if (GetBit(PINA, 5)) {
			LCD_DisplayString(10, "BULLET");
			LCD_DisplayString(32, "0");
			WeaponFlag = 0;
			state = CW_Start;
		}
		break;
	}
	return state;
}
int TickFct_BM(int state){
	switch(state){
		case BM_right:
		if (alltime > stage_4){
			for (int i = 0; i <= 6 - 1; i++){
				if (boss[i].health > 0){
					boss[i].position[0] += 1;
				}
			}
			state = BM_left;break;
		}
		case BM_left:
		if (alltime > stage_4){
			for (int i = 0; i <= 6 - 1; i++){
				if (boss[i].health > 0){
					boss[i].position[0] -= 1;
				}
			}
		}
		state = BM_right;break;
	}
	return state;
}
int TickFct_Boss(int state){
	switch(state){
		case Boss_Start:
		if (alltime > stage_4){
			boss[0].position[0] = 1;
			boss[0].position[1] = 13;
			boss[0].area[0] = 5;
			boss[0].area[1] = 8;
			boss[0].shape[0] = 0b00000111;
			boss[0].shape[1] = 0b00011000;
			boss[0].shape[2] = 0b00010000;
			boss[0].shape[3] = 0b00010000;
			boss[0].shape[4] = 0b11110000;
			boss[0].health = 50;
	
			boss[1].position[0] = 6;
			boss[1].position[1] = 13;
			boss[1].area[0] = 3;
			boss[1].area[1] = 4;
			boss[1].shape[0] = 0b00000011;
			boss[1].shape[1] = 0b00001110;
			boss[1].shape[2] = 0b00000011;
			boss[1].health = 50;
		
			boss[2].position[0] = 9;
			boss[2].position[1] = 13;
			boss[2].area[0] = 5;
			boss[2].area[1] = 8;
			boss[2].shape[0] = 0b11110000;
			boss[2].shape[1] = 0b00010000;
			boss[2].shape[2] = 0b00010000;
			boss[2].shape[3] = 0b00011000;
			boss[2].shape[4] = 0b00000111;
			boss[2].health = 50;
		
			boss[3].position[0] = 2;
			boss[3].position[1] = 9;
			boss[3].area[0] = 8;
			boss[3].area[1] = 7;
			boss[3].shape[0] = 0b00111000;
			boss[3].shape[1] = 0b01001110;
			boss[3].shape[2] = 0b01001111;
			boss[3].shape[3] = 0b01100000;
			boss[3].shape[4] = 0b01100000;
			boss[3].shape[5] = 0b00000000;
			boss[3].shape[6] = 0b00100000;
			boss[3].shape[7] = 0b01100000;
			boss[3].health = 50;
		
			boss[4].position[0] = 7;
			boss[4].position[1] = 9;
			boss[4].area[0] = 6;
			boss[4].area[1] = 7;
			boss[4].shape[0] = 0b01100000;
			boss[4].shape[1] = 0b01000000;
			boss[4].shape[2] = 0b00000000;
			boss[4].shape[3] = 0b01001111;
			boss[4].shape[4] = 0b01001110;
			boss[4].shape[5] = 0b00111000;
			boss[4].health = 50;
		
			boss[5].position[0] = 5;
			boss[5].position[1] = 7;
			boss[5].area[0] = 5;
			boss[5].area[1] = 5;
			boss[5].shape[0] = 0b00000111;
			boss[5].shape[1] = 0b00000111;
			boss[5].shape[2] = 0b00011111;
			boss[5].shape[3] = 0b00000111;
			boss[5].shape[4] = 0b00000111;
			boss[5].health = 5;

			BOSS_HP[0].position[0] = 8;
			BOSS_HP[0].position[1] = 0;
			BOSS_HP[0].area[0] = 8;
			BOSS_HP[0].area[1] = 1;
			BOSS_HP[0].shape[0] = 0b00000001;
			BOSS_HP[0].shape[1] = 0b00000001;
			BOSS_HP[0].shape[2] = 0b00000001;
			BOSS_HP[0].shape[3] = 0b00000001;
			BOSS_HP[0].shape[4] = 0b00000001;
			BOSS_HP[0].shape[5] = 0b00000001;
			BOSS_HP[0].shape[6] = 0b00000001;
			BOSS_HP[0].shape[7] = 0b00000001;
			BOSS_HP[0].health = 1;
			
			BOSS_HP[1].position[0] = 0;
			BOSS_HP[1].position[1] = 0;
			BOSS_HP[1].area[0] = 8;
			BOSS_HP[1].area[1] = 1;
			BOSS_HP[1].shape[0] = 0b00000001;
			BOSS_HP[1].shape[1] = 0b00000001;
			BOSS_HP[1].shape[2] = 0b00000001;
			BOSS_HP[1].shape[3] = 0b00000001;
			BOSS_HP[1].shape[4] = 0b00000001;
			BOSS_HP[1].shape[5] = 0b00000001;
			BOSS_HP[1].shape[6] = 0b00000001;
			BOSS_HP[1].shape[7] = 0b00000001;
			BOSS_HP[1].health = 1;
			
			state = Boss_stop;break;
		}
		case Boss_stop: break;
	}
	return state;
}
int TickFct_Contact(int state){
	switch(state){
		case Contact_Start:
		for (int i = 0; i <= MaxEnemies_1 - 1; i++){
			if (enemies_1[i].health > 0){	
				if (enemies_1[i].position[1]==players[0].position[1]-1&&abs(enemies_1[i].position[0]-(players[0].position[0]+1))<=1){
					enemies_1[i].health = 0;
					players[0].health = 0;
				}
			}
		}
		for (int i = 0; i <= MaxEnemies_2 - 1; i++){
			if (enemies_2[i].health > 0){
				if (abs(enemies_2[i].position[1]-2-(players[0].position[1]-1))<=1&&\
					abs(enemies_2[i].position[0]+2-(players[0].position[0]+1))<=2){
					enemies_2[i].health = 0;
					players[0].health = 0;
				}
			}
		}
		for (int i = 0; i <= MaxEnemies_3 - 1; i++){
			if (enemies_3[i].health > 0){
				if (abs(enemies_3[i].position[1]-3-(players[0].position[1]-1))<=3&&\
					abs(enemies_3[i].position[0]+1-(players[0].position[0]+1))<=1){
					enemies_3[i].health = 0;
					players[0].health = 0;
				}
			}
		}
		for (int i = 0; i <= MaxEnemies_4 - 1; i++){
			if (enemies_4[i].health > 0){
				if (abs(enemies_4[i].position[1]-1-(players[0].position[1]-1))<=1&&\
					abs(enemies_4[i].position[0]+1-(players[0].position[0]+1))<=1){
					enemies_4[i].health = 0;
					players[0].health = 0;
				}
			}
		}
		break;
	}
	return state;
}

int TickFct_E1M(int state){
	switch(state){
		case E1M_Start:
		for (int i = 0; i <= MaxEnemies_1 - 1; i++){
			if (enemies_1[i].health > 0){
				if (enemies_1[i].position[1] <= 31){
					enemies_1[i].position[1] += rand()%2;
					enemies_1[i].position[0] += rand()%3-1; //012 - 1 = -101
					if (enemies_1[i].position[0] > 15) enemies_1[i].position[0] = 0;
					if (enemies_1[i].position[0] < 0) enemies_1[i].position[0] = 15;
				}
				else enemies_1[i].health = 0;
			}
		}
		break;
	}
	return state;
}

int TickFct_E2M(int state){
	switch(state){
		case E2M_Start:
		for (int i = 0; i <= MaxEnemies_2 - 1; i++){
			if (enemies_2[i].health > 0){
				if (enemies_2[i].position[1] <= 31){
					enemies_2[i].position[1] += rand()%2;
					enemies_2[i].position[0] += rand()%3-1; //012 - 1 = -101
					if (enemies_2[i].position[0] > 15) enemies_2[i].position[0] = 0;
					if (enemies_2[i].position[0] < 0) enemies_2[i].position[0] = 15;
				}
				else enemies_2[i].health = 0;
			}
		}
		break;
	}
	return state;
}
int TickFct_E3M(int state){
	switch(state){
		case E3M_Start:
		for (int i = 0; i <= MaxEnemies_3 - 1; i++){
			if (enemies_3[i].health > 0){
				if (enemies_3[i].position[1] <= 14){
					enemies_3[i].position[1] += rand()%2;
				}
				enemies_3[i].position[0] += rand()%3-1; //012 - 1 = -101
				if (enemies_3[i].position[0] > 15) enemies_3[i].position[0] = 0;
				if (enemies_3[i].position[0] < 0) enemies_3[i].position[0] = 15;
			}
		}
		break;
	}
	return state;
}
int TickFct_E4M(int state){
	switch(state){
		case E4M_Start:
		for (int i = 0; i <= MaxEnemies_4 - 1; i++){
			if (enemies_4[i].health > 0){
				if (enemies_4[i].position[1] < players[0].position[1]){
					enemies_4[i].position[1] += 1;
					enemies_4[i].area[0] = 3;
					enemies_4[i].area[1] = 2;
					enemies_4[i].shape[0] = 0b00000001;
					enemies_4[i].shape[1] = 0b00000011;
					enemies_4[i].shape[2] = 0b00000001;
				}
				else if (enemies_4[i].position[1] > players[0].position[1]){
					enemies_4[i].position[1] -= 1;
					enemies_4[i].area[0] = 3;
					enemies_4[i].area[1] = 2;
					enemies_4[i].shape[0] = 0b00000010;
					enemies_4[i].shape[1] = 0b00000011;
					enemies_4[i].shape[2] = 0b00000010;
				}
				else if (enemies_4[i].position[0] > players[0].position[0]){
					enemies_4[i].position[0] -= 1;
					enemies_4[i].area[0] = 2;
					enemies_4[i].area[1] = 3;
					enemies_4[i].shape[0] = 0b00000010;
					enemies_4[i].shape[1] = 0b00000111;
					enemies_4[i].shape[2] = 0;
					if (enemies_4[i].position[0] < 0) enemies_2[i].position[0] = 15;
				}
				else if (enemies_4[i].position[0] < players[0].position[0]){
					enemies_4[i].position[0] += 1;
					enemies_4[i].area[0] = 2;
					enemies_4[i].area[1] = 3;
					enemies_4[i].shape[0] = 0b00000111;
					enemies_4[i].shape[1] = 0b00000010;
					enemies_4[i].shape[2] = 0;
					if (enemies_4[i].position[0] > 15) enemies_2[i].position[0] = 0;
				}
			}
		}
		break;
	}
	return state;
}
int TickFct_E3G(int state){
	switch(state){
		case E3G_Start:
		if (alltime >= stage_3 && alltime < stage_4){
			enemies_3_temp = 1;
			for (int i = 0; i <= MaxEnemies_3 - 1; i++){
				if (enemies_3[i].health > 0) enemies_3_temp++;
			}
			if (enemies_3_temp <= 1){
				enemies_3[enemies_3Num].position[0] = (rand()%15);
				enemies_3[enemies_3Num].position[1] = (rand()%4+6);
				enemies_3[enemies_3Num].area[0] = 3;
				enemies_3[enemies_3Num].area[1] = 7;
				enemies_3[enemies_3Num].shape[0] = 0b01111101;
				enemies_3[enemies_3Num].shape[1] = 0b00000111;
				enemies_3[enemies_3Num].shape[2] = 0b01111101;
				enemies_3[enemies_3Num].health = 10;
				if (enemies_3Num < MaxEnemies_3 - 1) enemies_3Num++;
				else enemies_3Num = 0;
			}
		}
	}
	return state;
}

int TickFct_E2G(int state){
	switch(state){
		case E2G_Start:
		if (alltime >= stage_2 && alltime < stage_3){
			enemies_2_temp = 1;
			for (int i = 0; i <= MaxEnemies_2 - 1; i++){
				if (enemies_2[i].health > 0) enemies_2_temp++;
			}
			if (enemies_2_temp <= 1){
				enemies_2[enemies_2Num].position[0] = (rand()%15);
				enemies_2[enemies_2Num].position[1] = (rand()%4+3);
				enemies_2[enemies_2Num].area[0] = 5;
				enemies_2[enemies_2Num].area[1] = 5;
				enemies_2[enemies_2Num].shape[0] = 0b00000110;
				enemies_2[enemies_2Num].shape[1] = 0b00000011;
				enemies_2[enemies_2Num].shape[2] = 0b00011110;
				enemies_2[enemies_2Num].shape[3] = 0b00000011;
				enemies_2[enemies_2Num].shape[4] = 0b00000110;
				enemies_2[enemies_2Num].health = 10;
				if (enemies_2Num < MaxEnemies_2 - 1) enemies_2Num++;
				else enemies_2Num = 0;
			}
		}
	}
	return state;
}
int TickFct_E1G(int state){
	switch(state){
		case E1G_Start:
		if (alltime < stage_4)
		{
			enemies_1_temp = 1;
			for (int i = 0; i <= MaxEnemies_1 - 1; i++){
				if (enemies_1[i].health > 0) enemies_1_temp++;
			}
			if (enemies_1_temp <= 3){
				enemies_1[enemies_1Num].position[0] = (rand()%15);
				enemies_1[enemies_1Num].position[1] = (rand()%3+1);
				enemies_1[enemies_1Num].area[0] = 3;
				enemies_1[enemies_1Num].area[1] = 3;
				enemies_1[enemies_1Num].shape[0] = 0b00000011;
				enemies_1[enemies_1Num].shape[1] = 0b00000110;
				enemies_1[enemies_1Num].shape[2] = 0b00000011;
				enemies_1[enemies_1Num].health = 3;
				if (enemies_1Num < MaxEnemies_1 - 1) enemies_1Num++;
				else enemies_1Num = 0;
			}
			break;
		}
	}
	return state;
}
int TickFct_Gotshot(int state){
	switch(state){
		case GS_Start:
		for (int i = 0; i <= MaxAttacks - 1; i++){
			if (attacks_1[i].position[1] <= 31){
				if (attacks_1[i].health > 0){
					for (int j = 0; j <= Maxshield - 1; j++){
						if (shield[j].health > 0){
							if ((attacks_1[i].position[1]==shield[j].position[1]&&abs(attacks_1[i].position[0]-(shield[j].position[0]+2))<=1)||\
							((attacks_1[i].position[1]==shield[j].position[1]-1)&&abs(attacks_1[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_1[i].position[1]==shield[j].position[1]-2)&&abs(attacks_1[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_1[i].position[1]==shield[j].position[1]-3)&&abs(attacks_1[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_1[i].position[1]==shield[j].position[1]-4)&&abs(attacks_1[i].position[0]-(shield[j].position[0]+2))<=2)||\
							(attacks_1[i].position[1]==shield[j].position[1]-5&&abs(attacks_1[i].position[0]-(shield[j].position[0]+2))<=1)){
								attacks_1[i].health -= 1;
							}
						}
					}
				}
				if (attacks_1[i].health > 0) {
					if (players[0].health > 0){
						if ((attacks_1[i].position[1]==players[0].position[1]&&(attacks_1[i].position[0]==players[0].position[0]||\
						attacks_1[i].position[0]==players[0].position[0]+2))||(attacks_1[i].position[1]==players[0].position[1]-1&&\
						abs(attacks_1[i].position[0]-(players[0].position[0]+1))<=1)||(attacks_1[i].position[1]==players[0].position[1]-2&&\
						attacks_1[i].position[0]==players[0].position[0]+1)||(attacks_1[i].position[1]==players[0].position[1]-3&&\
						attacks_1[i].position[0]==players[0].position[0]+1)){
							players[0].health -= Attack_123_Damage; // attack damage
							attacks_1[i].health -= 1;
							feedback[feedbackNum].position[0] = attacks_1[i].position[0]-1;
							feedback[feedbackNum].position[1] = attacks_1[i].position[1];
							feedback[feedbackNum].area[0] = 2;
							feedback[feedbackNum].area[1] = 2;
							feedback[feedbackNum].shape[0] = 0b00000011;
							feedback[feedbackNum].shape[1] = 0b00000011;
							feedback[feedbackNum].health = 5;
							if (feedbackNum < MaxFeedback - 1) feedbackNum++;
							else feedbackNum = 0;
						}
					}
				}
				if (attacks_1[i].health > 0) {
					if ((-i+attack_1_slope_timer)%4){///
						attacks_1[i].position[1] += 1;
					}
					else {
						if (attacks_1[i].position[0] >= 15) attacks_1[i].health = 0;
						else attacks_1[i].position[0] += 1;
					}
				}
			}
			else attacks_1[i].health = 0;
			if (attacks_2[i].position[1] <= 31) {
				if (attacks_2[i].health > 0){
					for (int j = 0; j <= Maxshield - 1; j++){
						if (shield[j].health > 0){
							if ((attacks_2[i].position[1]==shield[j].position[1]&&abs(attacks_2[i].position[0]-(shield[j].position[0]+2))<=1)||\
							((attacks_2[i].position[1]==shield[j].position[1]-1)&&abs(attacks_2[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_2[i].position[1]==shield[j].position[1]-2)&&abs(attacks_2[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_2[i].position[1]==shield[j].position[1]-3)&&abs(attacks_2[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_2[i].position[1]==shield[j].position[1]-4)&&abs(attacks_2[i].position[0]-(shield[j].position[0]+2))<=2)||\
							(attacks_2[i].position[1]==shield[j].position[1]-5&&abs(attacks_2[i].position[0]-(shield[j].position[0]+2))<=1)){
								attacks_2[i].health -= 1;
							}
						}
					}
				}
				if (attacks_2[i].health > 0) {
					if (players[0].health > 0){
						if ((attacks_2[i].position[1]==players[0].position[1]&&(attacks_2[i].position[0]==players[0].position[0]||\
						attacks_2[i].position[0]==players[0].position[0]+2))||(attacks_2[i].position[1]==players[0].position[1]-1&&\
						abs(attacks_2[i].position[0]-(players[0].position[0]+1))<=1)||(attacks_2[i].position[1]==players[0].position[1]-2&&\
						attacks_2[i].position[0]==players[0].position[0]+1)||(attacks_2[i].position[1]==players[0].position[1]-3&&\
						attacks_2[i].position[0]==players[0].position[0]+1)){
							players[0].health -= Attack_123_Damage; // attack damage
							attacks_2[i].health -= 1;
							feedback[feedbackNum].position[0] = attacks_2[i].position[0]-1;
							feedback[feedbackNum].position[1] = attacks_2[i].position[1];
							feedback[feedbackNum].area[0] = 3;
							feedback[feedbackNum].area[1] = 2;
							feedback[feedbackNum].shape[0] = 0b00000010;
							feedback[feedbackNum].shape[1] = 0b00000001;
							feedback[feedbackNum].shape[2] = 0b00000010;
							feedback[feedbackNum].health = 5;
							if (feedbackNum < MaxFeedback - 1) feedbackNum++;
							else feedbackNum = 0;
						}
					}
				}
				if (attacks_2[i].health > 0) {
						attacks_2[i].position[1] += 1;
				}
			}
			else attacks_2[i].health = 0;
			if (attacks_3[i].position[1] <= 31) {
				if (attacks_3[i].health > 0){
					for (int j = 0; j <= Maxshield - 1; j++){
						if (shield[j].health > 0){
							if ((attacks_3[i].position[1]==shield[j].position[1]&&abs(attacks_3[i].position[0]-(shield[j].position[0]+2))<=1)||\
							((attacks_3[i].position[1]==shield[j].position[1]-1)&&abs(attacks_3[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_3[i].position[1]==shield[j].position[1]-2)&&abs(attacks_3[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_3[i].position[1]==shield[j].position[1]-3)&&abs(attacks_3[i].position[0]-(shield[j].position[0]+2))<=2)||\
							((attacks_3[i].position[1]==shield[j].position[1]-4)&&abs(attacks_3[i].position[0]-(shield[j].position[0]+2))<=2)||\
							(attacks_3[i].position[1]==shield[j].position[1]-5&&abs(attacks_3[i].position[0]-(shield[j].position[0]+2))<=1)){
								attacks_3[i].health -= 1;
							}
						}
					}
				}
				if (attacks_3[i].health > 0) {
					if (players[0].health > 0){
						if ((attacks_3[i].position[1]==players[0].position[1]&&(attacks_3[i].position[0]==players[0].position[0]||\
						attacks_3[i].position[0]==players[0].position[0]+2))||(attacks_3[i].position[1]==players[0].position[1]-1&&\
						abs(attacks_3[i].position[0]-(players[0].position[0]+1))<=1)||(attacks_3[i].position[1]==players[0].position[1]-2&&\
						attacks_3[i].position[0]==players[0].position[0]+1)||(attacks_3[i].position[1]==players[0].position[1]-3&&\
						attacks_3[i].position[0]==players[0].position[0]+1)){
							players[0].health -= Attack_123_Damage; // attack damage
							attacks_3[i].health -= 1;
							feedback[feedbackNum].position[0] = attacks_3[i].position[0];
							feedback[feedbackNum].position[1] = attacks_3[i].position[1];
							feedback[feedbackNum].area[0] = 2;
							feedback[feedbackNum].area[1] = 2;
							feedback[feedbackNum].shape[0] = 0b00000011;
							feedback[feedbackNum].shape[1] = 0b00000011;
							feedback[feedbackNum].health = 5;
							if (feedbackNum < MaxFeedback - 1) feedbackNum++;
							else feedbackNum = 0;
						}
					}
				}
				if (attacks_3[i].health > 0) {
					if ((-i+attack_1_slope_timer)%4){//
						attacks_3[i].position[1] += 1;
					}
					else {
						if (attacks_3[i].position[0] <= 0) attacks_3[i].health = 0;
						else attacks_3[i].position[0] -= 1;
					}
				}
			}
			else attacks_3[i].health = 0;
		}
		if (attack_1_slope_timer < 4)attack_1_slope_timer++;
		else attack_1_slope_timer = 1;
		break;
	}
	return state;
}
int TickFct_Attack(int state) {
	switch(state){
		case A_Start:// generate attacks
		for (int i = 0; i <= MaxEnemies_1 - 1; i++){
			if (enemies_1[i].health > 0){
				signed char position_diff = ((enemies_1[i].position[0]+\
				enemies_1[i].area[0]/2)-players[0].position[0]-players[0].area[0]/2);
				if (position_diff <= -2) {
					if (attack_1_rate_timer%7 == 0){ // interval between two attack is 8
						attacks_1[Attacks_1Num].position[0] = enemies_1[i].position[0] + enemies_1[i].area[0]/2;
						attacks_1[Attacks_1Num].position[1] = enemies_1[i].position[1];
						attacks_1[Attacks_1Num].area[0] = 1;
						attacks_1[Attacks_1Num].area[1] = 1;
						attacks_1[Attacks_1Num].shape[0] = 0b00000001;
						attacks_1[Attacks_1Num].health = 1;
					if (Attacks_1Num < MaxAttacks - 1) Attacks_1Num++;
					else Attacks_1Num = 0;
					}
					if (attack_1_rate_timer < 7)attack_1_rate_timer++;
					else attack_1_rate_timer = 1;
				}
				else if (position_diff <= 2) {
					if (attack_2_rate_timer%5 == 0){ // interval between two attack is 5
						attacks_2[Attacks_2Num].position[0] = enemies_1[i].position[0] + enemies_1[i].area[0]/2;
						attacks_2[Attacks_2Num].position[1] = enemies_1[i].position[1];
						attacks_2[Attacks_2Num].area[0] = 1;
						attacks_2[Attacks_2Num].area[1] = 1;
						attacks_2[Attacks_2Num].shape[0] = 0b00000001;
						attacks_2[Attacks_2Num].health = 1;
						if (Attacks_2Num < MaxAttacks - 1) Attacks_2Num++;
						else Attacks_2Num = 0;
					}
					if (attack_2_rate_timer < 5)attack_2_rate_timer++;
					else attack_2_rate_timer = 1;
				}
				else {
					if (attack_3_rate_timer%7 == 0){ // interval between two attack is 7
						attacks_3[Attacks_3Num].position[0] = enemies_1[i].position[0] + enemies_1[i].area[0]/2;
						attacks_3[Attacks_3Num].position[1] = enemies_1[i].position[1];
						attacks_3[Attacks_3Num].area[0] = 1;
						attacks_3[Attacks_3Num].area[1] = 1;
						attacks_3[Attacks_3Num].shape[0] = 0b00000001;
						attacks_3[Attacks_3Num].health = 1;
						if (Attacks_3Num < MaxAttacks - 1) Attacks_3Num++;
						else Attacks_3Num = 0;
					}
					if (attack_3_rate_timer < 7)attack_3_rate_timer++;
					else attack_3_rate_timer = 1;
				}
			}
		}
		for (int i = 0; i <= MaxEnemies_2 - 1; i++){
			if (enemies_2[i].health > 0){
				if (attack_1_rate_timer%7 == 0){ // interval between two attack is 8
					attacks_1[Attacks_1Num].position[0] = enemies_2[i].position[0] + enemies_2[i].area[0]/2;
					attacks_1[Attacks_1Num].position[1] = enemies_2[i].position[1];
					attacks_1[Attacks_1Num].area[0] = 1;
					attacks_1[Attacks_1Num].area[1] = 1;
					attacks_1[Attacks_1Num].shape[0] = 0b00000001;
					attacks_1[Attacks_1Num].health = 1;
					if (Attacks_1Num < MaxAttacks - 1) Attacks_1Num++;
					else Attacks_1Num = 0;
				}
				if (attack_1_rate_timer < 7)attack_1_rate_timer++;
				else attack_1_rate_timer = 1;
				if (attack_2_rate_timer%5 == 0){ // interval between two attack is 5
					attacks_2[Attacks_2Num].position[0] = enemies_2[i].position[0] + enemies_2[i].area[0]/2;
					attacks_2[Attacks_2Num].position[1] = enemies_2[i].position[1];
					attacks_2[Attacks_2Num].area[0] = 1;
					attacks_2[Attacks_2Num].area[1] = 1;
					attacks_2[Attacks_2Num].shape[0] = 0b00000001;
					attacks_2[Attacks_2Num].health = 1;
					if (Attacks_2Num < MaxAttacks - 1) Attacks_2Num++;
					else Attacks_2Num = 0;
				}
				if (attack_2_rate_timer < 5)attack_2_rate_timer++;
				else attack_2_rate_timer = 1;
				if (attack_3_rate_timer%7 == 0){ // interval between two attack is 7
					attacks_3[Attacks_3Num].position[0] = enemies_2[i].position[0] + enemies_2[i].area[0]/2;
					attacks_3[Attacks_3Num].position[1] = enemies_2[i].position[1];
					attacks_3[Attacks_3Num].area[0] = 1;
					attacks_3[Attacks_3Num].area[1] = 1;
					attacks_3[Attacks_3Num].shape[0] = 0b00000001;
					attacks_3[Attacks_3Num].health = 1;
					if (Attacks_3Num < MaxAttacks - 1) Attacks_3Num++;
					else Attacks_3Num = 0;
				}
				if (attack_3_rate_timer < 7)attack_3_rate_timer++;
				else attack_3_rate_timer = 1;
			}
		}
		for (int i = 0; i <= MaxEnemies_3 - 1; i++){
			if (enemies_3[i].health > 0){
				if (enemies_4_rate_timer%8 == 0){
					if (alltime >= stage_3 && alltime < stage_4){
						enemies_4_temp = 1;
						for (int j = 0; j <= MaxEnemies_4 - 1; j++){
							if (enemies_4[j].health > 0) enemies_4_temp++;
						}
						if (enemies_4_temp <= 4){
							enemies_4[enemies_4Num].position[0] = enemies_3[i].position[0];
							enemies_4[enemies_4Num].position[1] = enemies_3[i].position[1]-4;
							enemies_4[enemies_4Num].area[0] = 3;
							enemies_4[enemies_4Num].area[1] = 2;
							enemies_4[enemies_4Num].shape[0] = 0b00000001;
							enemies_4[enemies_4Num].shape[1] = 0b00000011;
							enemies_4[enemies_4Num].shape[2] = 0b00000001;
							enemies_4[enemies_4Num].health = 1;
							if (enemies_4Num < MaxEnemies_4 - 1) enemies_4Num++;
							else enemies_4Num = 0;
						}
					}
				}
				if (enemies_4_rate_timer  < 8)enemies_4_rate_timer ++;
				else enemies_4_rate_timer  = 1;
			}
		}
		for (int i = 0; i <= 3 - 1; i++){
			if (boss[i].health > 0){
				if (attack_1_rate_timer%7 == 0){ // interval between two attack is 8
					if (rand()%3 == 0){
						if (i == 0){attacks_1[Attacks_1Num].position[0] = boss[i].position[0] + 4;}
						else if (i == 1){attacks_1[Attacks_1Num].position[0] = boss[i].position[0] + 1;}
						else if (i == 2){attacks_1[Attacks_1Num].position[0] = boss[i].position[0];}
						attacks_1[Attacks_1Num].position[1] = boss[i].position[1];
						attacks_1[Attacks_1Num].area[0] = 1;
						attacks_1[Attacks_1Num].area[1] = 1;
						attacks_1[Attacks_1Num].shape[0] = 0b00000001;
						attacks_1[Attacks_1Num].health = 1;
						if (Attacks_1Num < MaxAttacks - 1) Attacks_1Num++;
						else Attacks_1Num = 0;
					}
				}
				if (attack_1_rate_timer < 7)attack_1_rate_timer++;
				else attack_1_rate_timer = 1;
				if (attack_2_rate_timer%5 == 0){ // interval between two attack is 5
					if (rand()%3 == 0){
						if (i == 0){attacks_2[Attacks_2Num].position[0] = boss[i].position[0] + 4;}
						else if (i == 1){attacks_2[Attacks_2Num].position[0] = boss[i].position[0] + 1;}
						else if (i == 2){attacks_2[Attacks_2Num].position[0] = boss[i].position[0];}
						attacks_2[Attacks_2Num].position[1] = boss[i].position[1];
						attacks_2[Attacks_2Num].area[0] = 1;
						attacks_2[Attacks_2Num].area[1] = 1;
						attacks_2[Attacks_2Num].shape[0] = 0b00000001;
						attacks_2[Attacks_2Num].health = 1;
						if (Attacks_2Num < MaxAttacks - 1) Attacks_2Num++;
						else Attacks_2Num = 0;
					}
				}
				if (attack_2_rate_timer < 5)attack_2_rate_timer++;
				else attack_2_rate_timer = 1;
				if (attack_3_rate_timer%7 == 0){ // interval between two attack is 7
					if (rand()%3 == 0){
						if (i == 0){attacks_3[Attacks_3Num].position[0] = boss[i].position[0] + 4;}
						else if (i == 1){attacks_3[Attacks_3Num].position[0] = boss[i].position[0] + 1;}
						else if (i == 2){attacks_3[Attacks_3Num].position[0] = boss[i].position[0];}
						attacks_3[Attacks_3Num].position[1] = boss[i].position[1];
						attacks_3[Attacks_3Num].area[0] = 1;
						attacks_3[Attacks_3Num].area[1] = 1;
						attacks_3[Attacks_3Num].shape[0] = 0b00000001;
						attacks_3[Attacks_3Num].health = 1;
						if (Attacks_3Num < MaxAttacks - 1) Attacks_3Num++;
						else Attacks_3Num = 0;
					}
				}
				if (attack_3_rate_timer < 7)attack_3_rate_timer++;
				else attack_3_rate_timer = 1;
			}
		}
		if (!boss[0].health){
			if (!boss[1].health){
				if (!boss[2].health){
					if (boss[5].health){
						if (attack_1_rate_timer%5 == 0){ // interval between two attack is 8
							attacks_1[Attacks_1Num].position[0] = boss[5].position[0] + 2;
							attacks_1[Attacks_1Num].position[1] = boss[5].position[1]+1;
							attacks_1[Attacks_1Num].area[0] = 1;
							attacks_1[Attacks_1Num].area[1] = 1;
							attacks_1[Attacks_1Num].shape[0] = 0b00000001;
							attacks_1[Attacks_1Num].health = 1;
							if (Attacks_1Num < MaxAttacks - 1) Attacks_1Num++;
							else Attacks_1Num = 0;
						}
						if (attack_1_rate_timer < 5)attack_1_rate_timer++;
						else attack_1_rate_timer = 1;
						if (attack_2_rate_timer%3 == 0){ // interval between two attack is 5
							attacks_2[Attacks_2Num].position[0] = boss[5].position[0] + 2;
							attacks_2[Attacks_2Num].position[1] = boss[5].position[1]+1;
							attacks_2[Attacks_2Num].area[0] = 1;
							attacks_2[Attacks_2Num].area[1] = 1;
							attacks_2[Attacks_2Num].shape[0] = 0b00000001;
							attacks_2[Attacks_2Num].health = 1;
							if (Attacks_2Num < MaxAttacks - 1) Attacks_2Num++;
							else Attacks_2Num = 0;
						}
						if (attack_2_rate_timer < 3)attack_2_rate_timer++;
						else attack_2_rate_timer = 1;
						if (attack_3_rate_timer%5 == 0){ // interval between two attack is 7
							attacks_3[Attacks_3Num].position[0] = boss[5].position[0] + 2;
							attacks_3[Attacks_3Num].position[1] = boss[5].position[1]+1;
							attacks_3[Attacks_3Num].area[0] = 1;
							attacks_3[Attacks_3Num].area[1] = 1;
							attacks_3[Attacks_3Num].shape[0] = 0b00000001;
							attacks_3[Attacks_3Num].health = 1;
							if (Attacks_3Num < MaxAttacks - 1) Attacks_3Num++;
							else Attacks_3Num = 0;
						}
						if (attack_3_rate_timer < 5)attack_3_rate_timer++;
						else attack_3_rate_timer = 1;
					}
				}
			}
		}
	}
	return state;
}
int TickFct_Clear(int state) {
	switch(state){
		case Clear_Start:
		for (int i = 1; i <= 8; i++) {
		LED_Matrix_0[i-1] = 0;
		LED_Matrix_1[i-1] = 0;
		LED_Matrix_2[i-1] = 0;
		LED_Matrix_3[i-1] = 0;
		LED_Matrix_4[i-1] = 0;
		LED_Matrix_5[i-1] = 0;
		LED_Matrix_6[i-1] = 0;
		LED_Matrix_7[i-1] = 0;
		}
		break;
	}
	return state;
}
int TickFct_BombM(int state){
	switch(state){
		case BombM_Start:
		for (int i = 0; i <= MaxBombs - 1; i += 2){
			if (bombs[i].position[1] >= 0){
				for (int j = 0; j <= MaxEnemies_1 - 1; j++){
					if (enemies_1[j].health > 0){
						if (enemies_1[j].position[1] == bombs[i].position[1] || \
							enemies_1[j].position[1] == bombs[i+1].position[1]){
							enemies_1[j].health -= BombDamage;
							if (enemies_1[j].health <= 0){
								score += enemies_1_score;
								itoa(score, score_str, 10);
								//LCD_DisplayString(27, score_str);
								LCD_DisplayString(32, "0");
							}
						}
					}
				}
				for (int j = 0; j <= MaxEnemies_2 - 1; j++){
					if (enemies_2[j].health > 0){
						if (enemies_2[j].position[1] - 1 == bombs[i].position[1] || \
							enemies_2[j].position[1] - 1 == bombs[i+1].position[1]){
							enemies_2[j].health -= BombDamage;
							if (enemies_2[j].health <= 0){
								score += enemies_2_score;
								itoa(score, score_str, 10);
								//LCD_DisplayString(27, score_str);
								LCD_DisplayString(32, "0");
							}
						}
					}
				}
				for (int j = 0; j <= MaxEnemies_3 - 1; j++){
					if (enemies_3[j].health > 0){
						if (enemies_3[j].position[1] - 1 == bombs[i].position[1] || \
							enemies_3[j].position[1] - 1 == bombs[i+1].position[1]){
							enemies_3[j].health -= BombDamage;
							if (enemies_3[j].health <= 0){
								score += enemies_3_score;
								itoa(score, score_str, 10);
								//LCD_DisplayString(27, score_str);
								LCD_DisplayString(32, "0");
							}
						}
					}
				}
				for (int j = 0; j <= MaxEnemies_4 - 1; j++){
					if (enemies_4[j].health > 0){
						if (enemies_4[j].position[1] == bombs[i].position[1] || \
							enemies_4[j].position[1] == bombs[i+1].position[1]){
							enemies_4[j].health -= BombDamage;
							if (enemies_4[j].health <= 0){
								score += enemies_4_score;
								itoa(score, score_str, 10);
								//LCD_DisplayString(27, score_str);
								LCD_DisplayString(32, "0");
							}
						}
					}
				}
				for (int j = 0; j <= MaxAttacks - 1; j++){
					if (attacks_1[j].health > 0){
						if (attacks_1[j].position[1] == bombs[i].position[1] || attacks_1[j].position[1] == bombs[i].position[1]+1 ||\
							attacks_1[j].position[1] == bombs[i+1].position[1] || attacks_1[j].position[1] == bombs[i+1].position[1]+1){
							attacks_1[j].health -= 1;
						}
					}
					if (attacks_2[j].health > 0){
						if (attacks_2[j].position[1] == bombs[i].position[1] || attacks_2[j].position[1] == bombs[i].position[1]+1 ||\
							attacks_2[j].position[1] == bombs[i+1].position[1] || attacks_2[j].position[1] == bombs[i+1].position[1]+1){
							attacks_2[j].health -= 1;
						}
					}
					if (attacks_3[j].health > 0){
						if (attacks_3[j].position[1] == bombs[i].position[1] || attacks_3[j].position[1] == bombs[i].position[1]+1 ||\
							attacks_3[j].position[1] == bombs[i+1].position[1] || attacks_3[j].position[1] == bombs[i+1].position[1]+1){
							attacks_3[j].health -= 1;
						}
					}
				}
				bombs[i].position[1] -= 1;
				bombs[i+1].position[1] -= 1;
			}
			else {
				bombs[i].health = 0;
				bombs[i+1].health = 0;
			}
		}
	}
	return state;
}
int TickFct_Kill(int state) {
	switch(state){
		case Kill_Start:
		// compute if every bullet has contacted with every enemy
		for (int i = 0; i <= MaxBullets - 1; i++){
			if (bullets[i].position[1] >= 0) {
				for (int j = 0; j <= MaxEnemies_1 - 1; j++) {
					if (bullets[i].health > 0){
						if (enemies_1[j].health > 0){
							if ((bullets[i].position[1] == enemies_1[j].position[1]) && \
							(abs(bullets[i].position[0] - (enemies_1[j].position[0] + \
							enemies_1[j].area[0]/2)) <= enemies_1[j].area[0]/2)) {
								enemies_1[j].health -= BulletDamage;
								bullets[i].health -= 1;
								if (enemies_1[j].health <= 0){
									score += enemies_1_score;
									itoa(score, score_str, 10);
									LCD_DisplayString(27, score_str);
									LCD_DisplayString(32, "0");
								}
							}
						}
					}
				}
				
				for (int j = 0; j <= MaxEnemies_2 - 1; j++) {
					if (bullets[i].health > 0){
						if (enemies_2[j].health > 0){
							if ((bullets[i].position[1] == enemies_2[j].position[1]-1) && \
							(abs(bullets[i].position[0] - (enemies_2[j].position[0]+\
							enemies_2[j].area[0]/2)) <= enemies_2[j].area[0]/2)) {
								enemies_2[j].health -= BulletDamage;
								bullets[i].health -= 1;
								if (enemies_2[j].health <= 0){
									score += enemies_2_score;
									itoa(score, score_str, 10);
									LCD_DisplayString(27, score_str);
									LCD_DisplayString(32, "0");
								}
							}
						}
					}
				}
				for (int j = 0; j <= MaxEnemies_3 - 1; j++) {
					if (bullets[i].health > 0){
						if (enemies_3[j].health > 0){
							if ((bullets[i].position[1] == enemies_3[j].position[1]-2) && \
							(abs(bullets[i].position[0] - (enemies_3[j].position[0]+\
							enemies_3[j].area[0]/2)) <= enemies_3[j].area[0]/2)) {
								enemies_3[j].health -= BulletDamage;
								bullets[i].health -= 1;
								if (enemies_3[j].health <= 0){
									score += enemies_3_score;
									itoa(score, score_str, 10);
									LCD_DisplayString(27, score_str);
									LCD_DisplayString(32, "0");
								}
							}
						}
					}
				}
				for (int j = 0; j <= MaxEnemies_4 - 1; j++) {
					if (bullets[i].health > 0){
						if (enemies_4[j].health > 0){
							if ((bullets[i].position[1] == enemies_4[j].position[1]-1) && \
							(abs(bullets[i].position[0] - (enemies_4[j].position[0]+\
							enemies_4[j].area[0]/2)) <= enemies_4[j].area[0]/2)) {
								enemies_4[j].health -= BulletDamage;
								bullets[i].health -= 1;
								if (enemies_4[j].health <= 0){
									score += enemies_4_score;
									itoa(score, score_str, 10);
									LCD_DisplayString(27, score_str);
									LCD_DisplayString(32, "0");
								}
							}
						}
					}
				}
				if (bullets[i].health > 0){
					if (boss[5].health > 0){
						if ((bullets[i].position[1] == boss[5].position[1]-1) && \
						(abs(bullets[i].position[0] - (boss[5].position[0]+\
						boss[5].area[0]/2)) <= boss[5].area[0]/2)) {
							for (int j = 0; j <= 6-1; j++){
								if (boss[j].health > 0){
									boss[j].health -= BulletDamage;
									bullets[i].health -= 1;
									if (boss[j].health <= 0 && j <= 4){
										score += 30;
										itoa(score, score_str, 10);
										LCD_DisplayString(27, score_str);
										LCD_DisplayString(32, "0");
									}
									else if (boss[j].health <= 0 && j > 4){
										score += 100;
										itoa(score, score_str, 10);
										LCD_DisplayString(27, score_str);
										LCD_DisplayString(32, "0");
									}
									break;
								}
							}
						}
					}
				}
				
				if (bullets[i].health > 0) {
					bullets[i].position[1] -= 1;
				}
			}
			else bullets[i].health = 0;
		}
		break;
	}
	return state;
}

int TickFct_Show(int state) {
	switch(state) {
		case Show_Start:
		for (int i = 0; i <= MaxEnemies_1 - 1; i++){
			if (enemies_1[i].health > 0){
				Output_object(enemies_1[i].area, enemies_1[i].position, enemies_1[i].shape);
			}
			else { // defeat enemies
				enemies_1[i].position[0] = 16;
				enemies_1[i].position[1] = 32;
				enemies_1[i].area[0] = 0;
				enemies_1[i].area[1] = 0;
				enemies_1[i].shape[0] = 0;
			}
		}
		
		for (int i = 0; i <= MaxEnemies_2 - 1; i++){
			if (enemies_2[i].health > 0){
				Output_object(enemies_2[i].area, enemies_2[i].position, enemies_2[i].shape);
			}
			else { // defeat enemies
				enemies_2[i].position[0] = 16;
				enemies_2[i].position[1] = 32;
				enemies_2[i].area[0] = 0;
				enemies_2[i].area[1] = 0;
				enemies_2[i].shape[0] = 0;
			}
		}
		for (int i = 0; i <= MaxEnemies_3 - 1; i++){
			if (enemies_3[i].health > 0){
				Output_object(enemies_3[i].area, enemies_3[i].position, enemies_3[i].shape);
			}
			else { // defeat enemies
				enemies_3[i].position[0] = 16;
				enemies_3[i].position[1] = 32;
				enemies_3[i].area[0] = 0;
				enemies_3[i].area[1] = 0;
				enemies_3[i].shape[0] = 0;
			}
		}
		for (int i = 0; i <= MaxEnemies_4 - 1; i++){
			if (enemies_4[i].health > 0){
				Output_object(enemies_4[i].area, enemies_4[i].position, enemies_4[i].shape);
			}
			else { // defeat enemies
				enemies_4[i].position[0] = 16;
				enemies_4[i].position[1] = 32;
				enemies_4[i].area[0] = 0;
				enemies_4[i].area[1] = 0;
				enemies_4[i].shape[0] = 0;
			}
		}
		for (int i = 0; i <= MaxBullets - 1; i++){
			if (bullets[i].health > 0){
				Output_object(bullets[i].area, bullets[i].position, bullets[i].shape);
			}
			else { // defeat enemies
				bullets[i].position[0] = 16;
				bullets[i].position[1] = 32;
				bullets[i].area[0] = 0;
				bullets[i].area[1] = 0;
				bullets[i].shape[0] = 0;
			}
		}
		if (boss_hp > 239) {}
		else if (boss_hp > 223) BOSS_HP[0].shape[7] = 0;
		else if (boss_hp > 207) BOSS_HP[0].shape[6] = 0;
		else if (boss_hp > 191) BOSS_HP[0].shape[5] = 0;
		else if (boss_hp > 175) BOSS_HP[0].shape[4] = 0;
		else if (boss_hp > 159) BOSS_HP[0].shape[3] = 0;
		else if (boss_hp > 143) BOSS_HP[0].shape[2] = 0;
		else if (boss_hp > 127) BOSS_HP[0].shape[1] = 0;
		else if (boss_hp > 111) BOSS_HP[0].shape[0] = 0;
		else if (boss_hp > 95) BOSS_HP[1].shape[7] = 0;
		else if (boss_hp > 79) BOSS_HP[1].shape[6] = 0;
		else if (boss_hp > 63) BOSS_HP[1].shape[5] = 0;
		else if (boss_hp > 47) BOSS_HP[1].shape[4] = 0;
		else if (boss_hp > 31) BOSS_HP[1].shape[3] = 0;
		else if (boss_hp > 15) BOSS_HP[1].shape[2] = 0;
		else if (boss_hp > 0) BOSS_HP[1].shape[1] = 0;
		else if (boss_hp <= 0) BOSS_HP[1].shape[0] = 0;
		for (int i = 0; i <= 2 - 1; i++){
			Output_object(BOSS_HP[i].area, BOSS_HP[i].position, BOSS_HP[i].shape);
		}
		boss_hp = 0;
		for (int i = 0; i <= MaxFeedback - 1; i++){
			if (feedback[i].health > 0){
				if (feedback[i].health%2)Output_object(feedback[i].area, feedback[i].position, feedback[i].shape);
				feedback[i].health -= 1;
			}
			else {
				feedback[i].position[0] = 16;
				feedback[i].position[1] = 32;
				feedback[i].area[0] = 0;
				feedback[i].area[1] = 0;
				feedback[i].shape[0] = 0;
			}
		}
		for (int i = 0; i <= Maxshield - 1; i++){
			if (shield[i].health > 0){
				shield[i].position[0] = players[0].position[0]-1;
				shield[i].position[1] = players[0].position[1]+1;
				if (shield[i].health%20 <= 10)Output_object(shield[i].area, shield[i].position, shield[i].shape);
				shield[i].health -= 1;
			}
			else {
				shield[i].position[0] = 16;
				shield[i].position[1] = 32;
				shield[i].area[0] = 0;
				shield[i].area[1] = 0;
				shield[i].shape[0] = 0;
			}
		}
		for (int i = 0; i <= MaxAttacks - 1; i++){
			if (attacks_1[i].health > 0){
				Output_object(attacks_1[i].area, attacks_1[i].position, attacks_1[i].shape);
			}
			else { // defeat enemies
				attacks_1[i].position[0] = 16;
				attacks_1[i].position[1] = 32;
				attacks_1[i].area[0] = 0;
				attacks_1[i].area[1] = 0;
				attacks_1[i].shape[0] = 0;
			}
			if (attacks_2[i].health > 0){
				Output_object(attacks_2[i].area, attacks_2[i].position, attacks_2[i].shape);
			}
			else { // defeat enemies
				attacks_2[i].position[0] = 16;
				attacks_2[i].position[1] = 32;
				attacks_2[i].area[0] = 0;
				attacks_2[i].area[1] = 0;
				attacks_2[i].shape[0] = 0;
			}
			if (attacks_3[i].health > 0){
				Output_object(attacks_3[i].area, attacks_3[i].position, attacks_3[i].shape);
			}
			else { // defeat enemies
				attacks_3[i].position[0] = 16;
				attacks_3[i].position[1] = 32;
				attacks_3[i].area[0] = 0;
				attacks_3[i].area[1] = 0;
				attacks_3[i].shape[0] = 0;
			}
		}
		for (int i = 0; i <= playersNum - 1; i++){
			if (players[i].health > 0){
				if (players[i].health != health_last){
					itoa(players[i].health, health_str, 10);
					LCD_DisplayString(2, health_str);
					LCD_DisplayString(32, "0");
					health_last = players[i].health;
				}
				Output_object(players[i].area, players[i].position, players[i].shape);
			}
			else if (player_1_life > 1){
				player_1_life--;
				itoa(player_1_life, life_str, 10);
				LCD_DisplayString(8, life_str);
				LCD_DisplayString(32, "0");
				players[i].position[0] = 6;
				players[i].position[1] = 28;
				players[i].health = 99;
				shield[shieldNum].position[0] = players[i].position[0] - 1;
				shield[shieldNum].position[1] = players[i].position[1] + 1;
				shield[shieldNum].area[0] = 5;
				shield[shieldNum].area[1] = 6;
				shield[shieldNum].shape[0] = 0b00011110;
				shield[shieldNum].shape[1] = 0b00100001;
				shield[shieldNum].shape[2] = 0b00100001;
				shield[shieldNum].shape[3] = 0b00100001;
				shield[shieldNum].shape[4] = 0b00011110;
				shield[shieldNum].health = 255;
				if (shieldNum < Maxshield - 1) shieldNum++;
				else shieldNum = 0;
			}
			else { // defeated
				players[i].position[0] = 16;
				players[i].position[1] = 40;
				players[i].area[0] = 0;
				players[i].area[1] = 0;
				players[i].shape[0] = 0;
				players[i].health = 0;
				state = You_Died;
				if(player_1_life == 1){
					LCD_DisplayString(8, "00");
					LCD_DisplayString(2, "00");
					LCD_DisplayString(32, "0");
					player_1_life--;
				}
				display_highest_score(score);
			}
		}
		for (int i = 0; i <= MaxBombs - 1; i++){
			if (bombs[i].health > 0){
				Output_object(bombs[i].area, bombs[i].position, bombs[i].shape);
			}
			else {
				bombs[i].position[0] = 16;
				bombs[i].position[1] = 32;
				bombs[i].area[0] = 0;
				bombs[i].area[1] = 0;
				bombs[i].shape[0] = 0;
				bombs[i].shape[1] = 0;
				bombs[i].shape[2] = 0;
				bombs[i].shape[3] = 0;
				bombs[i].shape[4] = 0;
				bombs[i].shape[5] = 0;
				bombs[i].shape[6] = 0;
				bombs[i].shape[7] = 0;
			}
			if (bomb_num != bomb_num_last){
				if (bomb_num_last == 1){
					LCD_DisplayString(20, "0");
					LCD_DisplayString(32, "0");
				}
				else{
					itoa(bomb_num, bomb_num_str, 10);
					LCD_DisplayString(20, bomb_num_str);
					LCD_DisplayString(32, "0");
				}
				bomb_num_last = bomb_num;
			}
		}
		if (alltime > stage_4){
			for (int i = 0; i <= 6 - 1; i++){
				if (boss[i].health > 0){
					Output_object(boss[i].area, boss[i].position, boss[i].shape);
					boss_hp += boss[i].health;
				}
				else { // defeat enemies
					boss[i].position[0] = 16;
					boss[i].position[1] = 32;
					boss[i].area[0] = 0;
					boss[i].area[1] = 0;
					boss[i].shape[0] = 0;
					boss[i].shape[1] = 0;
					boss[i].shape[2] = 0;
					boss[i].shape[3] = 0;
					boss[i].shape[4] = 0;
					boss[i].shape[5] = 0;
					boss[i].shape[6] = 0;
					boss[i].shape[7] = 0;
				}
			}
			if (boss[5].health <= 0){
				state = You_Win;
				display_highest_score(score);
			}
		}
		alltime++;
		break;
		case You_Win:
		LED_Matrix_7[0] = 0x3c;LED_Matrix_7[1] = 0x18;LED_Matrix_7[2] = 0x18;LED_Matrix_7[3] = 0x3c;
		LED_Matrix_7[4] = 0x66;LED_Matrix_7[5] = 0x66;LED_Matrix_7[6] = 0x66;LED_Matrix_7[7] = 0x00;
		LED_Matrix_6[0] = 0x1c;LED_Matrix_6[1] = 0x36;LED_Matrix_6[2] = 0x63;LED_Matrix_6[3] = 0x63;
		LED_Matrix_6[4] = 0x63;LED_Matrix_6[5] = 0x36;LED_Matrix_6[6] = 0x1c;LED_Matrix_6[7] = 0x00;
		LED_Matrix_5[0] = 0x7e;LED_Matrix_5[1] = 0x66;LED_Matrix_5[2] = 0x66;LED_Matrix_5[3] = 0x66;
		LED_Matrix_5[4] = 0x66;LED_Matrix_5[5] = 0x66;LED_Matrix_5[6] = 0x66;LED_Matrix_5[7] = 0x00;
		LED_Matrix_4[0] = 0x18;LED_Matrix_4[1] = 0x00;LED_Matrix_4[2] = 0x18;LED_Matrix_4[3] = 0x18;
		LED_Matrix_4[4] = 0x3c;LED_Matrix_4[5] = 0x3c;LED_Matrix_4[6] = 0x18;LED_Matrix_4[7] = 0x00;
		LED_Matrix_3[0] = 0x63;LED_Matrix_3[1] = 0x77;LED_Matrix_3[2] = 0x7f;LED_Matrix_3[3] = 0x6b;
		LED_Matrix_3[4] = 0x63;LED_Matrix_3[5] = 0x63;LED_Matrix_3[6] = 0x63;LED_Matrix_3[7] = 0x00;
		LED_Matrix_2[0] = 0x1e;LED_Matrix_2[1] = 0x0c;LED_Matrix_2[2] = 0x0c;LED_Matrix_2[3] = 0x0c;
		LED_Matrix_2[4] = 0x0c;LED_Matrix_2[5] = 0x0c;LED_Matrix_2[6] = 0x1e;LED_Matrix_2[7] = 0x00;
		LED_Matrix_1[0] = 0x63;LED_Matrix_1[1] = 0x63;LED_Matrix_1[2] = 0x73;LED_Matrix_1[3] = 0x7b;
		LED_Matrix_1[4] = 0x6f;LED_Matrix_1[5] = 0x67;LED_Matrix_1[6] = 0x63;LED_Matrix_1[7] = 0x00;
		LED_Matrix_0[0] = 0x18;LED_Matrix_0[1] = 0x00;LED_Matrix_0[2] = 0x18;LED_Matrix_0[3] = 0x18;
		LED_Matrix_0[4] = 0x3c;LED_Matrix_0[5] = 0x3c;LED_Matrix_0[6] = 0x18;LED_Matrix_0[7] = 0x00;
		break;
		case You_Died:
		LED_Matrix_7[0] = 0x3c;LED_Matrix_7[1] = 0x66;LED_Matrix_7[2] = 0x76;LED_Matrix_7[3] = 0x06;
		LED_Matrix_7[4] = 0x06;LED_Matrix_7[5] = 0x66;LED_Matrix_7[6] = 0x3c;LED_Matrix_7[7] = 0x00;
		LED_Matrix_6[0] = 0x66;LED_Matrix_6[1] = 0x66;LED_Matrix_6[2] = 0x66;LED_Matrix_6[3] = 0x7e;
		LED_Matrix_6[4] = 0x66;LED_Matrix_6[5] = 0x66;LED_Matrix_6[6] = 0x3c;LED_Matrix_6[7] = 0x00;
		LED_Matrix_5[0] = 0xc6;LED_Matrix_5[1] = 0xc6;LED_Matrix_5[2] = 0xc6;LED_Matrix_5[3] = 0xd6;
		LED_Matrix_5[4] = 0xfe;LED_Matrix_5[5] = 0xee;LED_Matrix_5[6] = 0xc6;LED_Matrix_5[7] = 0x00;
		LED_Matrix_4[0] = 0x7e;LED_Matrix_4[1] = 0x06;LED_Matrix_4[2] = 0x06;LED_Matrix_4[3] = 0x3e;
		LED_Matrix_4[4] = 0x06;LED_Matrix_4[5] = 0x06;LED_Matrix_4[6] = 0x7e;LED_Matrix_4[7] = 0x00;
		LED_Matrix_3[0] = 0x3c;LED_Matrix_3[1] = 0x66;LED_Matrix_3[2] = 0x66;LED_Matrix_3[3] = 0x66;
		LED_Matrix_3[4] = 0x66;LED_Matrix_3[5] = 0x66;LED_Matrix_3[6] = 0x3c;LED_Matrix_3[7] = 0x00;
		LED_Matrix_2[0] = 0x18;LED_Matrix_2[1] = 0x3c;LED_Matrix_2[2] = 0x66;LED_Matrix_2[3] = 0x66;
		LED_Matrix_2[4] = 0x66;LED_Matrix_2[5] = 0x66;LED_Matrix_2[6] = 0x66;LED_Matrix_2[7] = 0x00;
		LED_Matrix_1[0] = 0x7e;LED_Matrix_1[1] = 0x06;LED_Matrix_1[2] = 0x06;LED_Matrix_1[3] = 0x3e;
		LED_Matrix_1[4] = 0x06;LED_Matrix_1[5] = 0x06;LED_Matrix_1[6] = 0x7e;LED_Matrix_1[7] = 0x00;
		LED_Matrix_0[0] = 0x66;LED_Matrix_0[1] = 0x36;LED_Matrix_0[2] = 0x1e;LED_Matrix_0[3] = 0x3e;
		LED_Matrix_0[4] = 0x66;LED_Matrix_0[5] = 0x66;LED_Matrix_0[6] = 0x3e;LED_Matrix_0[7] = 0x00;
		break;
	}
	MAX7219_clear();
	for (int i = 1; i <= 8; i++) {
		MAX7219_writeData_single(0, i, LED_Matrix_0[i-1]);
		MAX7219_writeData_single(1, i, LED_Matrix_1[i-1]);
		MAX7219_writeData_single(2, i, LED_Matrix_2[i-1]);
		MAX7219_writeData_single(3, i, LED_Matrix_3[i-1]);
		MAX7219_writeData_single(4, i, LED_Matrix_4[i-1]);
		MAX7219_writeData_single(5, i, LED_Matrix_5[i-1]);
		MAX7219_writeData_single(6, i, LED_Matrix_6[i-1]);
		MAX7219_writeData_single(7, i, LED_Matrix_7[i-1]);
	}
	return state;
}


int TickFct_fire(int state) {
	switch(state) {
		case fire_Start: if(!(GetBit(PINA, 4))) state = fire_bullet;set_PWM(0);break;
		case fire_bullet:
		if (WeaponFlag == 0){
			if (players[0].health > 0){
				if(GetBit(PINA, 4)) { 
					bullets[BulletsNum].position[0] = players[0].position[0] + players[0].area[0]/2;
					// bullets fired from the middle of the object 0
					bullets[BulletsNum].position[1] = players[0].position[1] - players[0].area[1];
					// bullets fired from the top of the object 0
					bullets[BulletsNum].area[0] = 1;
					bullets[BulletsNum].area[1] = 3;
					bullets[BulletsNum].shape[0] = 0b00000111;
					bullets[BulletsNum].health = 1;
					if (BulletsNum < MaxBullets - 1) BulletsNum++;
					else BulletsNum = 0;	
					set_PWM(A6);
					state = fire_Start;
				}
			}
		}
		if (WeaponFlag == 1){
			if (players[0].health > 0){
				if (bomb_num > 0){
					if(GetBit(PINA, 4)) {
						set_PWM(A5);
						bombs[BombsNum].position[0] = 0;
						bombs[BombsNum].position[1] = players[0].position[1] + 1;
						bombs[BombsNum].area[0] = 8;
						bombs[BombsNum].area[1] = 3;
						bombs[BombsNum].shape[0] = 0b00000100;
						bombs[BombsNum].shape[1] = 0b00000100;
						bombs[BombsNum].shape[2] = 0b00000010;
						bombs[BombsNum].shape[3] = 0b00000010;
						bombs[BombsNum].shape[4] = 0b00000010;
						bombs[BombsNum].shape[5] = 0b00000001;
						bombs[BombsNum].shape[6] = 0b00000001;
						bombs[BombsNum].shape[7] = 0b00000001;
						bombs[BombsNum].health = 1;
					
						bombs[BombsNum+1].position[0] = 8;
						bombs[BombsNum+1].position[1] = players[0].position[1] + 1;
						bombs[BombsNum+1].area[0] = 8;
						bombs[BombsNum+1].area[1] = 3;
						bombs[BombsNum+1].shape[0] = 0b00000001;
						bombs[BombsNum+1].shape[1] = 0b00000001;
						bombs[BombsNum+1].shape[2] = 0b00000001;
						bombs[BombsNum+1].shape[3] = 0b00000010;
						bombs[BombsNum+1].shape[4] = 0b00000010;
						bombs[BombsNum+1].shape[5] = 0b00000010;
						bombs[BombsNum+1].shape[6] = 0b00000100;
						bombs[BombsNum+1].shape[7] = 0b00000100;
						bombs[BombsNum+1].health = 1;
						if (BombsNum < MaxBombs - 1) BombsNum += 2;
						else BombsNum = 0;
						bomb_num--;
						state = fire_Start;
					}
				}
			}
		}
		break;
	}
	return state;
}


int TickFct_PC_L(int state) {
	switch(state) {
		case PCL_Start:
		if (!GetBit(PINA, 0)) state = PC_left;
		else state = PCL_Start;
		break;
		case PC_left:
		if (GetBit(PINA, 0)) {	
			state = PCL_Start;
			if (players[0].position[0] > 0) players[0].position[0] -= 1;
			else if (players[0].position[0] == 0) players[0].position[0] = 15;
		}
		break;
	}
	return state;
}
int TickFct_PC_R(int state) {
	switch(state) {
		case PCR_Start:
		if (!GetBit(PINA, 1)) state = PC_right;
		else state = PCR_Start;
		break;
		case PC_right:
		if (GetBit(PINA, 1)) {
			state = PCR_Start;
			if (players[0].position[0] < 15) players[0].position[0] += 1;
			else if (players[0].position[0] == 15) players[0].position[0] = 0;
		}
		break;
	}
	return state;
}
int TickFct_PC_U(int state) {
	switch(state) {
		case PCU_Start:
		if (!GetBit(PINA, 2)) state = PC_up;
		else state = PCU_Start;
		break;
		case PC_up:
		if (GetBit(PINA, 2)) {
			state = PCU_Start;
			if (players[0].position[1] > 0) players[0].position[1] -= 1;
		}
		break;
	}
	return state;
}
int TickFct_PC_D(int state) {
	switch(state) {
		case PCD_Start:
		if (!GetBit(PINA, 3)) state = PC_down;
		else state = PCD_Start;
		break;
		case PC_down:
		if (GetBit(PINA, 3)) {
			state = PCD_Start;
			if (players[0].position[1] < 31) players[0].position[1] += 1;
		}
		break;
	}
	return state;
}
int TickFct_Restart(int state){
	switch(state){
		case Restart_Start:
		if (!GetBit(PINA, 6)) state = Restart_do;break;
		case Restart_do:
		if (GetBit(PINA, 6)) {
			state = Restart_Start;
			players[0].position[0] = 6;
			players[0].position[1] = 28;
			players[0].health = 99;
			player_1_life = 3;
			bomb_num = 3;
			bomb_num_last = 0;
			score = 0;
			LCD_DisplayString(0, "HP");
			itoa(health_last, health_str, 10);
			LCD_DisplayString(2, health_str);
			LCD_DisplayString(4, "Life");
			itoa(player_1_life, life_str, 10);
			LCD_DisplayString(8, life_str);
			LCD_DisplayString(10, "BULLET");
			LCD_DisplayString(16, "Bomb");
			itoa(bomb_num, bomb_num_str, 10);
			LCD_DisplayString(20, bomb_num_str);
			LCD_DisplayString(22, "Score");
			itoa(score, score_str, 10);
			LCD_DisplayString(27, score_str);
			LCD_DisplayString(27, "00");
			LCD_DisplayString(32, "0");
			for (int i = 0; i <= 6 - 1; i++){
				boss[i].health = 0;
			}
			for (int i = 0; i <= MaxEnemies_1 - 1; i++){
				enemies_1[i].health = 0;
			}
			for (int i = 0; i <= MaxEnemies_2 - 1; i++){
				enemies_2[i].health = 0;
			}
			for (int i = 0; i <= MaxEnemies_3 - 1; i++){
				enemies_3[i].health = 0;
			}
			for (int i = 0; i <= MaxEnemies_4 - 1; i++){
				enemies_4[i].health = 0;
			}
			for (int i = 0; i <= MaxAttacks - 1; i++){
				attacks_1[i].health = 0;
				attacks_2[i].health = 0;
				attacks_3[i].health = 0;
			}
			alltime = 0;
		}
		break;
	}
	return state;
}
void display_highest_score(unsigned long score){
	score_str[0]=0;score_str[1]=0;score_str[2]=0;
	score_str[0] = EEPROM_read(0);
	score_str[1] = EEPROM_read(1);
	score_str[2] = EEPROM_read(2);
	score_highest = atoi(score_str);
	if (score > score_highest){
		itoa(score, score_str, 10);
		EEPROM_write(0, score_str[0]);
		EEPROM_write(1, score_str[1]);
		EEPROM_write(2, score_str[2]);
	}
	LCD_ClearScreen();
	LCD_DisplayString(0, "HighestScore:");
	LCD_DisplayString(13, score_str);
}