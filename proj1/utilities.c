#include "donuts.h"

/*
struct sembuf {
	short sme_num;	// semaphore index
	short sem_op;	// semaphore operation
	short sem_flg;	// operation flag
};
*/

union semnun {
	int val;
	struct semid_ds *buf;
	u_short *array;
};

/*
int semop(int semid, struct sembuf *sops, int nsops);
*/

int p(int semidgroup, int donut_type)
{
	struct sembuf semopbuf;
	semopbuf.sem_num = donut_type;
	semopbuf.sem_op = (-1);	// -1 is a p operation
	semopbuf.sem_flg = 0;
	
	if (semop(semidgroup, &semopbuf, 1) == -1) {
		perror("p operation failed:");
		return -1;
	}
	
	return 0;
}	

int v(int semidgroup, int donut_type)
{
	struct sembuf semopbuf;
	
	semopbuf.sem_num = donut_type;
	semopbuf.sem_op = (+1);		// +1 is a v operation
	semopbuf.sem_flg = 0;

	if (semop(semidgroup, &semopbuf, 1) == -1) {
		perror("v operation failed:");
		return -1;
	}

	return 0;
}

int semsetall(int semgroup, int number_in_group, int set_all_value)
{
	int i, j, k;
	union semnun sem_ctl_un;

	sem_ctl_un.val = set_all_value;
	
	for (i = 0; i < number_in_group; i++) {
		if (semctl(semgroup, i, SETVAL, sem_ctl_un) == -1) {
			perror("semset failed");
			return -1;
		}
	}

	return 0;
}


