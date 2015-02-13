#include "buf_mgr.h"
#include "ddonuts.h"
#include "global.h"
#include "util.h"

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


int main(int argc, char **argv){
	int j;
	struct timeval randtime;
	unsigned short		xsub1[3];
	MSG			msg;
	MBUF		raw;
	int inet_sock, local_file, donut_num, node_id;
	int type_val, id_val, read_val, local_size, my_id;
	char *buffer_ptr, *token_ptr, *last_token_ptr;
	char full_file_path_name[256];
	union type_size;
	struct sockaddr_in inet_telnum;
	struct hostent *heptr, *gethostbyname();

	if(argc < 4){
		printf("\nUSAGE: net_producer BM_host_name prod_id node_id\n");
		exit(2);
	}

	my_id = atoi(argv[2]);
	node_id = atoi(argv[3]);

	if((heptr = gethostbyname( argv[1] )) == NULL){
		perror("gethostbyname failed: ");
		exit(1);
	}

	bcopy(heptr->h_addr, &inet_telnum.sin_addr, heptr->h_length);
	inet_telnum.sin_family = AF_INET;
	inet_telnum.sin_port = htons( (u_short)PORT );

	gettimeofday(&randtime, NULL);

	xsub1[0] = (ushort)randtime.tv_usec;
	xsub1[1] = (ushort)(randtime.tv_usec >> 16);
	xsub1[2] = (ushort)(getpid());

	donut_num = 1;
	printf("\n starting producer %d on node %d on node %d \n", my_id, node_id);
	while(1){
		j=nrand48(xsub1) & 3;

		if((inet_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1){
			perror("inet_sock allocation failed: ");
			exit(2);
		}

		if(connect(inet_sock, (struct sockaddr *)&inet_telnum,
					sizeof(struct sockaddr_in)) == -1){
			perror("inet_sock connect failed: ");
			exit(2);										
		}

		make_msg(&msg, j, my_id, donut_num++, node_id);
		if(write(inet_sock, &msg, (4*sizeof(int))) == -1){
			perror("inet_sock write failed: ");
			exit(3);
		}

		read_msg(inet_sock, &raw.buf);

		type_val = ntohl(raw.m.mtype);
		id_val = ntohl(raw.m.mid);
		//			donut_num = ntohl(raw.m.mdonut_num);
		//			node_id = ntohl(raw.m.mnode_id);

		if(type_val != P_ACK)printf("\nBAD REPLY FROM BUFFER MANAGER\n");
		close(inet_sock);
		sleep(10000);
	}   //while 1
}
