#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// Mersenne Twister, aka MT19937
// The entire algorithm was taken from the Wikipedia article
// It works the same, but has been reformatted and commented
// to make it easier to understand without a PhD in maths 

// State array size
unsigned int n = 624;

// State initialization constants
unsigned int w = 32; // word size in bits, sizeof(int) * 4
unsigned int f = 1812433253; // Knuth TAOCP Vol2. 3rd Ed. P.106 (tl;dr magic)

// Main algorithm constants
unsigned int m = 397; // distance between combined state elements, 1 <= m < n
unsigned int UMASK = 0x80000000; // 0xffffffff << r ; r = w - 1
unsigned int LMASK = 0x7fffffff; // (0xffffffffUL >> (w - r)
unsigned int a = 0x9908b0df; // magic number for extra randomness

// Tempering constants
unsigned int u = 11; // bitshift 1
unsigned int s = 7;  // bitshift 2
unsigned int t = 15; // bitshift 3
unsigned int l = 18; // bitshift 4
unsigned int b = 0x9d2c5680; // bitmask 1
unsigned int c = 0xefc60000; // bitmask 2

typedef struct{
	unsigned int state_array[624];
	int state_index;
} mt_state;

mt_state glob_state;

unsigned int temper(unsigned int x){
	unsigned int y, z;
	y = x ^ (x >> u);
	y = y ^ ((y << s) & b);
	y = y ^ ((y << t) & c);
	z = y ^ (y >> l);
	return z;
}

void initialize_state(mt_state* state, unsigned int seed){
	unsigned int* state_array = &(state->state_array[0]);
	state_array[0] = seed;
	for (unsigned int i=1; i<n; i++){
		seed = f * (seed ^ (seed >> (w-2))) + i;
		state_array[i] = seed; 
	}
	state->state_index = 0;
}

unsigned int mt19937_rand(mt_state* state){
	unsigned int* state_array = state->state_array;
	int k = state->state_index; // point to current state location
	int j = (k + 1) % n; // second pointer, next element
	unsigned int x = (state_array[k] & UMASK) | (state_array[j] & LMASK);
	unsigned int xA = x >> 1;
	if(x & 1) xA ^= a; // extra randomness
	j = (k + m) % n;
	x = state_array[j] ^ xA;
	state->state_index = (k + 1) % n;
	state_array[k] = x; // the only change in the state array
	return temper(x); // temper x before returning it
}

// ==============================================================
//  ------------------------------------------------------------
//   End of MT19937-related code, generic challenge code ahead
//  ------------------------------------------------------------
// ==============================================================

unsigned int randBitsLeft = 0;
unsigned int randBuf = 0;

unsigned int getRandBit(){
	if(!randBitsLeft){
		randBuf = mt19937_rand(&glob_state);
		randBitsLeft = 32;
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
	unsigned int seed = 0;
	if(getentropy(&seed, sizeof(int)) < 0)
		exit(2);
	initialize_state(&glob_state, seed);
	puts("30000 rounds, 777 wins in a row. Go.");
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
		if(wins + losses >= 30000){
			puts("30000 rounds played.");
			puts("You lose.");
			return 0;
		}
	}
	return 2; // should never be reached
}
