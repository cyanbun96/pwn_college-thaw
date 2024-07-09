#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define SCR_W 60
#define SCR_H 15

#define X_POS 5
#define Y_POS 5

int lfsr_len = 7;
unsigned int lfsr = 1337;
unsigned int tap1 = 2;
unsigned int tap2 = 3;

char sb[SCR_H][SCR_W];

void drawScreen(){
	putchar('\n');
	for(int i = 0; i < SCR_H; ++i){
		for(int j = 0; j < SCR_W; ++j){
			putchar(sb[i][j]);
		}
		putchar('\n');
	}
}

void initScreen(){
	memset(sb, ' ', SCR_H * SCR_W);
	sb[Y_POS + 3][X_POS] = '|';
	sb[Y_POS + 4][X_POS] = '^';
	for(int i = 0; i < lfsr_len; ++i){
		sb[Y_POS][X_POS + i * 2] = '+';
		sb[Y_POS][X_POS + i * 2 + 1] = '-';
		sb[Y_POS + 1][X_POS + i * 2] = '>';
		sb[Y_POS + 2][X_POS + i * 2] = '+';
		sb[Y_POS + 2][X_POS + i * 2 + 1] = '-';
	}
	sb[Y_POS][X_POS + lfsr_len * 2] = '+';
	sb[Y_POS + 1][X_POS + lfsr_len * 2] = '>';
	sb[Y_POS + 2][X_POS + lfsr_len * 2] = '+';
	sb[Y_POS + 3][X_POS + lfsr_len * 2] = '+';
	sb[Y_POS + 3][X_POS + lfsr_len * 2] = '|';
	sb[Y_POS + 4][X_POS + lfsr_len * 2] = '^';
	sb[Y_POS + 3][X_POS + 1] = 'R';
	sb[Y_POS + 3][X_POS + 2] = 'e';
	sb[Y_POS + 3][X_POS + 3] = 'g';
	sb[Y_POS + 3][X_POS + 4] = 'i';
	sb[Y_POS + 3][X_POS + 5] = 's';
	sb[Y_POS + 3][X_POS + 6] = 't';
	sb[Y_POS + 3][X_POS + 7] = 'e';
	sb[Y_POS + 3][X_POS + 8] = 'r';

	sb[Y_POS - 5][X_POS] = 'T';
	sb[Y_POS - 5][X_POS + 8] = 'T';
	sb[Y_POS - 4][X_POS] = '+';
	sb[Y_POS - 3][X_POS] = '<';
	for(int i = 1; i < 8; ++i){
		sb[Y_POS - 4][X_POS + i] = '-';
	}
	for(int i = 0; i < lfsr_len * 2 - 9; ++i){
		sb[Y_POS - 3][X_POS + 9 + i] = '=';
	}
	sb[Y_POS - 3][X_POS + lfsr_len * 2] = '#';

	sb[Y_POS - 4][X_POS + 8] = '+';
	sb[Y_POS - 3][X_POS + 8] = '<';
	sb[Y_POS - 2][X_POS] = '+';
	for(int i = 1; i < 8; ++i){
		sb[Y_POS - 2][X_POS + i] = '-';
	}
	sb[Y_POS - 2][X_POS + 8] = '+';

	sb[Y_POS - 3][X_POS + 3] = 'X';
	sb[Y_POS - 3][X_POS + 4] = 'O';
	sb[Y_POS - 3][X_POS + 5] = 'R';

	int tap1xpos = X_POS - 1 + lfsr_len * 2 - (tap1 - 1) * 2;
	sb[Y_POS - 1][tap1xpos] = 'A';
	if(tap1xpos > X_POS + 8){
		sb[Y_POS - 2][tap1xpos] = '|';
		sb[Y_POS - 3][tap1xpos] = '+';
	}
	int tap2xpos = X_POS - 1 + lfsr_len * 2 - (tap2 - 1) * 2;
	sb[Y_POS - 1][tap2xpos] = 'A';
	if(tap2xpos > X_POS + 8){
		sb[Y_POS - 2][tap2xpos] = '|';
		sb[Y_POS - 3][tap2xpos] = '+';
	}

	sb[Y_POS - 3][X_POS - 1] = '-';
	sb[Y_POS - 3][X_POS - 2] = '+';
	sb[Y_POS - 2][X_POS - 2] = '|';
	sb[Y_POS - 1][X_POS - 3] = '(';
	sb[Y_POS - 1][X_POS - 1] = ')';
	sb[Y_POS][X_POS - 2] = '|';
	sb[Y_POS + 1][X_POS - 2] = '+';
	sb[Y_POS + 1][X_POS - 1] = '-';
}

void updateScreen(char randBit, char hidden){
	// LSB on the right
	if(!hidden)
		for(int i = 0; i < lfsr_len; ++i)
			sb[Y_POS + 1][X_POS + (lfsr_len - i) * 2 - 1] =
				'0' + ((lfsr >> i) & 1);
	if(!hidden){
		sb[Y_POS - 3][X_POS + 2] = '0' + ((lfsr >> tap2 - 1) & 1);
		sb[Y_POS - 3][X_POS + 6] = '0' + ((lfsr >> tap1 - 1) & 1);
		sb[Y_POS - 1][X_POS - 2] = '0'
			+ (((lfsr >> tap1 - 1) ^ (lfsr >> tap2 - 1)) & 1);
	}
	if(hidden){
		sb[Y_POS - 3][X_POS + 2] = '?';
		sb[Y_POS - 3][X_POS + 6] = '?';
		sb[Y_POS - 1][X_POS - 2] = '?';
		for(int i = 0; i < lfsr_len; ++i)
			sb[Y_POS + 1][X_POS + (lfsr_len - i) * 2 - 1] = '?';
	}

	for(int i = SCR_W - 1; i > X_POS + lfsr_len * 2 + 1; --i)
		sb[Y_POS + 1][i] = sb[Y_POS + 1][i - 1];
	if(randBit != -1)
		sb[Y_POS + 1][X_POS + lfsr_len * 2 + 1] = '0' + randBit;
}

unsigned int getRandBit(){
	unsigned int ret = lfsr & 1;
	unsigned int newbit = ((lfsr >> tap1) ^ (lfsr >> tap2)) & 1;
	lfsr = (lfsr >> 1) | (newbit << lfsr_len);
	return ret;
}

int main(){
	lfsr = 0b0001100;
	char hidden = 0, speed = 10;
	initScreen();
	updateScreen(-1, hidden);
	drawScreen();
	for(unsigned long i = 0;; ++i){
		if(speed <= 0)
			getchar();
		else
			usleep(100000 * (11 - speed));
		char randBit = getRandBit();
		updateScreen(randBit, hidden);
		drawScreen();
	}
}
