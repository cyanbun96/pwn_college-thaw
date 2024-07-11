#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

unsigned int randBitsLeft = 0;
unsigned int randBuf = 0;

unsigned int state[32];
unsigned int fptr = 3;
unsigned int rptr = 0;

unsigned int rand_laf(){
	unsigned int val = state[fptr] + state[rptr];
	state[fptr] = val;
	++fptr;
	if (fptr >= 32){
		fptr = 0;
		++rptr;
	}
	else{
		++rptr;
		if (rptr >= 32)
			rptr = 0;
	}
	return val;
}

unsigned int getRandBit(){
	if(!randBitsLeft){
		randBuf = rand_laf();
		randBitsLeft = sizeof(int) * 8;
	}	
	randBitsLeft--;
	unsigned int retn = randBuf & 1;
	randBuf >>= 1;
	return retn;
}

unsigned int getSeed(){
	if(getentropy(&state, 4*32) != 0)
		exit(1);
	return 0;
}

void getFlag(){
	char buf[64] = {0};
	int fd = open("/flag", O_RDONLY);
	if(!fd){
		puts("You win! Can't open /flag though, sorry...");
		puts("consolation flag: pwn.college{gudjob}");
		exit(7);
	}
	read(fd, buf, 63);
	puts(buf);
	getchar();
	exit(0);
}

int main(){
	puts("Welcome to \"The House Always Wins\"!");
	puts("The CPU flips a coin, you guess the result.");
	puts("0 - heads, 1 - tails.");
	puts("Read the text file in /challenge/ for hints!");
	puts("Win conditions: streak == 10, wins + losses <= 2000");
	unsigned long wins = 0, losses = 0, streak = 0;
	getSeed();
	for(;;){
		char cpu_choice = getRandBit() + '0';
		printf("Wins: %lu  Losses: %lu  Streak: %lu\n", wins, losses, streak);
		puts("Input (1/0):");
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
		if(streak == 10)
			getFlag();
		if(wins + losses >= 2000){
			puts("2000 rounds played.");
			puts("You lose.");
			return 0;
		}
	}
	return 2; // should never be reached
}
