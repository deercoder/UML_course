/**

BUF MANAGER:  Message Queue for producing a new donut

**/

#ifndef BUF_MGR_H
#define BUF_MGR_H


#define NUMFLAVORS 4
#define NUMSLOTS 900
#define NUMCONSUMERS 50
#define NUMPRODUCERS 30
#define NUM_DOZ_TO_CONS 200

#define SEMKEY (key_t)918273245
#define MEMKEY (key_t)91827245
#define NUMSEMIDS 3

#define PROD 0
#define CONSUMER 1
#define OUTPTR 2

#define PRO_JELLY 0
#define PRO_PLAIN 1

#define JELLY 100
#define PLAIN 101

#define P_ACK 0
#define BUF_MGR 100

#define BUF_MSG 10000

#define MSGKEY (1234567)

typedef struct {
	int serial_number;
	int node_id;
	int prod_id;
	int donut_num;
} donut_t;


struct donut_ring {
	donut_t     flavor[NUMFLAVORS][NUMSLOTS];
	int         out_ptr[NUMFLAVORS];
	int         in_ptr[NUMFLAVORS];
	int         space_count[NUMFLAVORS];
	int         donut_count[NUMFLAVORS];
	int         serial[NUMFLAVORS];
};


union semnun {
	int val;
	struct semid_ds *buf;
	char *array;
};

#endif
