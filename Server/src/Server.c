// Server.c
//
// Operating Systems - Assignment 4
//
// Purpose: Server/client relationship where server makes 1024 bytes of shared memory
//			and fills with 32 random letters between A and T every 0.25 seconds.
//			The client retreives data from shared memory and creates a histogram that
//			indicates the frequency of each letter.
//
//			Uses shared memory and semaphores for process synchronization
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

#define SHM_SIZE 1024			// Size of shared memory
#define USLEEP_DELAY 250000		// sleep delay in microseconds

int main(int argc, char *argv[]) {

	key_t shmkey, semkey;						// Key variable
	int shmid, semid;							// Shared memory ID, semaphore ID
	void *shared_memory = (void *) 0;			// Shared_memory pointer
	struct shared_use_st *shared_data;			// Structure for holding data
	char buffer[32];							// Buffer to hold 32 characters temporarily
	unsigned short init_values[1] = { 1 };		// For controlling semaphore using sem_ctrl
	int mem_index = 0;							// To keep track of position in shared_memory

	// Structure for incrementing semaphore
	struct sembuf seminc = {
		.sem_num = 0,
		.sem_op = 1,
		.sem_flg = SEM_UNDO
	};
	
	// Structure for decrementing semaphore
	struct sembuf semdec = {
		.sem_num = 0,
		.sem_op = -1,
		.sem_flg = SEM_UNDO
	};

	// Initialize signal
	signal (SIGINT, signalHandler);

	srand(time(NULL));	// For random letter generation

	// Connect to shared memory. If not created, create one (IPC_CREAT)
	if ((shmid = shmget((key_t)1234, SHM_SIZE, 0640 | IPC_CREAT)) == -1){		// 0640 - creator read/write
		perror("shmget failed\n");
		exit(2);
	}

	// Attach to the recently created segment
	// shmat attaches to shared memory segment identified by shmid to the address space of the calling process
	shared_memory = shmat(shmid, (void *)0, 0);			
	if (shared_memory == (void *) (-1)){
		perror("shmat failed\n");
		exit (3);
	}
	//printf("Server attached at %X\n", (unsigned int)shared_memory);
	printf("Server shmid: %d\n", shmid);


	// Create a semaphore set
    if ((semid = semget( (key_t)1234, 1, 0640 | IPC_CREAT )) == -1){		// rmvd flag IPC_EXCL
        printf("semget failed\n");
        exit(5);
    }

	// Test operation for incrementing semaphore
	if (semop(semid, &seminc, 1) == -1) {
		perror("semaphore_p failed\n");
		exit(1);
	}

	// Test operation for decrementing semaphore
	if(semop(semid, &semdec, 1) == -1){
		perror("semaphore_v failed\n");
		exit(2);
	}

	printf("Server semid: %d\n", semid);

	// Initialize semaphore
	if ((semctl (semid, 0, SETALL, init_values)) == -1) {
		printf ("semaphore init failed\n");
		exit (6);
	}

	// Set shared memory as shared data
	shared_data = (struct shared_use_st *)shared_memory;

	/*********** MAIN FUNCTION OF SERVER *************/

	// Generate random letters and print to shared memory
	while(!signalFlag){

		if (signalFlag) break;
		
		usleep(USLEEP_DELAY);		// Sleep for USLEEP_DELAY microseconds


		// BEGIN CRITICAL SECTION
		// Increment semaphore, restricting others from accessing
		if (semop(semid, &semdec, 1) == -1){
			printf("Semaphore increment failed\n");
			exit(7);
		}

		// Generate random letters and put into shared memory
		for (int i = 0; i < 32; i++){
			buffer[i] = (rand() % (85-65)) + 65; 	//65 is ASCII for capital A, 84 is ASCII for capital T
			if (mem_index == SHM_SIZE) mem_index = 0;
			shared_data->data[mem_index] = buffer[i];
			mem_index++;
		}

		// Decrement semaphore, allowing others to access
		if (semop(semid, &seminc, 1) == -1){
			printf("Semaphore decrement failed\n");
			exit(8);
		}
		// END CRITICAL SECTION

	}

	/*********** END OF SERVER FUNCTION **************/
    
	
	// Cleanup shared memory and semaphore 
    if (semctl(semid, 1, IPC_RMID) ==-1){
        printf("semctl() remove id failed\n");
        exit(9);
      }

	// Detach from segment if finished
	if (shmdt(shared_memory) == -1){
		perror("shmdt failed\n");
		exit(10);
	}

	// Delete shared memory segment
	if((shmctl(shmid, IPC_RMID, NULL)) == -1){
		perror("shdctl Delete failed!\n");
		exit(11);
	}

	printf("Done\n");


	return(0);
}

