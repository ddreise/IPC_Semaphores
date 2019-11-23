// histogram.c
//
// Daniel Dreise (c) November 23, 2019

#include <stdio.h>

#define NUM_VALUES 20

void histogram(char *data, int length){

    int i = 0, j = 0;
    int results[NUM_VALUES] = {0};


    for (i = 0; i < length; i++){
        results[(data[i] - 65)]++;
    }

    for (i = 0; i < NUM_VALUES; i++){
        printf("%d ", i);
        for (j = 0; j < results[i]; j++){
            printf("*");
        }
        printf("\n");
        
    }

}