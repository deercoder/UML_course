/**
  node controller: a lamport algorithm node controller
**/

#include "buf_mgr.h"
#include "ddonuts.h"
#include "global.h"

#include <unistd.h>
#include <pthread.h>

#define MAXNODE 16
#define DEBUG

char *host_list[] = {"host_list", "cs91515-1", "cs91515-2", "cs91515-3", "cs91515-4", "cs91515-5", "cs91515-6"};

typedef struct thread_arg{
	int my_chan;
	int my_node_id;
}TH_ARG;


#ifdef DEBUG
#define dbg_printf printf
#endif

int connected_ch[MAXNODE];
void *chan_monitor(void *);
void *msg_maker(void *);

void child_handler(int signum);


/*
 *  establish the node controller
 * ./node_control node_id total_number
 */
int main(int argc, char **argv)
{
    // node id & total node number
    int role;
    int number;

    // data for client-end socket
    int inet_sock_client;
    struct hostent *heptr_client;
    struct sockaddr_in inet_telnum_client;	
    int nbytes;
    char buffer[1024];

    // data for server-end socket
    int inet_sock_server;
    struct hostent *heptr_server;
    struct sockaddr_in inet_telnum_server;
    int wild_card = INADDR_ANY;
    sigset_t mask;
    struct sigaction sigstrc;
    int accept_addr_len;
    int accept_fd;

    // data
    char hello[] = "Hello, this is from server!";

   if (argc != 3){
	printf("Error: input number mismatch!\n");
	exit(0);
   }

   role = *argv[1] - '0'; // get node id
   number = *argv[2] - '0'; // get total number of nodes

   // set up client for this node(except for last node without client) 
   if (role != number)
   {
       // allocate a socket to communicate with
       if ((inet_sock_client=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	   perror("inet_socke_client allocation failed: ");
     	   exit(1);
       }

       // get a host pointer to point to a hostent structure
       // which contains the remote IP address of server
       if ((heptr_client = gethostbyname(host_list[role])) == NULL) {
	   perror("gethostbyname failed: ");
	   exit(1);
       }

       // byte copy the IP address from the h_addr field in the hostend
       // structure into an IP address structure
       bcopy(heptr_client->h_addr, &inet_telnum_client.sin_addr, heptr_client->h_length);
       inet_telnum_client.sin_family = AF_INET;
       inet_telnum_client.sin_port = htons((u_short) PORT);	

       // use the connect system call to open a TCP connection
       if (connect(inet_sock_client, (struct sockaddr *)&inet_telnum_client, sizeof(struct sockaddr_in)) == -1) {
		perror("inet_sock_client connect failed: ");
		exit(1);
	}

	
	// remain blank for other operations in the client-end
	if ((nbytes = read(inet_sock_client, buffer, 1024)) == -1) {
		perror("inet_sock_client read error!");
		exit(1);
	}
	buffer[nbytes] = '\0';
	printf("client-end has received %s\n", buffer);
	close(inet_sock_client);
   } 
 

   // set up server for this node(except for 1st node without server)
   if (role != 1)
   {
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
	inet_telnum_server.sin_port = htons( (u_short) PORT);	

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

	   if (write(accept_fd, hello, strlen(hello)) == -1) {
		perror("write accept_fd error:");
		exit(4);
	   }
	   close(accept_fd);
	}
   }

   dbg_printf("%d, %d\n", role, number);
 
   close(inet_sock_client);
   close(inet_sock_server);
   return 0;
}


void child_handler(int signum) {
	wait(NULL);
}
