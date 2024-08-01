#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// xoroshiro128+ code taken from
// http://vigna.di.unimi.it/xorshift/xoroshiro128plus.c

unsigned long s[2]; // the state array

static inline unsigned long rotl(const unsigned long x, int k) {
	return (x << k) | (x >> (64 - k));
}

unsigned long next(void) {
	const unsigned long s0 = s[0];
	unsigned long s1 = s[1];
	const unsigned long result = s0 + s1;

	s1 ^= s0;
	s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
	s[1] = rotl(s1, 36); // c
	return result;
}

// end of xoroshiro128+ code

unsigned int randBitsLeft = 0;
unsigned long randBuf = 0;

unsigned int getRandBit(){
	if(!randBitsLeft){
		randBuf = next();
		randBitsLeft = 64;
	}	
	randBitsLeft--;
	unsigned int retn = randBuf & 1;
	randBuf >>= 1;
	return retn;
}

void getFlag(){
	char buf[64] = {0};
	int fd = open("/flag", O_RDONLY);
	if(!fd || !read(fd, buf, 63)){
		puts("Can't open /flag.");
		puts("consolation flag: pwn.college{gudjob}");
		getchar();
		exit(7);
	}
	puts(buf);
	getchar();
	exit(0);
}

int main(){
	if(getentropy(&s, sizeof(long) * 2) < 0)
		exit(2);
	puts("t99e o7di :]");
	unsigned long wins = 0, losses = 0, streak = 0;
	for(;;){
		char cpu_choice = getRandBit() + '0';
		char player_choice = getchar();
		getchar(); // eat the newline
		if(player_choice != '1' && player_choice != '0'){
			puts("Unrecognized input.");
			exit(1);
		}
		if(cpu_choice != player_choice){
			puts("CPU.");
			losses++;
			streak = 0;
		}
		else if(cpu_choice == player_choice){
			puts("You.");
			wins++;
			streak++;
		}
		else
			break; // literally
		if(streak == 777)
			getFlag();
		if(wins + losses >= 1000){
			puts("1000 rounds played.");
			puts("You lose.");
			return 0;
		}
	}
	return 2; // should never be reached
}
