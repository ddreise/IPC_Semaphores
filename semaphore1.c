/*
 * semaphore1.c
 *
 * A simple example of semaphore control.
 * It will launch s2 and negotiate control over
 * a file with semaphores.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

char *info[10] = {
	"apple", "beef", "cat", "dog", "elephant",
	"green", "house", "igloo", "juice", "kite"
};

int
main (void)
{
	/*
	 * the following are operation structures for semaphore control
	 * the acquire will decrement the semaphore by 1
	 * and the release will increment the semaphore by 1.
	 * Both of these structs are initialized to work with the 1st
	 * semaphore we allocate.
	 */

	struct sembuf acquire_operation = { 0, -1, SEM_UNDO }; //Wait(), P()
	struct sembuf release_operation = { 0, 1, SEM_UNDO }; //Signal(), V()

	/*
	 * the initial value of the semaphore will be 1, indicating that
	 * our critical region is ready is ready for use by the first
	 * task that can successfully decrement our shared semaphore
	 */

	unsigned short init_values[1] = { 1 };

	int semid;
	char tmp[20];
	FILE *fp;
	int x;


	printf ("S1: This program will use critical regions.\n");
	printf ("S1: It will mark the critical region with a semaphore\n");
	printf ("S1: and only when instructed, will a common file between\n");
	printf ("S1: S1 and S2 be accessed.\n");

	printf ("S1: Obtaining semaphores for use ...\n");
	semid = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);

	if (semid == -1) {
		printf ("S1: grr, couldn't get semaphores!\n");
		exit (1);
	}	/* endif */

	printf ("S1: our semaphore ID is %d\n", semid);

	printf ("S1: let's init the semaphores ...\n");
	if (semctl (semid, 0, SETALL, init_values) == -1) {
		printf ("S1: grr, can't init semaphores\n");
		exit (1);
	}	/* endif */

	printf ("S1: now that we have semaphores, let's\n");
	printf ("S1: let's alternate control over a common file with S2 ...\n");

	switch (fork()) {
	case 0:
		/*
		 * child program
		 */

		sprintf (tmp, "%d", semid);
		execl ("./semaphore2.o", "semaphore2", tmp, NULL);
		
		printf ("grr, error in execl\n");
		semctl (semid, 0, IPC_RMID);
		exit (2);
		break;

	case -1:
		printf ("S1: grr, couldn't fork!\n");
		semctl (semid, 0, IPC_RMID);
		exit (3);
		break;
	}	/* end switch */

	/*
	 * only parent runs this. loop 10 times via semaphore control
	 * to place data into shared file between apps.
	 */

	for (x = 0; x < 10; x++) {
		/*
		 * do a decrement on our only semaphore
		 */

		if (semop (semid, &acquire_operation, 1) == -1) {
			printf ("S1: grr, can't start critical region\n");
			exit (4);
		}	/* endif */

		printf ("S1: we're in our critical region\n");
		if ((fp = fopen ("shared.txt", "w")) == NULL) {
			printf ("S1: grr, can't write to shared file\n");
			semctl (semid, 0, IPC_RMID);
			exit (5);
		}	/* endif */

		fprintf (fp, "%s", info[x]);
		fclose (fp);

		printf ("S1: sleeping for 2 seconds to simulate really important\n");
		printf ("S1: work being done by S1 via critical region\n");
		sleep (2);

		printf ("S1: done critical region\n");

		printf ("S1: tell S2 we're done by incrementing semaphore ...\n");

		/*
		 * do an increment on our only semaphore
		 */

		if (semop (semid, &release_operation, 1) == -1) {
			printf ("S1: grr, can't exit critical region\n");
			semctl (semid, 0, IPC_RMID);
			exit (6);
		}	/* endif */
	}	/* endif */

	remove ("shared.txt"); // remove shared file as it's no longer needed

	printf ("S1: we're done. wait 5 sec for S2 to finish with \n");
	printf ("S1: shared file.\n");
	sleep (5);

	printf ("S1: release the semaphores\n");
	semctl (semid, 0, IPC_RMID, 0);

	return 0;
}	/* end main */




