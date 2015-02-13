#ifndef GLOBAL_H
#define GLOBAL_H

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MSG_BODY  248
#define MSG_SIZE  (MSG_BODY+8)

#define PORT  	  36439
#define BUF_PORT  16178
#define PRODUCER_MSG  	  1
#define CONSUMER_MSG  	  2
#define DATA  	  100
#define END_DATA  101
#define TEST  	  102
#define END_TEST  103

#define BUF_MGR_HOST "cs91515-6"

typedef  struct{
	int  mtype;
	int  msize;
	int mid;	// unique id for msg
	int mnode_id;	// node id
	int mdonut_num; // donut number
	char mbody[MSG_BODY];
} MSG;

typedef union{
	MSG m;
	char buf[MSG_SIZE];
} MBUF;

extern int errno;

extern void make_header(MSG *msg_ptr, int type, int size);

extern void read_header(int socket, char *buffer);

extern void converge_read(int socket, char *buffer, int num_bytes);

extern void make_msg(MSG* msg_ptr, int type, int id, int num, int node_id);

#endif
