#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define SCR_W 60
#define SCR_H 13

#define X_POS 5
#define Y_POS 5

int lfsr_len = 7;
unsigned int lfsr = 1337;
unsigned int tap1 = 2;
unsigned int tap2 = 3;

char sb[SCR_H][SCR_W];
char hidden = 0, speed = 1;

void drawScreen(){
	putchar('\n');
	for(int i = 0; i < SCR_H; ++i){
		for(int j = 0; j < SCR_W; ++j){
			if(sb[i][j] == 0)
				sb[i][j] = ' ';
			putchar(sb[i][j]);
		}
		putchar('\n');
	}
}

void initScreen(){
	memset(sb, ' ', SCR_H * SCR_W);
	sb[0][0] = '#';
	sb[SCR_H - 1][0] = '#';
	sb[SCR_H - 1][SCR_W - 1] = '#';
	sb[0][SCR_W - 1] = '#';
	for(int i = 1; i < SCR_W - 1; ++i)
		sb[0][i] = '=';
	for(int i = 1; i < SCR_W - 1; ++i)
		sb[SCR_H - 1][i] = '=';
	for(int i = 1; i < SCR_H - 1; ++i)
		sb[i][0] = '#';
	for(int i = 1; i < SCR_H - 1; ++i)
		sb[i][SCR_W - 1] = '#';
	for(int i = 1; i < SCR_W - 1; ++i)
		sb[Y_POS + 4][i] = '-';

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
	sb[Y_POS + 3][X_POS + lfsr_len * 2] = '|';
	sb[Y_POS + 4][X_POS + lfsr_len * 2] = '^';
	sprintf(sb[Y_POS + 3] + X_POS + 1, "Register");
	sb[Y_POS + 3][X_POS + 9] = ' ';

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

	sprintf(sb[Y_POS - 3] + X_POS + 3, "XOR");

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

void updateScreen(char randBit, unsigned long step){
	sprintf(sb[Y_POS + 5] + 2, "Step: %lu", step);
	sprintf(sb[Y_POS + 6] + 2, "State: %u", lfsr);

	sprintf(sb[Y_POS + 5] + 30, "Tap1: %lu", tap1);
	sprintf(sb[Y_POS + 6] + 30, "Tap2: %lu", tap2);

	if(speed == 0)
		sprintf(sb[Y_POS + 5] + 40, "Enter - step");
	else
		sprintf(sb[Y_POS + 5] + 40, "Speed: %d", speed);
	sprintf(sb[Y_POS + 6] + 40, "Ctrl + C - quit");
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
	puts("1 - Start  2 - Settings  3 - Info");
	char menuin = getchar();
	getchar(); // eat the newline
	if(menuin == '1'){
		lfsr = 0b0001100;
	}
	else if(menuin == '3'){
		puts("LFSR Toy by CyanBun96 2024");
		puts("for custom pwn.college dojo");
		puts("The House Always Wins");
		return 0;
	}
	else if(menuin == '2'){
		puts("Register length (2-26):");
		scanf("%d", &lfsr_len);
		puts("Tap 1 position (1-Register length):");
		scanf("%d", &tap1);
		puts("Tap 2 position (1-Register length):");
		scanf("%d", &tap2);
		puts("Initial state in binary (e.g. 1010111):");
		scanf("%b", &lfsr);
		puts("Speed (0-11, 0 - manual control):");
		scanf("%d", &speed);
		puts("Hide state (1 - yes, 0 - no):");
		scanf("%d", &hidden);
		getchar(); //eat the newline
	}
	else{
		puts("Invalid input!");
		return 1;
	}
	initScreen();
	updateScreen(-1, 0);
	drawScreen();
	for(unsigned long i = 1;; ++i){
		if(speed <= 0)
			getchar();
		else
			usleep(100000 * (11 - speed));
		char randBit = getRandBit();
		updateScreen(randBit, i);
		drawScreen();
	}
}
