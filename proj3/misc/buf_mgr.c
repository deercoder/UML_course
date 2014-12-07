/*
 * Buffer Manager: provide control for 4 ring buffers
 */

#include <sched.h>
#include <utmpx.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <strings.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "buf_mgr.h"
#include "global.h"

#define		STDOUT		1
#define		STDIN		0

void child_handler(int signum);

// buf manager is a process, which handles any producer/consumer request
// so it is a server-side program which communicates using socket

int main(int argc, char *argv[])
{
    // data for buf manager(server-side)
    int inet_sock_server;
    struct hostent *heptr_server;
    struct sockaddr_in inet_telnum_server;
    int wild_card = INADDR_ANY;
    sigset_t mask;
    struct sigaction sigstrc;
    int accept_addr_len;
    int accept_fd;

    // data
    char buf_str[] = "This is the msg from buf manager";
    int i = 0;

    // set up sigaction structure to clean zombies
    sigemptyset(&mask);
    sigstrc.sa_handler = child_handler;
    sigstrc.sa_mask = mask;
    sigstrc.sa_flags = 0;
    sigaction(SIGCHLD, &sigstrc, NULL);

    // allocate a socket to communicate with
    if ((inet_sock_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    perror("inet_sock_server allocation failed:");
	    exit(1);
    }

    // byte copy the wild_card IP address INADDR_ANY into
    // IP address structure, along with the port and family add
    // use the structure to give yourself a connect address
    bcopy(&wild_card, &inet_telnum_server.sin_addr, sizeof(int));
    inet_telnum_server.sin_family = AF_INET;
    inet_telnum_server.sin_port = htons( (u_short) BUF_PORT);	

    // bind to the IP address
    if (bind(inet_sock_server, (struct sockaddr *)&inet_telnum_server, sizeof(struct sockaddr_in)) == -1) {
	    perror("inet_sock bind failed:");
	    exit(2);
    }

    // allow client connect requests to arrive: call-wait 5
    if (listen(inet_sock_server, 5) == -1) {
	    perror("inet_sock_server listen error:");
	    exit(2);
    }

    // server is keeping until client establish the connection
    while (1) {
	    accept_addr_len = sizeof(struct sockaddr_in);
	    accept_fd = accept(inet_sock_server, (struct sockaddr *)&inet_telnum_server, &accept_addr_len);
	    if (accept_fd == -1) {
		    perror("inet_sock_server accept error:");
		    exit(3);
	    }

	    if (write(accept_fd, buf_str, strlen(buf_str)) == -1) {
		    perror("write accept_fd error:");
		    exit(4);
	    }
	    //close(accept_fd);
	    // 
    }


   return 0;
}

void child_handler(int sig) {
/*
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
*/
	char msg[100];
	int len;

	sprintf(msg, "\nThread %d took signal, PROCESS HALT\n" , sig);
	len=strlen(msg);
	write(STDOUT, msg, len);

	if(sig == SIGCHLD) 
		return;

	exit(1);
}

void read_header(int socket, char* buffer)
{
	int i;
	for(i=0; i<(2*sizeof(int)); i++){
             if(read(socket, buffer+i, 1) != 1){
                perror("read type_size failed: ");
                exit(3);
             }
          }
}


