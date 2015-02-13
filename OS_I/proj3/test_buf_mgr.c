/***
* This program is used for testing buf manager
***/
#include "buf_mgr.h"
#include "global.h"

#include <unistd.h>
#include <pthread.h>

int main(int argc, char * argv[])
{
	// data for client-end socket
	int inet_sock_client;
	struct hostent *heptr_client;
	struct sockaddr_in inet_telnum_client;	
	int nbytes;
	char buffer[1024];

	// allocate a socket to communicate with
	if ((inet_sock_client=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("inet_socke_client allocation failed: ");
		exit(1);
	}

	// get a host pointer to point to a hostent structure
	// which contains the remote IP address of server
	if ((heptr_client = gethostbyname(BUF_MGR_HOST)) == NULL) {
		perror("gethostbyname failed: ");
		exit(1);
	}

	// byte copy the IP address from the h_addr field in the hostend
	// structure into an IP address structure
	bcopy(heptr_client->h_addr, &inet_telnum_client.sin_addr, heptr_client->h_length);
	inet_telnum_client.sin_family = AF_INET;
	inet_telnum_client.sin_port = htons((u_short) BUF_PORT);	

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
	printf("client-end has received msg: [%s]\n", buffer);

	return 0;
} 

