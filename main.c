#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#define PIN_SCK                   PORTB7
#define PIN_MOSI                  PORTB5
#define PIN_SS                    PORTB4
#define MAX7219_LOAD1             PORTB |= (1 << PIN_SS)
#define MAX7219_LOAD0             PORTB &= ~(1 << PIN_SS)

#define MAX7219_MODE_DECODE       0x09
#define MAX7219_MODE_INTENSITY    0x0A
#define MAX7219_MODE_SCAN_LIMIT   0x0B
#define MAX7219_MODE_SHUTDOWN     0x0C
#define MAX7219_MODE_TEST         0x0F
#define MAX7219_MODE_NOOP         0x00

#define NUM_OF_MATRIXES 8

void SPI_MasterInit(void) {
	DDRB |= (1 << PORTB5) | (1 << PORTB7) | (1 << PORTB4);
	SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}
void SPI_MasterTransmit(char cData) {
	SPDR = cData;
	while(!(SPSR & (1 << SPIF)));
}
void MAX7219_writeData(char data_register, char data) {
	MAX7219_LOAD0;
	SPI_MasterTransmit(data_register);
	SPI_MasterTransmit(data);
	MAX7219_LOAD1;
}
void MAX7219_clear() {
	for (int i = 1; i <= 8; i++) {
		MAX7219_writeData(i, 0);
		MAX7219_writeData(i, 0);
		MAX7219_writeData(i, 0);
		MAX7219_writeData(i, 0);
		MAX7219_writeData(i, 0);
		MAX7219_writeData(i, 0);
		MAX7219_writeData(i, 0);
		MAX7219_writeData(i, 0);
	}
}
void MAX7219_init() {
	MAX7219_writeData(MAX7219_MODE_DECODE, 0);
	MAX7219_writeData(MAX7219_MODE_SCAN_LIMIT, 7);
	MAX7219_writeData(MAX7219_MODE_INTENSITY, 1);
	MAX7219_writeData(MAX7219_MODE_SHUTDOWN, 1);
	MAX7219_writeData(MAX7219_MODE_TEST, 0);
	MAX7219_clear();
}


void MAX7219_writeData_single(char matrix,char data_register, char data) {
	if (matrix >= 0 && matrix < NUM_OF_MATRIXES) {
		MAX7219_LOAD0;
		for (int i = NUM_OF_MATRIXES - 1; i >= 0; i--) {
			if (i == matrix)SPI_MasterTransmit(data_register);
			else SPI_MasterTransmit(0x00);
			SPI_MasterTransmit(data);
		}
		MAX7219_LOAD1;
	}
}

unsigned char LED_Matrix_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char LED_Matrix_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char LED_Matrix_2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char LED_Matrix_3[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char LED_Matrix_4[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char LED_Matrix_5[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char LED_Matrix_6[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char LED_Matrix_7[8] = {0, 0, 0, 0, 0, 0, 0, 0};

typedef struct object {
	signed char position[2];
	unsigned char area[2];
	unsigned char shape[];
} object;
object objects[1];
const unsigned char objectsNum = 1;
void Output_object_LED_0(unsigned char area[2], signed char position[2], unsigned char shape[]);
void Output_object_LED_1(unsigned char area[2], signed char position[2], unsigned char shape[]);
void Output_object_LED_2(unsigned char area[2], signed char position[2], unsigned char shape[]);
void Output_object_LED_3(unsigned char area[2], signed char position[2], unsigned char shape[]);
void Output_object_LED_4(unsigned char area[2], signed char position[2], unsigned char shape[]);
void Output_object_LED_5(unsigned char area[2], signed char position[2], unsigned char shape[]);
void Output_object_LED_6(unsigned char area[2], signed char position[2], unsigned char shape[]);
void Output_object_LED_7(unsigned char area[2], signed char position[2], unsigned char shape[]);

void Output_object_LED_0(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	for (int i = 0; i <= area[0] - 1; i++) {
		if ((position[1]-area[1]+1) >= 0) { 
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_0[position[0]+i] = shape[i] << (position[1] - area[1] + 1);
		}
		else if (position[1]-area[1]+1 < 0) {//³¬¹ýLED0ÆÁÄ»·¶Î§µ½LED1
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_0[position[0]+i] = shape[i] >> (area[1] - position[1] - 1);
			position[1] += 8;
			Output_object_LED_1(area, position, shape);
			position[1] -= 8;
		}
		if ((position[0]+i) >= 8) {//³¬¹ýLED0ÆÁÄ»·¶Î§µ½LED4
			position[0] -= 8; 
			Output_object_LED_4(area, position, shape);
			position[0] += 8;
		}
	}
}
void Output_object_LED_1(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	for (int i = 0; i <= area[0] - 1; i++) {
		if ((position[1]-area[1]+1) >= 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_1[position[0]+i] = shape[i] << (position[1] - area[1] + 1);
		}
		else if (position[1]-area[1]+1 < 0) {//³¬¹ýLED1ÆÁÄ»·¶Î§µ½LED2
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_1[position[0]+i] = shape[i] >> (area[1] - position[1] - 1);
			position[1] += 8;
			Output_object_LED_2(area, position, shape);
			position[1] -= 8;
		}
		if ((position[0]+i) >= 8) {//³¬¹ýLED1ÆÁÄ»·¶Î§µ½LED5
			position[0] -= 8; 
			Output_object_LED_5(area, position, shape);
			position[0] += 8;
		}
	}
}
void Output_object_LED_2(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	for (int i = 0; i <= area[0] - 1; i++) {
		if ((position[1]-area[1]+1) >= 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_2[position[0]+i] = shape[i] << (position[1] - area[1] + 1);
		}
		else if (position[1]-area[1]+1 < 0) {//³¬¹ýLED1ÆÁÄ»·¶Î§µ½LED2
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_2[position[0]+i] = shape[i] >> (area[1] - position[1] - 1);
			position[1] += 8;
			Output_object_LED_3(area, position, shape);
			position[1] -= 8;
		}
		if ((position[0]+i) >= 8) {//³¬¹ýLED1ÆÁÄ»·¶Î§µ½LED5
			position[0] -= 8;
			Output_object_LED_6(area, position, shape);
			position[0] += 8;
		}
	}
}
void Output_object_LED_3(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	for (int i = 0; i <= area[0] - 1; i++) {
		if ((position[1]-area[1]+1) >= 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_3[position[0]+i] = shape[i] << (position[1] - area[1] + 1);
		}
		else if (position[1]-area[1]+1 < 0) {//³¬¹ýLED1ÆÁÄ»·¶Î§µ½LED2
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_3[position[0]+i] = shape[i] >> (area[1] - position[1] - 1);
		}
		if ((position[0]+i) >= 8) {//³¬¹ýLED1ÆÁÄ»·¶Î§µ½LED5
			position[0] -= 8;
			Output_object_LED_7(area, position, shape);
			position[0] += 8;
		}
	}
}
void Output_object_LED_4(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	for (int i = 0; i <= area[0] - 1; i++) {
		if (position[1]-area[1]+1 >= 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_4[position[0]+i] = shape[i] << (position[1] - area[1] + 1);
		}
		else if (position[1]-area[1]+1 < 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_4[position[0]+i] = shape[i] >> (area[1] - position[1] - 1);
			position[1] += 8;
			Output_object_LED_5(area, position, shape);
			position[1] -= 8;
		}
	}
}
void Output_object_LED_5(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	for (int i = 0; i <= area[0] - 1; i++) {
		if (position[1]-area[1]+1 >= 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_5[position[0]+i] = shape[i] << (position[1] - area[1] + 1);
		} 
		else if (position[1]-area[1]+1 < 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_5[position[0]+i] = shape[i] >> (area[1] - position[1] - 1);
			position[1] += 8;
			Output_object_LED_6(area, position, shape);
			position[1] -= 8;
		}
	}
}
void Output_object_LED_6(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	for (int i = 0; i <= area[0] - 1; i++) {
		if (position[1]-area[1]+1 >= 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_6[position[0]+i] = shape[i] << (position[1] - area[1] + 1);
		}
		else if (position[1]-area[1]+1 < 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_6[position[0]+i] = shape[i] >> (area[1] - position[1] - 1);
			position[1] += 8;
			Output_object_LED_7(area, position, shape);
			position[1] -= 8;
		}
	}
}
void Output_object_LED_7(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	for (int i = 0; i <= area[0] - 1; i++) {
		if (position[1]-area[1]+1 >= 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_7[position[0]+i] = shape[i] << (position[1] - area[1] + 1);
		}
		else if (position[1]-area[1]+1 < 0) {
			if (position[0]+i >= 0 && position[0]+i <= 7) LED_Matrix_7[position[0]+i] = shape[i] >> (area[1] - position[1] - 1);
		}
	}
}
void Output_object(unsigned char area[2], signed char position[2], unsigned char shape[]) {
	if (position[1] >= 24) {//´óÓÚ24ÔòÎª×óÊýµÚÒ»¿é°å
		position[1] -= 24;
		if (position[0] >= 8) {
			position[0] -= 8;
			Output_object_LED_4(area, position, shape);
		}
		else if (position[0] >= 0) Output_object_LED_0(area, position, shape);
	}
	else if (position[1] >= 16) {
		position[1] -= 16;
		if (position[0] >= 8) {
			position[0] -= 8;
			Output_object_LED_5(area, position, shape);
		}
		else if (position[0] >= 0) Output_object_LED_1(area, position, shape);
	}
	else if (position[1] >= 8) {
		position[1] -= 8;
		if (position[0] >= 8) {
			position[0] -= 8;
			Output_object_LED_6(area, position, shape);
		}
		else if (position[0] >= 0) Output_object_LED_2(area, position, shape);
	}
	else if (position[1] >= 0) {
		if (position[0] >= 8) {
			position[0] -= 8;
			Output_object_LED_7(area, position, shape);
		}
		else if (position[0] >= 0) Output_object_LED_3(area, position, shape);
	}
}
int main(void) {
	SPI_MasterInit();
	MAX7219_init();
	// player object
	objects[0].position[0] = 6;
	objects[0].position[1] = 16;
	objects[0].area[0] = 3;
	objects[0].area[1] = 4;
	objects[0].shape[0] = 0b00001100;
	objects[0].shape[1] = 0b00000111;
	objects[0].shape[2] = 0b00001100;


	Output_object(objects[0].area, objects[0].position, objects[0].shape);

	
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
}