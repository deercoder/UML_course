#ifndef UTIL_H
#define UTIL_H

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

void make_header(MSG *msg_ptr, int type, int size);

void read_header(int socket, char *buffer);

void converge_read(int socket, char *buffer, int num_bytes);

void make_msg(MSG* msg_ptr, int type, int id, int num, int node_id);
void read_msg(int inet_sock, char *buf);

#endif

