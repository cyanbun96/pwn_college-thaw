#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int lfsr_len = 7;
unsigned int lfsr = 1337;
unsigned int tap1 = 2;
unsigned int tap2 = 3;

unsigned int getRandBit(){
	unsigned int ret = lfsr & 1;
	unsigned int newbit = ((lfsr >> tap1) ^ (lfsr >> tap2)) & 1;
	lfsr = (lfsr >> 1) | (newbit << lfsr_len);
	return ret;
}

unsigned int getSeed(){
	unsigned int ret = 0;
	char buf[20];
	memset(buf, 0, 20);
	int fd = open("/flag", O_RDONLY);
	if(fd < 1){
		puts("Can't open /flag");
		exit(3);
	}
	read(fd, buf, 19);
	for(int i = 0; buf[i]; ++i)
		ret += (buf[i] << i % 3) * ((31337 << (i % 5)) ^ i);
	unsigned int mask = 0;
	for(int i = 0; i < lfsr_len; ++i) // gotta be a better way
		mask = ((mask << 1) | 1); // don't care if doesn't work better
	ret &= mask; // limiting the lfsr initial state to fit in lfsr_len
	return ret;
}

void getFlag(){
	char buf[64] = {0};
	int fd = open("/flag", O_RDONLY);
	read(fd, buf, 63);
	puts(buf);
	getchar();
	exit(0);
}

int main(){
	puts("Win conditions: streak == 777, wins + losses <= 2000");
	unsigned long wins = 0, losses = 0, streak = 0;
	lfsr = getSeed();
	/*for(int j = 0; j < 1000; ++j){ //debug stuf pleas ignore
		printf("%d\t%u\t", j, lfsr);
		for(int i = 0; i < lfsr_len; ++i)
			putchar('0' + ((lfsr >> i) & 1));
		printf(" %u\n", getRandBit());
	}*/
	for(;;){
		char cpu_choice = getRandBit() + '0';
		printf("Wins: %lu  Losses: %lu  Streak: %lu\n", wins, losses, streak);
		puts("Input (0/1):");
		char player_choice = getchar();
		getchar(); // eat the newline
		if(player_choice != '1' && player_choice != '0'){
			puts("Unrecognized input!");
			exit(1);
		}
		printf("Player > %c vs %c < CPU\n", player_choice, cpu_choice);
		if(cpu_choice != player_choice){
			puts("CPU wins!");
			losses++;
			streak = 0;
		}
		else if(cpu_choice == player_choice){
			puts("Player wins!");
			wins++;
			streak++;
		}
		else
			break; // literally
		if(streak == 777)
			getFlag();
		if(wins + losses >= 2000){
			puts("2000 rounds played.");
			puts("You lose.");
			return 0;
		}
	}
	return 2; // should never be reached
}
