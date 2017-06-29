#include <stdio.h>
#include <signal.h>

char bKeepLooping = 1;

void intHandler(int dummy) {
	bKeepLooping = 0;
}

int main( int argc, char *argv[]){

	signal(SIGINT, intHandler);
	while (bKeepLooping) {
	
	}
	printf("Exited successfully\n");
	return 0;
}
