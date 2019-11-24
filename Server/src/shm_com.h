// shm_com.h
//
// Daniel Dreise (c) All Rights Reserved
// November 23, 2019

#ifndef SHM_COM_H
#define SHM_COM_H

#define SHM_SIZE 1024

// Structure for holding shared data
struct shared_use_st {
    char data[SHM_SIZE];
};

#endif
