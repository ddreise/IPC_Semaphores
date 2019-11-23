// Useful_Functions.c

// General Ring Buffer Structure

#include <stdio.h>
#include <stdlib.h>
#include "Useful_Functions.h"

/* void circ_buff_push (circ_buff *data_struct, uint8_t *data){

    int next;

    next = data_struct->head + 1;
    if (next >= data_struct->length) next = 0;

    if (next == data_struct->tail) return -1;

    data_struct->data[data_struct->head] = data;
    data_struct->head = next;

    return 0;
} */