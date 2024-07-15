#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

unsigned int randBitsLeft = 0;
unsigned int randBuf = 0;
unsigned int state = 0;

unsigned int rand0(unsigned int *state){
	unsigned int ret = ((*state * 1103515245) + 12345) & 0x7fffffff;
	*state = ret;
	return ret;
}

unsigned int getRandBit(){
	if(!randBitsLeft){
		randBuf = rand0(&state);
		randBitsLeft = sizeof(int) * 8;
	}
	randBitsLeft--;
	unsigned int retn = randBuf & 1;
	randBuf >>= 1;
	return retn;
}

unsigned int getSeed(){
	unsigned int ret = 0;
	if(getentropy(&ret, 4) != 0)
		exit(1);
	return ret;
}

void getFlag(){
	char buf[64] = {0};
	int fd = open("/flag", O_RDONLY);
	if(!fd || !read(fd, buf, 63)){
		puts("You win! Can't open /flag though, sorry...");
		puts("consolation flag: pwn.college{gudjob}");
		getchar();
		exit(7);
	}
	puts(buf);
	getchar();
	exit(0);
}

int main(){
	puts("...");
	unsigned long wins = 0, losses = 0, streak = 0;
	state = getSeed();
	if(!state) // seeds 0 and 1 give the same results in glibc rand type 0
		state = 1;
	for(;;){
		char cpu_choice = getRandBit() + '0';
		//printf("Wins: %lu  Losses: %lu  Streak: %lu\n", wins, losses, streak);
		//puts("Input (0/1):");
		char player_choice = getchar();
		getchar(); // eat the newline
		if(player_choice != '1' && player_choice != '0'){
			puts("Unrecognized input!");
			exit(1);
		}
		//printf("Player > %c vs %c < CPU\n", player_choice, cpu_choice);
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
