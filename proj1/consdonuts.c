#include "donuts.h"

int shmid, semid[3];

int main(int argc, char* argv[])
{
    int out_ptr[NUMFLAVORS];
    int i, j, k;
    int s, t;
    int donuts;
    int sequence = 0;
    int entry[12][NUMFLAVORS];        // add this to record the output
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

    for(i = 0; i < 3; i++) {
        shared_ring->outptr[i] = 0; // initialize the index of each
    }

    for (i = 0; i < 10; i++) {
        char timeTmp[256];
        /*int hour = randtime.tv_sec / (60 * 60);
        int minute = (randtime.tv_sec - 60 * 60 * hour ) / 60;
        int second = randtime.tv_sec - hour * 3600 - minute * 60;*/
        int usecond = randtime.tv_usec/1000;
        time_t timep;
        struct tm *pt;
        time(&timep);
        pt = gmtime(&timep);
        sprintf(timeTmp, "%d:%d:%d.%d", pt->tm_hour, pt->tm_min, pt->tm_sec, usecond);
        printf("\nconsumer process PID:%d\t time:%s dozen#%d\n", getpid(), timeTmp, i);
        printf("plain\tjelly\tcoconut\thoney-dip\n");

        for (s = 0; s < 12; s++)
            for (t = 0; t < NUMFLAVORS; t++)
                entry[s][t] = -1; // initialize to -1

        sequence = 0;
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

            entry[sequence++][k] = donuts;

            if (v(semid[PROD], k) == -1) {
                perror("v producer is failed:");
                exit(-1);
            }
            if (v(semid[OUTPTR], k) == -1) {
                perror("v outptr is failed:");
                exit(-1);
            }
        }

        int printCount = 0;
        for(s = 0; s < 12; s++) {
            for(t = 0; t < 4; t++) {
                if (entry[s][t] == -1) {
                    if (t != 3)
                        printf("\t");
                    else
                        printf("\t\n");
                }
                else if(t == 3){
                    printf("%d\n", entry[s][t]);
                    printCount++;
                }
                else{
                    printf("%d\t", entry[s][t]);
                    printCount++;
                }
                if (printCount == 12)
                    break;
            }
        }
        printf("\n");
        usleep(nrand48(xsub1) % 2000000);
    }

    return 0;
}
