/*
 * semaphore2.c
 *
 * A simple example of semaphore control.
 * This app is launched by semaphore1.c, and will
 * wait for access to a critical region in
 * order to read file data written by s1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int
main (int argc, char *argv[])
{
	/*
	 * the following are operation structures for semaphore control
	 * the acquire will decrement the semaphore by 1
	 * and the release will increment the semaphore by 1.
	 * Both of these structs are initialized to work with the 1st
	 * semaphore we allocate.
	 */

	struct sembuf acquire_operation = { 0, -1, SEM_UNDO };
	struct sembuf release_operation = { 0, 1, SEM_UNDO };

	/*
	 * the initial value of the semaphore will be 1, indicating that
	 * our critical region is ready is ready for use by the first
	 * task that can successfully decrement our shared semaphore
	 */

	unsigned short init_values[1] = { 1 };

	int semid;
	char data[512];
	FILE *fp;
	int x;

	printf ("S2: This program will use critical regions.\n");
	printf ("S2: It will mark the critical region with a semaphore\n");
	printf ("S2: and only when instructed, will a common file between\n");
	printf ("S2: S1 and S2 be accessed.\n");

	if (argc != 2) {
		printf ("S2: grr, insufficient args for this app\n");
		exit (1);
	}	/* endif */

	/*
	 * since program S1 forked and exec us into existance, it
	 * provided the semaphore ID as the 2nd argument for this
	 * task. Thus, convert the argument into an integer and
	 * use this integer as the semaphore ID
	 */

	semid = atoi (argv[1]);

	printf ("S2: our semaphore ID is %d\n", semid);

	printf ("S2: loop forever, until our semaphore goes away\n");
	printf ("S2: this will happen only if S1 terminates and deallocates the semaphore\n");

	while (1) {
		/*
		 * attempt to decrement the semaphore
		 */

		if (semop (semid, &acquire_operation, 1) == -1) {
			printf ("S2: hmm, can't start critical region\n");
			break;
		}	/* endif */

		if ((fp = fopen ("shared.txt", "r")) != NULL) {
			fgets (data, sizeof (data), fp);
			printf ("S2: received from S1 ... %s\n", data);
			fclose (fp);
		}	/* endif */

		/*
		 * attempt to increment the semaphore
		 */

		if (semop (semid, &release_operation, 1) == -1) {
			printf ("S2: hmm, can't end critical region\n");
			break;
		}	/* endif */
	}	/* endif */

	printf ("S2: we're done!\n");

	return 0;
}	/* end main */




