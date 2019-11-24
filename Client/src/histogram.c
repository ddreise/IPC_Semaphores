// histogram.c
//
// Daniel Dreise (c) November 23, 2019

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_VALUES 20

void histogram(char *data, int length){

    int i = 0, j = 0;
    int results[NUM_VALUES] = {0};

	// Loop through the whole shared memory and increment values of array according to letter found
    for (i = 0; i < length; i++){
        results[(data[i] - 65)]++;
    }

	// Clear terminal
    system("clear");

	// Print out histogram
    for (i = 0; i < NUM_VALUES; i++){
        printf("%c ", i + 65);
        for (j = 0; j < results[i]; j++){
            printf("*");
        }
        printf("\n");
        
    }

}
