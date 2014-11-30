/**
  node controller: a lamport algorithm node controller
**/

#include "buf_mgr.h"
#include "ddonuts.h"

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


/*
 *  establish the node controller
 * ./node_control node_id total_number
 */
int main(int argc, char **argv)
{
   int role;
   int number;

   if (argc != 3){
	printf("Error: input number mismatch!\n");
	exit(0);
   }

   role = *argv[1] - '0'; // get node id
   number = *argv[2] - '0'; // get total number of nodes

   // set up client for this node(except for last node without client) 
   if (role != number)
   {


   } 
 

   // set up server for this node(except for 1st node without server)
   if (role != 1)
   {



   }



   dbg_printf("%d, %d\n", role, number);

   return 0;
}



