#include "donuts.h"

int shmid, semid[3];

int main(int argc, char* argv[])
{
	int out_ptr[NUMFLAVORS];
	int i, j, k;
	int donuts;
	struct donut_ring *shared_ring;
	struct timeval randtime;

	/* get shared memory */
	if ((shmid = shmget(MEMKEY, sizeof(struct donut_ring), 0)) == -1) {
		perror("shared get failed:");
		exit(-1);
	}

	/* shared memory attach */
	if ((shared_ring = shmat(shmid, NULL, 0)) == (void *)-1) {
		perror("shard attach failed:");
		exit(-1);
	}

	/*  allocate semaphore */	
	for (i = 0; i < NUMSEMIDS; i++) {
		if ((semid[i] = semget(SEMKEY+i, NUMFLAVORS, 0)) == -1) {
			perror("semaphore allocation failed:");
			exit(-1);
		}
	}	

	gettimeofday(&randtime, (struct timezone *)0);

	/* use microsecond components for uniquencess */
	unsigned short xsub1[3];
	xsub1[0] = (ushort) randtime.tv_usec;
	xsub1[1] = (ushort) (randtime.tv_usec >> 16);
	xsub1[2] = (ushort) (getpid());	// unique value

/*	time_t rawtime; 
	struct tm * timeinfo; 
	time ( &rawtime ); 
	timeinfo = localtime ( &rawtime ); 
	int t = 0;
*/
	for (i = 0; i < 10; i++) {
//		printf("consumer process PID:%d\t time:%s dozen#%d\n", getpid(), asctime(timeinfo), i);
//		printf("\n plain\tjelly\tcoconut\thoney-dip\n");
		for (j = 0; j < 12; j++) {
		    k = nrand48(xsub1) & 3;	// generate a random number
		    if (p(semid[CONSUMER], k) == -1) {
			perror("p consumer is failed:");
			exit(-1);	
		    }
		    if (p(semid[OUTPTR], k) == -1) {
			perror("p outptr is failed:");
			exit(-1);
		    }
		    donuts = shared_ring->flavor[k][shared_ring->outptr[k]];
		    shared_ring->outptr[k] = (shared_ring->outptr[k] + 1) % NUMSLOTS;
		   
		    if (v(semid[PROD], k) == -1) {
			perror("v producer is failed:");
			exit(-1);
		    }
		    if (v(semid[OUTPTR], k) == -1) {
			perror("v outptr is failed:");
			exit(-1);
		    }
		}
		usleep(nrand48(xsub1) % 2000000);
	}

	return 0;	
}

