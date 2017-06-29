#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void display_message(int a) {
    signal(SIGALRM, SIG_IGN);
    printf("copyit: still copying...\n");
    signal(SIGALRM, display_message);
    alarm(1);
}

char bKeepLooping = 1;

void intHandler(int dummy) {
	bKeepLooping = 0;
}

int main( int argc, char *argv[]){

    if(argc != 3) {
	printf("copyit: Incorrect arguments!\n");
	printf("usage: %s <sourcefile> <targetfile>\n", argv[0]);
    	exit(1);
    }
    signal(SIGINT, intHandler);
    signal(SIGALRM, display_message);
    alarm(1);
    
    // open the source file
    int file_r = open(argv[1], O_RDONLY);
    if (file_r < 0) {
	// could not open file
	printf("Unable to open %s: %s\n", argv[1], strerror(errno));
	exit(1);
    }

    // create the target file
    int file_w = open(argv[2], O_CREAT|O_WRONLY, 700);
    if (file_w < 0) {
	// could not create file
	printf("Unable to create %s: %s\n", argv[2], strerror(errno));
	exit(1);
    }

    char buffer[4096];
    int count = 40;
    int total = 0;
    while (bKeepLooping) {

	// read a bit of data from the source file
	int result = read(file_r, &buffer, count);
	while (result < 0 && errno == EINTR) {
	    result = read(file_r, &buffer, count);
	}
	if (result < 0) {
	    printf("copyit: could not read the file: %s\n", strerror(errno));
	    exit(1);
	}
	else if (result == 0) {
	    bKeepLooping = 0;
	}
	else {
	    total += result;
	}
	
	// write a bit of data to the target file	
	result = write(file_w, &buffer, count);
    	while (result < 0 && errno == EINTR) {
	    result = write(file_w, &buffer, count);
	}
	if (result < 0) {
	    printf("copyit: could not write to the file: %s\n", strerror(errno));
	    exit(1);
	}
    }

    // close both files
    close(file_r);
    close(file_w);

    // print success message
    printf("copyit: Copied %d bytes from file %s to %s\n", total, argv[1], argv[2]);
    return 0;
}
