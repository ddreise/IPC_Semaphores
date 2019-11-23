


#ifndef USEFUL_FUNCTIONS
#define USEFUL_FUNCTIONS


struct circ_buff{

    int8_t *data;
    int16_t head;
    int16_t tail;
    int16_t length;

};

//void circ_buff_push (circ_buff *data_struct, uint8_t *data);

#endif