// Client.c
//
// Operating Systems - Assignment 4
//
// Purpose: Server/client relationship where server makes 1024 bytes of shared memory
//			and fills with 32 random letters between A and T every 0.25 seconds.
//			The client retreives data from shared memory and creates a histogram that
//			indicates the frequency of each letter.
//
// 
// Daniel Dreise - All Rights Reserved
// November 16, 2019
//
// Code sampled from:
// 		cprogramming.com - How to generate a random chars in C programming - YayIguess




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include "Signal.h"

#define SHM_SIZE 1024			// Size of shared memory
#define USLEEP_DELAY 250000		// sleep delay in microseconds

int main(int argc, char *argv[]) {

	key_t shmkey, semkey;			// Key variable
	int shmid, semid;			// Shared memory ID
	void* shared_memory = (void *) 0;
	char* data;			// shared memory pointer
	char buf[20];		// Buffer to hold frequency of letters
	char temp;
	unsigned short init_values[1] = { 1 };

	enum letters {
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, T
	} letter;
	
	struct sembuf seminc = {
		.sem_num = 0,
		.sem_op = 1,
		.sem_flg = SEM_UNDO
	};

	// Operation for decrementing semaphore
	struct sembuf semdec = {
		.sem_num = 0,
		.sem_op = -1,
		.sem_flg = SEM_UNDO
	};

	// Make the "key" identifier to give to each process so they can access shared memory
	// if ((shmkey = ftok(".", 'A')) == -1) {
	// 	perror("ftok failed\n");
	// 	exit(1);
	// }

	// Connect to shared memory. If not created, create one (IPC_CREAT)
	if ((shmid = shmget((key_t)1234, SHM_SIZE, 0640 | IPC_CREAT)) == -1){		// 0444 - client can only rad
		
		// If no shared memory exists, relaunch server application
		//system("cd ..");
		//system("./Server/bin/Server &");
		//sleep(1);
		//if ((shmid = shmget(shmkey, SHM_SIZE, 0640)) == -1){
			perror("client/server failed\n");
			exit(2);
		//}
	}

	// Attach to the recently created segment
	// shmat attaches to shared memory segment identified by shmid to the address space of the calling process
	shared_memory = shmat(shmid, (void *)0, 0);			
	if (shared_memory == (void *) (-1)){
		perror("shmat failed\n");
		exit (3);
	}
	printf("Memory attached at %d\n", (int)shared_memory);

	// Make the semaphore key identifier
	// if ((semkey = ftok(".", 'S')) == -1) {
	// 	perror("ftok for semaphore failed\n");
	// 	exit(4);
	// }

	// Create a semaphore set
    if ((semid = semget( (key_t)1234, 1, 0666 | IPC_CREAT )) == -1){		// rmvd flag IPC_EXCL
        printf("semget failed\n");
        exit(5);
    }
	printf("Client semid: %d\n", semid);

	if ((semctl (semid, 0, SETALL, init_values)) == -1) {
		printf ("semaphore init failed\n");
		exit (6);
	}


	/*********** MAIN FUNCTION OF CLIENT *************/

	// Read from shared memory
	while(!signalFlag){

		if (semop(semid, &semdec, 1) == -1){
			printf("Semaphore decrement failed\n");
			exit(7);
		}

		 for (int i = 0; i < SHM_SIZE; i++){

		// 	letter = data[i];
		// 	letter++;

			//printf("%c", data[i]);

		 }

		if (semop(semid, &seminc, 1) == -1){
			printf("Semaphore increment failed\n");
			exit(8);
		}

	}

	/*********** END OF CLIENT FUNCTION **************/

	if (semctl(semid, 1, IPC_RMID) == -1){
		printf("semctl() remove id failed\n");
		exit(9);
	}


	// Detach from segment if finished
	if (shmdt(data) == -1){
		perror("shmdt failed\n");
		exit(4);
	}

	// Delete shared memory segment
	if((shmctl(shmid, IPC_RMID, NULL)) == -1){
		perror("shdctl Delete failed!\n");
		exit(5);
	}



	return(0);
}