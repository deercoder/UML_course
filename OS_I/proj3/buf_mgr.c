/* buf_mgr.c  -   a fork based distributed buffer manager  */

#include "buf_mgr.h"
#include "ddonuts.h"
#include "global.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/signal.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


int		shmid, semid[3];
void 	sig_handler(int);

int main(int argc, char *argv[])
{
	MSG				msg;
	MBUF				raw;
//	DONUT				donut;
	int 				inet_sock, new_sock, out_index;
	int 				type_val, id_val, read_val, trigger;
	int 				i, j, k, fromlen, nsigs, donut_num, node_id;
	char				*buffer_ptr;
	struct sockaddr_in	inet_telnum;
	struct hostent		*heptr, *gethostbybname();
	int 				wild_card = INADDR_ANY;
	struct sigaction	sigstrc;
	sigset_t			mask;
	struct donut_ring	*shared_ring;
	struct timeval		randtime;
	unsigned short		xsub1[3];
	int sigs[] = {SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGTERM, SIGBUS, SIGSEGV, SIGFPE};

	nsigs = sizeof(sigs)/sizeof(int);
	sigemptyset (&mask);

	for(i=0; i< nsigs; i++)
		sigaddset (&mask, sigs [i]);

	for(i = 0; i <  nsigs; i++){
		sigstrc.sa_handler 	= sig_handler;
		sigstrc.sa_mask 	= mask;
		sigstrc.sa_flags 	= 0;
		if(sigaction (sigs [i], &sigstrc, NULL) == -1){
			perror("can't set signals: ");
			exit(1);
		}
	}

	if((shmid = shmget (SEMKEY, sizeof(struct donut_ring),
					IPC_CREAT | 0600)) == -1){
		perror("shared get failed: ");
		exit(1);
	}

	if((shared_ring = (struct donut_ring *)shmat (shmid, NULL, 0)) == (void *)-1){
		perror("shared attach failed: ");
		sig_handler(-1);
	}

	for(i=0; i<NUMSEMIDS; i++)
		if ((semid[i] = semget (SEMKEY+i, NUMFLAVORS, 
						IPC_CREAT | 0600)) == -1){
			perror("semaphore allocation failed: ");
			sig_handler(-1);
		}

	if(semsetall (semid [PROD], NUMFLAVORS, NUMSLOTS) == -1){
		perror("semsetsall failed: ");
		sig_handler(-1);
	}
	if(semsetall (semid [CONSUMER], NUMFLAVORS, 0) == -1){
		perror("semsetsall failed: ");
		sig_handler(-1);
	}

	/********** set up sigaction structure to eliminate zombies ************/

	sigemptyset(&mask);

	sigstrc.sa_handler 	= SIG_IGN; // ignore this signal to prevent zombie
	sigstrc.sa_mask 	= mask;
	sigstrc.sa_flags 	= SA_RESTART;

	sigaction(SIGCHLD, &sigstrc, NULL);

	/********** allocate a socket to communicate with **********************/

	if((inet_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("inet_sock allocation failed:");
		sig_handler(-1);
	}


	/***** byte copy the wild_card IP address INADDR_ANY into ************/
	/***** IP address structure, along with port and family and **********/
	/***** use the structure to give yourself a connect address **********/

	bcopy(&wild_card, &inet_telnum.sin_addr, sizeof(int));
	inet_telnum.sin_family = AF_INET;
	inet_telnum.sin_port = htons((u_short) BUF_PORT);

	if(bind(inet_sock, (struct sockaddr *)&inet_telnum, sizeof(struct sockaddr_in)) == -1){
		perror("inet_sock bind failed");
		sig_handler(-1);
	}

	/***** allow client connect requests to arrive: call-wait 5***********/

	listen(inet_sock, 5);

	for(i=0; i<NUMFLAVORS; ++i){
		for(j=0; j<NUMSLOTS; ++j){
			shared_ring->flavor[i][j].node_id = -1;
			shared_ring->flavor[i][j].prod_id = -1;
			shared_ring->flavor[i][j].donut_num = -1;
		}
	}

	printf("\nTHE BUFFER MANAGER IS UP\n");

	/******  forever more, answer the phone and create a child to *********/
	/******  handle each client connection (use EINTR check)      *********/

	trigger = 0;
	for(;;){ /* forever */
		if(trigger++ == 650){			
			write(1, "\n\n*********\n\n", 13);
			if(fork() == 0){
				close(1);
				if(open("/tmp/cnt",O_CREAT | O_RDWR, 0600) != 1){
					perror("open /tmp/cnt");
					exit(3);
				}
				execlp("ps", "ps", "-1", NULL);
				perror("exec ps");
				exit(4);
			}
		}
	}

	/******  set sizeof(struct sockaddr) into fromlen to specify **********/
	/******  original buffer size for returned address (the      **********/
	/******  actual size of the returned address then goes here  **********/

	fromlen = sizeof(struct sockaddr);

	while((new_sock = accept(inet_sock, (struct sockaddr *)&inet_telnum, &fromlen)) == -1 && errno == EINTR);

	if(new_sock == -1){
		perror("accept failed:");
		sig_handler(-1);
	}

	switch(fork()){

		/*******  parent takes this case if fork succeeds      *********/

		default: close(new_sock);
			 break;

			 /*******  parent takes this case if fork fails         *********/
		case -1: perror("fork failed: ");      
			 sig_handler(-1);

			 /*******  child takes this case if fork succeeds        ********/
		case 0: close(inet_sock);

			/*******  read message info of message sent by client   ********/

			read_msg(new_sock, &raw.buf);

			type_val = ntohl(raw.m.mtype);
			id_val = ntohl(raw.m.mid);
			donut_num = ntohl(raw.m.mdonut_num);
			node_id = ntohl(raw.m.mnode_id);

			/*******   what type of message has the client sent to us?? *****/
			switch(type_val){
				case PRO_JELLY:
					if( p(semid[PROD], JELLY) == -1){
						perror("p operation failed: ");
						exit(9);
					}
					shared_ring->flavor[JELLY][shared_ring->in_ptr[JELLY]].node_id = node_id;
					shared_ring->flavor[JELLY][shared_ring->in_ptr[JELLY]].prod_id = id_val;
					shared_ring->flavor[JELLY][shared_ring->in_ptr[JELLY]].donut_num = donut_num;

					usleep(100);
					shared_ring->in_ptr[JELLY] = (shared_ring->in_ptr[JELLY]+1) % NUMSLOTS;

					if( v(semid[CONSUMER],JELLY) == -1){
						perror("v operation failed: ");
						exit(9);
					}
					make_msg(&msg, P_ACK, BUF_MGR, donut_num, 0);
					if(write(new_sock, &msg, (4*sizeof(int))) == -1){
						perror("new_sock write failed: ");
						exit(3);
					}
					close(new_sock);
					exit(10);

				case PRO_PLAIN:
					if( p(semid[PROD], PLAIN) == -1){
						perror("p operation failed: ");
						exit(9);
					}
					shared_ring->flavor[PLAIN][shared_ring->in_ptr[PLAIN]].node_id = node_id;
					shared_ring->flavor[PLAIN][shared_ring->in_ptr[PLAIN]].prod_id = id_val;
					shared_ring->flavor[PLAIN][shared_ring->in_ptr[PLAIN]].donut_num = donut_num;

					usleep(100);
					shared_ring->in_ptr[JELLY] = (shared_ring->in_ptr[JELLY]+1) % NUMSLOTS;

					if( v(semid[CONSUMER],JELLY) == -1){
						perror("v operation failed: ");
						exit(9);
					}
					make_msg(&msg, P_ACK, BUF_MGR, donut_num, 0);
					if(write(new_sock, &msg, (4*sizeof(int))) == -1){
						perror("new_sock write failed: ");
						exit(3);
					}
					close(new_sock);
					exit(10);

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
