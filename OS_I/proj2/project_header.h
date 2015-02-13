/********************************************************************/
/********************************************************************/
/* define DEBUG to get files with random numbers for each of the    */
/* threads....file names prodNn   consNn                            */
/* define GLOBAL to get global vs. local thread creation            */
/********************************************************************/
/********************************************************************/
#ifndef PROJECT_HEADER_H
#define PROJECT_HEADER_H

#define _GNU_SOURCE
#include <sched.h>
#include <utmpx.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <strings.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <errno.h>

// macros included in sched.h don't seem to work
// so define them here directly

/********************************************************
#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp)
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp)
********************************************************/


#define         NUMFLAVORS      4
#define         NUMSLOTS        900
#define         NUMCONSUMERS    35
#define         NUMPRODUCERS    10
#define         NUM_DOZ_TO_CONS 500
#define		STDOUT		1
#define		STDIN		0

#ifndef 	PRI_OTHER_MAX
#define 	PRI_OTHER_MAX 	63
#endif /* end of PRI_OTHER_MAX */

typedef struct  {
	int         producer_id;
	int         serial_number;
}donut_t;

typedef struct  {
        donut_t     donut_ring_buffers[NUMFLAVORS][NUMSLOTS];
        int         out_ptr[NUMFLAVORS];
	int         in_ptr[NUMFLAVORS];
	int         space_count[NUMFLAVORS];
	int         donut_count[NUMFLAVORS];
	int         serial[NUMFLAVORS];
}DonutStore;
#endif /* end of PROJECT_HEADER_H */
