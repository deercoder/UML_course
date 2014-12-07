/***
* utils for transferring data between different process
***/ 

#include "global.h"
#include "util.h"

void make_header(MSG *message_ptr, int type, int size)
{

	message_ptr->msize = htonl(size);
	message_ptr->mtype = htonl(type);
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


void converge_read(int socket, char* buffer, int num_bytes)
{
int j;
	while ((j = read(socket, buffer, num_bytes)) != num_bytes){
              switch(j){
                default:  num_bytes -= j;
                          buffer += j;
                          break;
                case -1:  perror("inet_sock read failed: ");
                          exit(3);

                case  0:  printf("unexpected EOF on inet_sock\n");
                          exit(4);
              }
	}
}


/**
  make_msg(&msg, j, my_id, donut_num++, node_id);
*/
void make_msg(MSG *message_ptr, int type, int id, int num, int node_id)
{

	//message_ptr->msize = htonl(size);
	message_ptr->mtype = htonl(type);
}



void read_msg(int inet_sock, char *buf)
{
	int i;
	for(i=0; i<(2*sizeof(int)); i++){
             if(read(socket, buf+i, 1) != 1){
                perror("read buf content failed: ");
                exit(3);
             }
          }
}
