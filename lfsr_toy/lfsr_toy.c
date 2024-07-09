#include <stdio.h>
#include <unistd.h>

#define SCR_W 60
#define SCR_H 13

#define X_POS 5
#define Y_POS 5

int lfsr_len = 7;
unsigned int mask = 0;
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
	// Borders
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

	// Register
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

	// XOR
	sb[Y_POS - 5][X_POS] = 'T';
	sb[Y_POS - 5][X_POS + 8] = 'T';
	sb[Y_POS - 4][X_POS] = '+';
	sb[Y_POS - 3][X_POS] = '<';
	for(int i = 1; i < 8; ++i){
		sb[Y_POS - 4][X_POS + i] = '-';
	}
	sb[Y_POS - 4][X_POS + 8] = '+';
	sb[Y_POS - 3][X_POS + 8] = '<';
	sb[Y_POS - 2][X_POS] = '+';
	for(int i = 1; i < 8; ++i){
		sb[Y_POS - 2][X_POS + i] = '-';
	}
	sb[Y_POS - 2][X_POS + 8] = '+';
	sprintf(sb[Y_POS - 3] + X_POS + 3, "XOR");

	// Taps and input pipes
	for(int i = 0; i < lfsr_len * 2 - 9; ++i){
		sb[Y_POS - 3][X_POS + 9 + i] = '=';
	}
	sb[Y_POS - 3][X_POS + lfsr_len * 2] = '#';
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

	// XOR output display
	sb[Y_POS - 3][X_POS - 1] = '-';
	sb[Y_POS - 3][X_POS - 2] = '+';
	sb[Y_POS - 2][X_POS - 2] = '|';
	sb[Y_POS - 1][X_POS - 3] = '(';
	sb[Y_POS - 1][X_POS - 1] = ')';
	sb[Y_POS][X_POS - 2] = '|';
	sb[Y_POS + 1][X_POS - 2] = '+';
	sb[Y_POS + 1][X_POS - 1] = '-';

	// Init displays to "hidden" values
	for(int i = 0; i < lfsr_len; ++i)
		sb[Y_POS + 1][X_POS + (lfsr_len - i) * 2 - 1] = '?';
	sb[Y_POS - 3][X_POS + 2] = '?';
	sb[Y_POS - 3][X_POS + 6] = '?';
	sb[Y_POS - 1][X_POS - 2] = '?';
}

void updateScreen(char randBit, unsigned long step){
	sprintf(sb[Y_POS + 5] + 2, "Step: %lu", step);
	sprintf(sb[Y_POS + 6] + 2, "State: %u", (lfsr & mask));

	sprintf(sb[Y_POS + 5] + 30, "Tap1: %u", tap1);
	sprintf(sb[Y_POS + 6] + 30, "Tap2: %u", tap2);

	if(speed == 0)
		sprintf(sb[Y_POS + 5] + 40, "Enter - step");
	else
		sprintf(sb[Y_POS + 5] + 40, "Speed: %d", speed);
	sprintf(sb[Y_POS + 6] + 40, "Ctrl + C - quit");

	// Binary LFSR state, LSB on the right
	if(!hidden)
		for(int i = 0; i < lfsr_len; ++i)
			sb[Y_POS + 1][X_POS + (lfsr_len - i) * 2 - 1] =
				'0' + ((lfsr >> i) & 1);
	
	// XOR display
	if(!hidden){
		sb[Y_POS - 3][X_POS + 2] = '0' + ((lfsr >> (tap2 - 1)) & 1);
		sb[Y_POS - 3][X_POS + 6] = '0' + ((lfsr >> (tap1 - 1)) & 1);
		sb[Y_POS - 1][X_POS - 2] = '0'
			+ (((lfsr >> (tap1 - 1)) ^ (lfsr >> (tap2 - 1))) & 1);
	}
	
	// LFSR output going offscreen
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
		lfsr_len = 7;
		lfsr = 0b0001100;
		tap1 = 2;
		tap2 = 3;
	}
	else if(menuin == '3'){
		puts("LFSR Toy v1.0 by CyanBun96, 2024");
		puts("For custom pwn.college dojo");
		puts("The House Always Wins");
		puts("License: GPL-3.0");
		puts("Source repo:");
		puts("https://github.com/cyanbun96/pwn_college-thaw");
		return 0;
	}
	else if(menuin == '2'){
		// no checks, have fun :)
		puts("Register length (2-26):");
		scanf("%d", &lfsr_len);
		puts("Tap 1 position (1-Register length):");
		scanf("%d", &tap1);
		puts("Tap 2 position (1-Register length):");
		scanf("%d", &tap2);
		puts("Initial state in binary (e.g. 1010111):");
		scanf("%b", &lfsr);
		puts("Speed (0-11, 0 - manual control):");
		scanf("%hhd", &speed);
		puts("Hide internal state (1 - yes, 0 - no):");
		scanf("%hhd", &hidden);
		getchar(); //eat the newline
	}
	else{
		puts("Invalid input!");
		return 1;
	}
	for(int i = 0; i < lfsr_len; ++i){
		mask <<= 1;
		mask |= 1;
	}
	lfsr = lfsr & mask;
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
