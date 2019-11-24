
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
#include "shm_com.h"
#include "histogram.h"

#define SHM_SIZE 1024			// Size of shared memory
#define USLEEP_DELAY 250000		// sleep delay in microseconds

int main(int argc, char *argv[]) {

	key_t shmkey, semkey;			// Key variable
	int shmid, semid;			// Shared memory ID
	void *shared_memory = (void *) 0;	// Shared memory pointer
	struct shared_use_st *shared_data;	// Shared memory data structure
	char buf[20];				// Buffer to hold frequency of letters
	char temp;
	unsigned short init_values[1] = { 1 };
	
	// Structure for incrementing semaphore
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

	// Initialize signals
	signal (SIGINT, signalHandler);

	// Connect to shared memory. If not created, create one (IPC_CREAT)
	if ((shmid = shmget((key_t)1234, SHM_SIZE, 0640)) == -1){		// 0444 - client can only rad	

		// If unable to connect to shared memory, fork a process that will launch server application
		switch (fork()){
			case 0:

				execl("./Server", "Server", NULL);
				printf("Return not expected, must be error with execl()\n");
				exit(5);
				break;

			case -1:

				printf("Couldn't fork\n");
				exit(6);
				break;

		}
	}

	// Wait one second for server to launch
	sleep(1);

	// Try reconnecting. If failed again, exit program.
	if ((shmid = shmget((key_t)1234, SHM_SIZE, 0640)) == -1){
		perror("Client/Server non-functional\n");
		exit(2);
	}
	
	// Attach to the recently created segment
	// shmat attaches to shared memory segment identified by shmid to the address space of the calling process
	shared_memory = shmat(shmid, (void *)0, 0);			
	if (shared_memory == (void *) (-1)){
		perror("shmat failed\n");
		exit (3);
	}

	printf("Client shmid: %d\n", shmid);

	// Create a semaphore set
    if ((semid = semget( (key_t)1234, 1, 0640 | IPC_CREAT )) == -1){		// rmvd flag IPC_EXCL
        printf("semget failed\n");
        exit(5);
    }
	printf("Client semid: %d\n", semid);

	// Initialize semaphore
	if ((semctl (semid, 0, SETALL, init_values)) == -1) {
		printf ("semaphore init failed\n");
		exit (6);
	}


	shared_data = (struct shared_use_st *) shared_memory;

	/*********** MAIN FUNCTION OF CLIENT *************/

	// Read from shared memory
	while(!signalFlag){

		if(signalFlag) break;	// If somehow still makes it into loop with sigflag, break from it.
		// Sleep for 2 seconds
		sleep(2);

		// Clear the terminal
		system("clear");

		/******* CRITICAL REGION ******/
		// Decrement semaphore, restict others from accessing shared memory
		if (semop(semid, &semdec, 1) == -1){
			printf("Semaphore decrement failed\n");
			exit(7);
		}

		// Print out histogram of data in shared memory
		histogram(shared_data->data, SHM_SIZE);

		// Increment semphore, allow others to access shared memory
		if (semop(semid, &seminc, 1) == -1){
			printf("Semaphore increment failed\n");
			exit(8);
		}
		/****** END CRITICAL REGION ******/

	}

	/*********** END OF CLIENT FUNCTION **************/

	// Remove semaphore id
	if (semctl(semid, 1, IPC_RMID) == -1){
		printf("semctl() remove id failed\n");
		exit(9);
	}


	// Detach from segment if finished
	if (shmdt(shared_memory) == -1){
		perror("shmdt failed\n");
		exit(4);
	}

	// Delete shared memory segment
	if((shmctl(shmid, IPC_RMID, NULL)) == -1){
		perror("shdctl Delete failed!\n");
		exit(5);
	}

	printf("Done\n");

	return(0);
}
