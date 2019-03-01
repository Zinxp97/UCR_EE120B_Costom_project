#include <avr/io.h>
#include <stdio.h>
//#include <util/delay.h>
#include <bit.h>
#include <timer.h>
#include <ledmatrix.h>
#include <stdlib.h>
#include "io.c"
#include <string.h>
void int2str(signed short n, char *str){
	char buf[5] = "";
	int i = 0;
	int len = 0;
	int temp = n < 0 ? -n: n;
	if (str == NULL)return;
	if (temp == 0) buf[0] = '0';
	while(temp){
		buf[i++] = (temp % 10) + '0';
		temp = temp / 10;
	}
	len = i;
	str[i] = 0;
	while(1){
		i--;
		if (buf[len - i - 1] == 0)break;
		str[i] = buf[len - i - 1];
	}
}

typedef struct object {
	signed char position[2];
	unsigned char area[2];
	unsigned char shape[5];
	signed short health;
} object;

enum Clear {Clear_Start};
int TickFct_Clear(int state);
enum Kill {Kill_Start};
int TickFct_Kill(int state);
enum Show {Show_Start};
int TickFct_Show(int state);
enum Player_control_left {PCL_Start, PC_left};
int TickFct_PC_L(int state);
enum Player_control_right {PCR_Start, PC_right};
int TickFct_PC_R(int state);
enum Player_control_up {PCU_Start, PC_up};
int TickFct_PC_U(int state);
enum Player_control_down {PCD_Start, PC_down};
int TickFct_PC_D(int state);
enum Bullet {Blt_Start, Blt_fire};
int TickFct_Blt(int state);
enum enemies_generate {EG_Start};
int TickFct_EG(int state); //random generate enemies
enum enemies_move {EM_Start};
int TickFct_EM(int state);
enum attack_state {A_Start};
int TickFct_Attack(int state);
enum Gotshot_state {GS_Start};
int TickFct_Gotshot(int state);

/////////////////////////////////////////// LCD display 
unsigned char player_1_life_last = 3;
unsigned char player_1_life = 3;
unsigned char life_str[3];
unsigned char health_last = 99;
unsigned char health_str[3];
unsigned long score = 0;
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
const unsigned short Enemies_1Speed = 30;
//////////////////////////////////////// Number
const unsigned char playersNum = 1; // number of players
const unsigned char MaxBullets = 20; // maximum number of bullets in the screen
const unsigned char MaxAttacks = 20;// maximum number of attacks in the screen
const unsigned char MaxEnemies_1 = 20; // maximum number of enemies in the screen
const unsigned char MaxFeedback = 30;
const unsigned char Maxshield = 2;
//////////////////////////////////////// Score
const unsigned char enemies_1_score = 10;
const unsigned char enemies_2_score = 30;
//////////////////////////////////////// Damage
const unsigned char BulletDamage = 10;
const unsigned char Attack_123_Damage = 10;
////////////////////////////////////////
unsigned char Attacks_1Num = 0; 
unsigned char Attacks_2Num = 0;
unsigned char Attacks_3Num = 0;
unsigned char BulletsNum = 0;
unsigned char enemies_1Num = 0;
unsigned char feedbackNum = 0;
unsigned char shieldNum = 0;
object players[1];
object enemies_1[20]; // maximum number of enemies_1
object enemies_2[10];
object attacks_1[20]; // maximum number of attacks_1
object attacks_2[20];
object attacks_3[20];
unsigned char attack_1_slope_timer = 0;
unsigned char attack_1_rate_timer = 0;
unsigned char attack_2_rate_timer = 0;
unsigned char attack_3_rate_timer = 0;
object bullets[20]; // maximum number of bullets
object feedback[30];
object shield[2];
int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	SPI_MasterInit();
	MAX7219_init();
	LCD_init();
	LCD_ClearScreen();
	LCD_DisplayString(0, "HP");
	int2str(health_last, health_str);
	LCD_DisplayString(2, health_str);
	LCD_DisplayString(4, "Life");
	int2str(player_1_life, life_str);
	LCD_DisplayString(8, life_str);
	LCD_DisplayString(16, "Bomb");
	int2str(bomb_num, bomb_num_str);
	LCD_DisplayString(20, bomb_num_str);
	LCD_DisplayString(22, "Score");
	int2str(score, score_str);
	LCD_DisplayString(27, score_str);
	
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
	for(int i = 0; i <= MaxBullets - 1; i++){
		bullets[i].position[0] = 16;
		bullets[i].position[1] = 32;
		bullets[i].area[0] = 0;
		bullets[i].area[1] = 0;
		bullets[i].shape[0] = 0;
		bullets[i].health = 0;
	}
	// enemy type_1
	for(int i = 0; i <= MaxEnemies_1 - 1; i++){
		enemies_1[i].position[0] = 16;
		enemies_1[i].position[1] = 32;
		enemies_1[i].area[0] = 0;
		enemies_1[i].area[1] = 0;
		enemies_1[i].shape[0] = 0;
		enemies_1[i].health = 0;
	}
	i = 0;
	//test
	enemies_2[i].position[0] = 10;
	enemies_2[i].position[1] = 14;
	enemies_2[i].area[0] = 3;
	enemies_2[i].area[1] = 7;
	enemies_2[i].shape[0] = 0b00100011;
	enemies_2[i].shape[1] = 0b01111110;
	enemies_2[i].shape[2] = 0b00100011;
	enemies_2[i].health = 3;

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
	tasks[i].state = Blt_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Blt;
	i++;
	tasks[i].state = EG_Start;
	tasks[i].period = Enemies_1Frequnce;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_EG;
	i++;
	tasks[i].state = EM_Start;
	tasks[i].period = Enemies_1Speed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_EM;
	i++;
	while(1){}
}
int TickFct_EM(int state){
	switch(state){
		case EM_Start:
		for (int i = 0; i <= MaxEnemies_1 - 1; i++){
			if (enemies_1[i].health > 0){
				if (enemies_1[i].position[1] <= 31){
					enemies_1[i].position[1] += rand()%2;
					enemies_1[i].position[0] += rand()%3-1; //012 - 1 = -101
					if (enemies_1[i].position[0] >= 15) enemies_1[i].position[0] = 0;
					if (enemies_1[i].position[0] <= 0) enemies_1[i].position[0] = 15;
				}
				else enemies_1[i].health = 0;
			}
		}
		break;
	}
	return state;
}
int TickFct_EG(int state){
	switch(state){
		case EG_Start:
		enemies_1[enemies_1Num].position[0] = (rand()%15);
		enemies_1[enemies_1Num].position[1] = (rand()%8);// 会被改成0
		enemies_1[enemies_1Num].area[0] = 3;
		enemies_1[enemies_1Num].area[1] = 3;
		enemies_1[enemies_1Num].shape[0] = 0b00000011;
		enemies_1[enemies_1Num].shape[1] = 0b00000110;
		enemies_1[enemies_1Num].shape[2] = 0b00000011;
		enemies_1[enemies_1Num].health = 30;
		if (enemies_1Num < MaxEnemies_1 - 1) enemies_1Num++;
		else enemies_1Num = 0;
		break;
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
							((attacks_1[i].position[1]==shield[j].position[1]-1)&&(attacks_1[i].position[0]==shield[j].position[0]||attacks_1[i].position[0]==shield[j].position[0]+4))||\
							((attacks_1[i].position[1]==shield[j].position[1]-2)&&(attacks_1[i].position[0]==shield[j].position[0]||attacks_1[i].position[0]==shield[j].position[0]+4))||\
							((attacks_1[i].position[1]==shield[j].position[1]-3)&&(attacks_1[i].position[0]==shield[j].position[0]||attacks_1[i].position[0]==shield[j].position[0]+4))||\
							((attacks_1[i].position[1]==shield[j].position[1]-4)&&(attacks_1[i].position[0]==shield[j].position[0]||attacks_1[i].position[0]==shield[j].position[0]+4))||\
							(attacks_1[i].position[1]==shield[j].position[1]-5&&abs(attacks_1[i].position[0]-(shield[j].position[0]+2))<=1)){
								attacks_1[i].health -= 1;
							}
						}
					}
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
							((attacks_2[i].position[1]==shield[j].position[1]-1)&&(attacks_2[i].position[0]==shield[j].position[0]||attacks_2[i].position[0]==shield[j].position[0]+4))||\
							((attacks_2[i].position[1]==shield[j].position[1]-2)&&(attacks_2[i].position[0]==shield[j].position[0]||attacks_2[i].position[0]==shield[j].position[0]+4))||\
							((attacks_2[i].position[1]==shield[j].position[1]-3)&&(attacks_2[i].position[0]==shield[j].position[0]||attacks_2[i].position[0]==shield[j].position[0]+4))||\
							((attacks_2[i].position[1]==shield[j].position[1]-4)&&(attacks_2[i].position[0]==shield[j].position[0]||attacks_2[i].position[0]==shield[j].position[0]+4))||\
							(attacks_2[i].position[1]==shield[j].position[1]-5&&abs(attacks_2[i].position[0]-(shield[j].position[0]+2))<=1)){
								attacks_2[i].health -= 1;
							}
						}
					}
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
							((attacks_3[i].position[1]==shield[j].position[1]-1)&&(attacks_3[i].position[0]==shield[j].position[0]||attacks_3[i].position[0]==shield[j].position[0]+4))||\
							((attacks_3[i].position[1]==shield[j].position[1]-2)&&(attacks_3[i].position[0]==shield[j].position[0]||attacks_3[i].position[0]==shield[j].position[0]+4))||\
							((attacks_3[i].position[1]==shield[j].position[1]-3)&&(attacks_3[i].position[0]==shield[j].position[0]||attacks_3[i].position[0]==shield[j].position[0]+4))||\
							((attacks_3[i].position[1]==shield[j].position[1]-4)&&(attacks_3[i].position[0]==shield[j].position[0]||attacks_3[i].position[0]==shield[j].position[0]+4))||\
							(attacks_3[i].position[1]==shield[j].position[1]-5&&abs(attacks_3[i].position[0]-(shield[j].position[0]+2))<=1)){
								attacks_3[i].health -= 1;
							}
						}
					}
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
			/////
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
	}
	return state;
}
int TickFct_Clear(int state) {
	switch(state){
		case Clear_Start:
		//LCD_ClearScreen();
		//MAX7219_clear();
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
									int2str(score, score_str);
									LCD_DisplayString(27, score_str);
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
		for (int i = 0; i <= playersNum - 1; i++){
			if (players[i].health > 0){
				if (players[0].health != health_last){
					int2str(players[0].health, health_str);
					LCD_DisplayString(2, health_str);
					health_last = players[0].health;
				}
				///////////////////////////////
				Output_object(players[i].area, players[i].position, players[i].shape);
			}
			else if (player_1_life > 0){
				player_1_life -= 1;
				int2str(player_1_life, life_str);
				LCD_DisplayString(8, life_str);
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
				shield[shieldNum].health = 99;
				if (shieldNum < Maxshield - 1) shieldNum++;
				else shieldNum = 0;
			}
			else { // defeated
				players[i].position[0] = 16;
				players[i].position[1] = 32;
				players[i].area[0] = 0;
				players[i].area[1] = 0;
				players[i].shape[0] = 0;
				LCD_DisplayString(2, '0');
			}
		}

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
				if (shield[i].health%10 <= 5)Output_object(shield[i].area, shield[i].position, shield[i].shape);
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
		}// HP**Life**Laser//Bullet//Bomb   Bomb**Score*** 
		
		break;
	}
	return state;
}


int TickFct_Blt(int state) {
	switch(state) {
		case Blt_Start: if(!(GetBit(PINA, 4))) state = Blt_fire;break;
		case Blt_fire:
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
			state = Blt_Start;
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
