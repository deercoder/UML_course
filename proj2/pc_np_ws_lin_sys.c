
/********************************************************************/
/********************************************************************/
/* define DEBUG to get files with random numbers for each of the    */
/* threads....file names prodNn   consNn                            */
/* define GLOBAL to get global vs. local thread creation            */
/********************************************************************/
/********************************************************************/
#define _GNU_SOURCE
#include <sched.h>
#include <utmpx.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <errno.h>



/********************************************************
# define CPU_SET(cpu, cpusetp)   __CPU_SET_S (cpu, sizeof (cpu_set_t), cpusetp)
# define CPU_CLR(cpu, cpusetp)   __CPU_CLR_S (cpu, sizeof (cpu_set_t), cpusetp)
# define CPU_ISSET(cpu, cpusetp) __CPU_ISSET_S (cpu, sizeof (cpu_set_t), \
                                                cpusetp)
# define CPU_ZERO(cpusetp)       __CPU_ZERO_S (sizeof (cpu_set_t), cpusetp)

// macros included in sched.h don't seem to work
// so define them here directly

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
#endif

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


DonutStore          the_store;
pthread_t      	    thread_id[(NUMCONSUMERS+NUMPRODUCERS)], sig_wait_id;

pthread_mutex_t	    prod[NUMFLAVORS];
pthread_mutex_t	    cons[NUMFLAVORS];
pthread_cond_t	    prod_cond[NUMFLAVORS];
pthread_cond_t	    cons_cond[NUMFLAVORS];


int  main(int argc, char *argv[])
{

	void	*sig_waiter(void *arg);
	void	*producer(void *arg);
	void	*consumer(void *arg);
	void    sig_handler (int sig);

	int	       i,j,k,len,nsigs;
	int            first_consumer_thread_index;
        char           msg[100];
	struct timeval randtime, first_time, last_time;
	struct sched_param   sched_struct;
	int	       arg_array[NUMPRODUCERS+NUMCONSUMERS];
	sigset_t       all_signals;
	pthread_attr_t thread_attr;
        struct sigaction        new;
	unsigned int cpu;
	cpu_set_t mask;

/*** 
        int sigs[]              = { SIGPIPE, SIGBUS, SIGSEGV, SIGFPE, SIGHUP, SIGTERM, SIGINT, SIGQUIT, SIGABRT, SIGCHLD };
***/
	int sigs[]              = { SIGPIPE, SIGBUS, SIGSEGV, SIGFPE };

	sprintf(msg, "\nThis run will have\n  Producers  = %d\n  Consumers  = %d\n  Qdepth     = %d\n  Cons dozns = %d\n  Donut flav = %d\n", 
          NUMPRODUCERS, NUMCONSUMERS, NUMSLOTS, NUM_DOZ_TO_CONS, NUMFLAVORS);
        write(STDOUT, msg, strlen(msg));
/***
	sprintf(msg, "To proceed hit return, to cancel hit ^C\n");
        write(STDOUT, msg, strlen(msg));
	read(STDIN, msg, 100);
***/

        printf("\nINHERITED PROCESS AFFINITY MASK:\n");
        sched_getaffinity(0, sizeof(cpu_set_t), &mask);
//	printf("there are %d processors in this system\n", CPU_COUNT(&mask));
        for(i=0; i<8; ++i){
          printf("CPU ID %d  %s\n", i, (CPU_ISSET(i, &mask))?"exists":"does not exist");
        }

	gettimeofday(&first_time, (struct timezone *)0);
	for(i=0; i<(NUMCONSUMERS+NUMPRODUCERS); i++){
		arg_array[i] = i+1;
	}

	for(i=0; i<NUMFLAVORS; i++){
                pthread_mutex_init(&prod[i], NULL);
                pthread_mutex_init(&cons[i], NULL);
                pthread_cond_init(&prod_cond[i], NULL);
                pthread_cond_init(&cons_cond[i], NULL);
		the_store.out_ptr[i]=0;
		the_store.in_ptr[i]=0;
		the_store.serial[i]=0;
		the_store.space_count[i]=NUMSLOTS;
		the_store.donut_count[i]=0;
	}

	gettimeofday(&randtime, (struct timezone *)0);

     	sigfillset(&all_signals);
        nsigs = sizeof(sigs)/sizeof(int);

        for(i=0; i< nsigs; i++)
                sigdelset(&all_signals, sigs[i]);
	sigprocmask(SIG_BLOCK, &all_signals, NULL);

        for(i=0; i< nsigs; i++){
                new.sa_handler  = sig_handler;
                new.sa_mask     = all_signals;
                new.sa_flags    = 0;
                if(sigaction(sigs[i], &new, NULL) == -1){
                                perror("can't set signals: ");
                                exit(1);
                }
        }

	sprintf(msg,"just before threads created\n");
        len=strlen(msg);
        write(STDOUT, msg, len);

/***
	sprintf(msg,"The value of sched_get_priority_max(SCHED_OTHER) is %d\n", 
					   sched_get_priority_max(SCHED_OTHER));
        len=strlen(msg);
        write(STDOUT, msg, len);
***/

        if((errno = pthread_create(&sig_wait_id, NULL,
                          sig_waiter, NULL)) != 0){
                perror("pthread_create failed ");
                exit(3);
        }


	pthread_attr_init(&thread_attr);
	pthread_attr_setinheritsched(&thread_attr, PTHREAD_INHERIT_SCHED);

#ifdef  GLOBAL
	sched_struct.sched_priority = sched_get_priority_max(SCHED_OTHER);
	pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
    	pthread_attr_setschedparam(&thread_attr, &sched_struct);      
	pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
#endif

        for(i=0; i<NUMPRODUCERS; i++){
                if((errno = pthread_create(&thread_id[i], &thread_attr, 
                          producer, (void *)&arg_array[i])) != 0){
		
		perror("pthread_create failed ");
		exit(3);
                }
	}


	first_consumer_thread_index = i;
        for( ; i<(NUMCONSUMERS+NUMPRODUCERS); i++){
		if((errno = pthread_create(&thread_id[i], &thread_attr,
                         consumer, (void *)&arg_array[i-NUMPRODUCERS])) != 0){
		  perror("pthread_create consumers failed ");
		  exit(3);
		}
	}

	sprintf(msg, "just after threads created\n");
        len=strlen(msg);
        write(STDOUT, msg, len);


	for(i=first_consumer_thread_index; i<(NUMCONSUMERS+NUMPRODUCERS); i++){
/***
                sprintf(msg,"About to join %d\n", thread_id[i]);
                len=strlen(msg);
                write(STDOUT, msg, len);
***/
		pthread_join(thread_id[i], NULL);
/***
        	sprintf(msg,"Just joined %d\n", thread_id[i]);
        	len=strlen(msg);
       		write(STDOUT, msg, len);
***/
		write(STDOUT, "*", 1);
	}

	gettimeofday(&last_time, (struct timezone *)0);
	if((i=last_time.tv_sec - first_time.tv_sec) == 0)
          j=last_time.tv_usec - first_time.tv_usec;
	else{
	    if(last_time.tv_usec - first_time.tv_usec < 0){
            i--;
	    j= 1000000 + (last_time.tv_usec - first_time.tv_usec);
            }else{ j=last_time.tv_usec - first_time.tv_usec;}
        }
	printf("\nElapsed consumer time is %d sec and %d usec\n",
              i, j);
	printf("\n\n ALL CONSUMERS FINISHED, NORMAL PROCESS EXIT\n\n");
	exit(0);
}


void	*producer(void *arg)
{
	int	i, j, len;
	char	msg[100];
	ushort  xsub1[3];
	struct timeval randtime;
        int    cn;
        char   number[2] = {'\060','\n'};
        char   file_name[10] = "prod";
        char   thread_number[5];


	gettimeofday(&randtime, (struct timezone *)0);
	xsub1[0] = (ushort)randtime.tv_usec;
        xsub1[1] = (ushort)(randtime.tv_usec >> 16);
        xsub1[2] = (ushort)(getpid());

#ifdef  DEBUG
        itoa(pthread_self(), thread_number);

        strcat(file_name, thread_number);

        if((cn = open(file_name, O_WRONLY | O_CREAT, 0666)) == -1){
           perror("filed to open producer log ");
	}
#endif


	while(1){
	  j=nrand48(xsub1) & 3;

#ifdef  DEBUG
          number[0] = j + 060;
	  write(cn, number, 2);
#endif
	  /*  printf("in producer with rand = %d\n",j);  */
	  pthread_mutex_lock(&prod[j]);
	  while(the_store.space_count[j] == 0){
		pthread_cond_wait(&prod_cond[j], &prod[j]);
	  }
	  the_store.space_count[j]--;
	  the_store.donut_ring_buffers[j][the_store.in_ptr[j]].serial_number =
                                                         the_store.serial[j];
	  the_store.donut_ring_buffers[j][the_store.in_ptr[j]].producer_id =
                                                         *(int *)arg;
	  the_store.in_ptr[j] = (the_store.in_ptr[j]+1) % NUMSLOTS;
	  the_store.serial[j]++;
	  pthread_mutex_unlock(&prod[j]);
/*
          if(the_store.serial[j] % 120 == 0){
            sprintf(msg, "producer %d finishes 10 dozen %ds\n",*(int *)arg, j);
            len=strlen(msg);
            write(STDOUT, msg, len);
          }
*/


	  pthread_mutex_lock(&cons[j]);
	  the_store.donut_count[j]++;
	  pthread_mutex_unlock(&cons[j]);

	  pthread_cond_signal(&cons_cond[j]);

	  /*  printf("prod type %d serial %d\n",j,serial[j]-1);  */
	}
	return NULL;
}

void	*consumer(void *arg)
{
	donut_t extracted_donut;
	int	i, j, m, id, len;
	char	msg[100], lbuf[5];
        ushort  xsub1[3];
        struct timeval randtime;
        int    cn;
	unsigned cpu;
	char   number[2] = {'\060','\n'};
        char   file_name[10] = "cons";
	char   thread_number[5];
        gettimeofday(&randtime, (struct timezone *)0);
        xsub1[0] = (ushort)randtime.tv_usec;
        xsub1[1] = (ushort)(randtime.tv_usec >> 16);
        xsub1[2] = (ushort)(getpid());

#ifdef  DEBUG
	itoa(pthread_self(), thread_number);

	strcat(file_name, thread_number);

        if((cn = open(file_name, O_WRONLY | O_CREAT, 0666)) == -1){
           perror("filed to open producer log ");
        }
#endif



	id = *(int *)arg;
	sprintf(lbuf," %d ", id);
	if(id == 2){
	  sprintf(file_name,"cons%03d", id);
	  if((cn = open(file_name, O_WRONLY | O_CREAT, 0666)) == -1){
           perror("failed to open cons002 log ");
          }
	}
        for(i=0; i<NUM_DOZ_TO_CONS; i++){
	 if(id == 2){
	   sprintf(msg, "cpu %d\n", sched_getcpu());
	   write(cn, msg, strlen(msg));
	 }
         for(m=0; m<12; m++){

	  j=nrand48(xsub1) & 3;

#ifdef  DEBUG
          number[0] = j + 060;
          write(cn, number, 2);
#endif

          /*  printf("in consumer %d  with i = %d and m = %d with rand = %d\n", *(int *)arg, i, m, j);  */
	  
	  pthread_mutex_lock(&cons[j]);
	  while(the_store.donut_count[j] == 0){
		pthread_cond_wait(&cons_cond[j], &cons[j]);
	  }
	  the_store.donut_count[j]--;
	  extracted_donut =
                         the_store.donut_ring_buffers[j][the_store.out_ptr[j]];
	  the_store.out_ptr[j] = (the_store.out_ptr[j]+1) % NUMSLOTS;
	  pthread_mutex_unlock(&cons[j]);

	  pthread_mutex_lock(&prod[j]);
	  the_store.space_count[j]++;
	  pthread_mutex_unlock(&prod[j]);

	  pthread_cond_signal(&prod_cond[j]);

	  /*  printf("consumer type %d serial %d\n",j,k);  */
	 }
/*
         sprintf(msg, "consumer %d dozen %d done \n",id, i+1);
	 len=strlen(msg);
	 write(STDOUT, msg, len);
*/
/****
         sched_yield();
****/
	 usleep(100);
	}
	write(1, lbuf, 4);
	return NULL;
}


void   * sig_waiter(void *arg)
{
	sigset_t	sigterm_signal;
	int		sig_taken, len;
	char 		msg[100];

	sigemptyset(&sigterm_signal);
	sigaddset(&sigterm_signal, SIGUSR1);
	sigaddset(&sigterm_signal, SIGUSR2);
	sigaddset(&sigterm_signal, SIGINT);
	sigaddset(&sigterm_signal, SIGTERM);

	do{
	  if(sigwait(&sigterm_signal,  &sig_taken) != 0){
		write(1, "\n   sigwait() failed \n",
			strlen("\n   sigwait() failed \n"));
		exit(2);
	  }
	  if(sig_taken == SIGUSR1 || sig_taken == SIGUSR2){
	   sprintf(msg, "sigwaiter thread catching SIGNAL %s  (number %d)\n\n", 
			(sig_taken == SIGUSR1)?"SIGUSR1":"SIGUSR2", sig_taken);
           len=strlen(msg);
           write(STDOUT, msg, len);
	  }else{
           sprintf(msg, "process going down on SIGNAL (number %d)\n\n", sig_taken);
           len=strlen(msg);
           write(STDOUT, msg, len);
	   exit(2);
	  }
	}while(1);
/***
	printf("process going down on SIGNAL (number %d)\n\n", sig_taken);
***/
	exit(1);
	return NULL;
}


void    sig_handler (int sig)
{
        pthread_t       signaled_thread_id;
        int             i, thread_index, len;
        char            msg[100];


        signaled_thread_id = pthread_self();
        for (i = 0; i < (NUMCONSUMERS + NUMPRODUCERS); i++) {
                if (signaled_thread_id == thread_id[i]) {
                        thread_index = i;
                        break;
                }
		thread_index = -1;
        }
        sprintf(msg, "\nThread %d took signal # %d, PROCESS HALT\n" ,
					thread_index, sig);
        len=strlen(msg);
        write(STDOUT, msg, len);

/***
        printf("\nThread %d took signal # %d, PROCESS HALT\n" ,
                                              thread_index, sig);
***/
	if(sig == SIGCHLD) return;
        exit(1);
}
