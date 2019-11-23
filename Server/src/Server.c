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
#include "Useful_Functions.h"

#define SHM_SIZE 1024			// Size of shared memory
#define USLEEP_DELAY 250000		// sleep delay in microseconds

int main(int argc, char *argv[]) {

	key_t shmkey, semkey;						// Key variable
	int shmid, semid;							// Shared memory ID, semaphore ID
	void *shared_memory = (void *) 0;
	char *data;
	char buffer[32];							// Buffer to hold 32 characters temporarily
	unsigned short sem_value = 0;
	unsigned short init_values[1] = { 1 };

	struct sembuf seminc = {
		.sem_num = 0,
		.sem_op = 1,
		.sem_flg = SEM_UNDO
	};
	
	struct sembuf semdec = {
		.sem_num = 0,
		.sem_op = -1,
		.sem_flg = SEM_UNDO
	};


	signal (SIGINT, signalHandler);



	srand(time(NULL));	// For random letter generation
	
	// Make the shared memory"key" identifier to give to each process so they can access shared memory
	// if ((shmkey = ftok(".", 'M')) == -1) {
	// 	perror("ftok for shared memory failed\n");
	// 	exit(1);
	// }

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
	printf("Memory attached at %X\n", (int)shared_memory);


	// Make the semaphore key identifier
	// if ((semkey = ftok(".", 'S')) == -1) {
	// 	perror("ftok for semaphore failed\n");
	// 	exit(4);
	// }

	// Create a semaphore set
    if ((semid = semget( (key_t)1234, 1, 0640 | IPC_CREAT )) == -1){		// rmvd flag IPC_EXCL
        printf("semget failed\n");
        exit(5);
    }

	// Operation for incrementing semaphore

	if (semop(semid, &seminc, 1) == -1) {
		perror("semaphore_p failed\n");
		exit(1);
	}

	// Operation for decrementing semaphore

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



	/*********** MAIN FUNCTION OF SERVER *************/

	// Generate random letters and print to shared memory
	while(!signalFlag){
		
		usleep(USLEEP_DELAY);		// Sleep for USLEEP_DELAY microseconds

		for (int i = 0; i < 32; i++){
			buffer[i] = (rand() % (84-65)) + 65; 	//65 is ASCII for capital A, 84 is ASCII for capital T
		}

		// BEGIN CRITICAL SECTION
		if (semop(semid, &semdec, 1) == -1){
			printf("Semaphore increment failed\n");
			exit(7);
		}

		//strcat(data, buffer);		// Print buffer to shared memory location
		//printf("printed to shared memory...\n");


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
	if (shmdt(data) == -1){
		perror("shmdt failed\n");
		exit(10);
	}

	// Delete shared memory segment
	if((shmctl(shmid, IPC_RMID, NULL)) == -1){
		perror("shdctl Delete failed!\n");
		exit(11);
	}


	return(0);
}

