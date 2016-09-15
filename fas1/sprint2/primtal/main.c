#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	int T = 10;

	if(argc >= 2) {
		T = atoi(argv[1]);
	}
	
	bool primes[T];

	for(int i = 0; i < T; i++) {
		primes[i] = true;
	}

	for(int i = 1; i < T; i++) {
		if(!primes[i]) continue;

		for(int j = i * 2 + 1; j < T; j += i + 1) {
			primes[j] = false;
		}
	}

	for(int i = 0; i < T; i++) {
		if(primes[i]) {
			printf("%d\n", i + 1);
		}
	}
}
