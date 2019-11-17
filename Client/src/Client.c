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
#include <unistd.h>
#include <time.h>

#define SHM_SIZE 1024			// Size of shared memory
#define USLEEP_DELAY 250000		// sleep delay in microseconds

int main(int argc, char *argv[]) {

	key_t key;			// Key variable
	int shmid;			// Shared memory ID
	char* data;			// shared memory pointer
	char buf[20];	// Buffer to hold frequency of letters
	char temp;

	enum letters {
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, T
	} letter;
	
	// Make the "key" identifier to give to each process so they can access shared memory
	if ((key = ftok(".", 'A')) == -1) {
		perror("ftok failed\n");
		exit(1);
	}

	// Connect to shared memory. If not created, create one (IPC_CREAT)
	if ((shmid = shmget(key, SHM_SIZE, 0444)) == -1){		// 0444 - client can only rad
		
		// If no shared memory exists, relaunch server application
		system("cd ..");
		system("./Server/bin/Server &");
		sleep(1);
		if ((shmid = shmget(key, SHM_SIZE, 0444)) == -1){
			perror("client/server failed\n");
			exit(2);
		}
	}

	// Attach to the recently created segment
	// shmat attaches to shared memory segment identified by shmid to the address space of the calling process
	data = shmat(shmid, (void *)0, 0);			
	if (data == (char *) (-1)){
		perror("shmat failed\n");
		exit (3);
	}

	/*********** MAIN FUNCTION OF CLIENT *************/

	// Read from shared memory
	while(1){

		 for (int i = 0; i < SHM_SIZE; i++){

		// 	letter = data[i];
		// 	letter++;

			printf("%c", data[i]);

		 }

	}

	/*********** END OF CLIENT FUNCTION **************/

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