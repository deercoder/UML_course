#include "donuts.h"

int shmid, semid[3];

void sig_handler(int);

int main(int argc, char* argv[])
{
	int in_ptr[NUMFLAVORS];
	int serial[NUMFLAVORS];
	int i, j, k;
	struct donut_ring *shared_ring;
	struct timeval randtime;

	/* handling of system call */
	sigset_t mask_sigs;
	int nsigs;
	struct sigaction new_action;
	int sigs[] = { SIGHUP, SIGINT, SIGQUIT, SIGBUS, SIGTERM, SIGSEGV, SIGFPE};
	
	/* producer initializes serial counters and in-pointers */
	for (i = 0; i < NUMFLAVORS; i++) {
		in_ptr[i] = 0;
		serial[i] = 0;
	}

	nsigs = sizeof(sigs) / sizeof(int);
	sigemptyset(&mask_sigs); // empty the mask of signal

	/* add signals that we want to handle into the mask */
	for (i = 0; i < nsigs; i++) {
		sigaddset(&mask_sigs, sigs[i]);
	}

	/* make system call to cathch those signals */	
	for (i = 0; i < nsigs; i++) {
		new_action.sa_handler = sig_handler;
		new_action.sa_mask = mask_sigs;
		new_action.sa_flags = 0;
	
		if (sigaction(sigs[i], &new_action, NULL) == -1) {
			perror("can't set signals:");
			exit(1);
	
		}
	}

	/* get shared memory */
	if ((shmid = shmget(MEMKEY, sizeof(struct donut_ring), IPC_CREAT|0600)) == -1) {
		perror("shared get failed:");
		exit(1);
	}

	/* shared memory attach */
	if ((shared_ring = shmat(shmid, NULL, 0)) == (void *)-1) {
		perror("shard attach failed:");
		sig_handler(-1);
	}

	/*  allocate semaphore */	
	for (i = 0; i < NUMSEMIDS; i++) {
		if ((semid[i] = semget(SEMKEY+i, NUMFLAVORS, IPC_CREAT|0600)) == -1) {
			perror("semaphore allocation failed:");
			sig_handler(-1);
		}
	}	

	gettimeofday(&randtime, (struct timezone *)0);

	/* use microsecond components for uniquencess */
	unsigned short xsub1[3];
	xsub1[0] = (ushort) randtime.tv_usec;
	xsub1[1] = (ushort) (randtime.tv_usec >> 16);
	xsub1[2] = (ushort) (getpid());	// unique value

	if (semsetall(semid[PROD], NUMFLAVORS, NUMSLOTS) == -1) {
		perror("semsetall failed:");
		sig_handler(-1);
	}
	
	if (semsetall(semid[CONSUMER], NUMFLAVORS, 0) == -1) {
		perror("semsetall failed:");
		sig_handler(-1);
	}		

	if (semsetall(semid[OUTPTR], NUMFLAVORS, 1) == -1) {
		perror("semsetall failed:");
		sig_handler(-1);
	}

	while(1)
	{
		/* use nrand48 with xsub1 to get 32 bit random number */
		j = nrand48(xsub1) & 3;		// random value 0 - 3
		if (p(semid[PROD], j) == -1) {
			perror("p producer is failed");
			sig_handler(-1);
		}
		shared_ring->flavor[j][in_ptr[j]] = serial[j];
		in_ptr[j] = (in_ptr[j] + 1) % NUMSLOTS;
		serial[j]++;
		if (v(semid[CONSUMER], j) == -1) {
			perror("v consumer is failed");
			sig_handler(-1);
		}
	}
	return 0;	
}

void sig_handler(int sig)
{
	int i;
	printf("In signal handler with signal #%d\n", sig);
	
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("handler failed shm RMID:");
	}

	for (i = 0; i < NUMSEMIDS; i++) {
		if (semctl(semid[i], 0, IPC_RMID) == -1) {
			perror("handler failed sem RMID:");
		}
	}

	exit(5);
}

