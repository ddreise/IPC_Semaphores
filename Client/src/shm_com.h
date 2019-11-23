#ifndef SHM_COM_H
#define SHM_COM_H

#define SHM_SIZE 1024

struct shared_use_st {
    char data[SHM_SIZE];
};

#endif
