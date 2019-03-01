#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <bit.h>
#include <timer.h>
#include <ledmatrix.h>

typedef struct object {
	signed char position[2];
	unsigned char area[2];
	unsigned char shape[3];
	unsigned char health;
} object;
object objects[3];
const unsigned char objectsNum = 3;
const unsigned long ControlPeriod = 100;

enum Player_control_left {PCL_Start, PC_left};
int TickFct_PC_L(int state);
enum Player_control_right {PCR_Start, PC_right};
int TickFct_PC_R(int state);
enum Player_control_up {PCU_Start, PC_up};
int TickFct_PC_U(int state);
enum Player_control_down {PCD_Start, PC_down};
int TickFct_PC_D(int state);
enum TickFct_Bullet {Blt_Start, Blt_fire};
int TickFct_Blt(int state);
enum TickFct_output {output_Start};
int TickFct_OutP(int state);

object bullets[999]; // maximum number of bullets
unsigned char BulletsNum = 0;
int main(void) {
	// signed _position[2];
	DDRA = 0x00; PORTA = 0xFF;
	SPI_MasterInit();
	MAX7219_init();
	TimerSet(tasksPeriodGCD);
	TimerOn();
	// player object
	int j = 0;
	objects[j].position[0] = 4;
	objects[j].position[1] = 28;
	objects[j].area[0] = 3;
	objects[j].area[1] = 4;
	objects[j].shape[0] = 0b00001100;
	objects[j].shape[1] = 0b00000111;
	objects[j].shape[2] = 0b00001100;
	objects[j].health = 3;
	// enemy 1 object
	j++;
	objects[j].position[0] = 8;
	objects[j].position[1] = 4;
	objects[j].area[0] = 3;
	objects[j].area[1] = 3;
	objects[j].shape[0] = 0b00000011;
	objects[j].shape[1] = 0b00000110;
	objects[j].shape[2] = 0b00000011;
	objects[j].health = 3;
	j++;
	//test
	objects[j].position[0] = 10;
	objects[j].position[1] = 14;
	objects[j].area[0] = 3;
	objects[j].area[1] = 7;
	objects[j].shape[0] = 0b00100011;
	objects[j].shape[1] = 0b01111110;
	objects[j].shape[2] = 0b00100011;
	objects[j].health = 3;

	int i = 0;
	tasks[i].state = output_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_OutP;
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
	while(1)
	{
		/*for (i = 0; i < tasksNum; ++i) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
		}*/
	}
}
int TickFct_OutP(int state)
{
	switch(state)
	{
		case output_Start:
		MAX7219_clear();
		for (int i = 0; i <= BulletsNum - 1; i++){
			if(BulletsNum - 1 >= 0) {
				if (bullets[i].position[1] >= 0) {
					if (bullets[i].position[1] == objects[1].position[1]) {
						if (objects[1].health > 0)objects[1].health -= 1;
						if (bullets[i].health > 0)bullets[i].health -= 1;
					}
					if (bullets[i].health > 0) {
						Output_object(bullets[i].area, bullets[i].position, bullets[i].shape);
						bullets[i].position[1] -= 1;
					}
					else {
						bullets[i].position[1] = -1;
					}
				}
			}
		}
		for (int i = 0; i <= objectsNum - 2; i++){
			if (objects[i].health > 0){
				Output_object(objects[i].area, objects[i].position, objects[i].shape);
			}
			else {
				objects[i].position[0] = 16;
				objects[i].position[1] = 32;
				objects[i].area[0] = 0;
				objects[i].area[1] = 0;
				objects[i].shape[0] = 0;
			}
		}
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
		break;
	}
	return state;
}
int TickFct_Blt(int state)
{
	switch(state)
	{
		case Blt_Start: if(!(GetBit(PINA, 4))) state = Blt_fire;break;
		case Blt_fire:
		if(GetBit(PINA, 4))
		{
			bullets[BulletsNum].position[0] = objects[0].position[0] + objects[0].area[0]/2;
			// bullets fired from the middle of the object 0
			bullets[BulletsNum].position[1] = objects[0].position[1] - objects[0].area[1];
			// bullets fired from the top of the object 0
			bullets[BulletsNum].area[0] = 1;
			bullets[BulletsNum].area[1] = 3;
			bullets[BulletsNum].shape[0] = 0b00000111;
			bullets[BulletsNum].health = 1;
			BulletsNum++;
			state = Blt_Start;
		}
		break;
	}
	return state;
}


int TickFct_PC_L(int state)
{
	switch(state)
	{
		case PCL_Start:
		if (!GetBit(PINA, 0)) state = PC_left;
		else state = PCL_Start;
		break;
		case PC_left:
		if (GetBit(PINA, 0))
		{	
			state = PCL_Start;
			if (objects[0].position[0] > 0) objects[0].position[0] -= 1;
			else if (objects[0].position[0] == 0) objects[0].position[0] = 15;
		}
		break;
	}
	return state;
}
int TickFct_PC_R(int state)
{
	switch(state)
	{
		case PCR_Start:
		if (!GetBit(PINA, 1)) state = PC_right;
		else state = PCR_Start;
		break;
		case PC_right:
		if (GetBit(PINA, 1))
		{
			state = PCR_Start;
			if (objects[0].position[0] < 15) objects[0].position[0] += 1;
			else if (objects[0].position[0] == 15) objects[0].position[0] = 0;
		}
		break;
	}
	return state;
}
int TickFct_PC_U(int state)
{
	switch(state)
	{
		case PCU_Start:
		if (!GetBit(PINA, 2)) state = PC_up;
		else state = PCU_Start;
		break;
		case PC_up:
		if (GetBit(PINA, 2))
		{
			state = PCU_Start;
			if (objects[0].position[1] > 0) objects[0].position[1] -= 1;
		}
		break;
	}
	return state;
}
int TickFct_PC_D(int state)
{
	switch(state)
	{
		case PCD_Start:
		if (!GetBit(PINA, 3)) state = PC_down;
		else state = PCD_Start;
		break;
		case PC_down:
		if (GetBit(PINA, 3))
		{
			state = PCD_Start;
			if (objects[0].position[1] < 31) objects[0].position[1] += 1;
		}
		break;
	}
	return state;
}
