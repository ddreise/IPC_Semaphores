// Signal.c
//
// Signal processing to properly quit server
//
// Daniel Dreise - All Rights Reserved
// November 22, 2019

#include <stdio.h>
#include <signal.h>

int signalFlag = 0;

void signalHandler(int signalnum){

    signalFlag = 1;

	printf("\nQuitting program...\n");

}

