/*          client.c  a minimal ftp client              */
/*   this version works on all BSD based  systems       */
/*   USAGE:  simple_ftp hostname full_path_name_to_get  */
/*   remote file is stored with root name in CWD        */

#include "ftp.h"
	

int main(int argc, char *argv[])
{
        MSG     msg;
	MBUF	raw;
        int     inet_sock, local_file;
	int	type_val, size_val, read_val, local_size;
        int     i,j,k;
	char    *buffer_ptr, *token_ptr, *last_token_ptr;
	char	full_file_path_name[256];
	union type_size;
        struct sockaddr_in inet_telnum;
	struct hostent *heptr, *gethostbyname();

/***** copy command line argv[2] to full_file_path_name      *****/
/***** for safe keeping and future use                       *****/

	strcpy(full_file_path_name, argv[2]);

/***** allocate a socket to communicate with *****/

        if((inet_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1){
          perror("inet_sock allocation failed: ");
          exit(1);
        }

/***** get a hostent pointer to point to a hostent structure *****/
/***** which contains the remote IP address of server        *****/

	if((heptr = gethostbyname( argv[1] )) == NULL){
	  perror("gethostbyname failed: ");
	  exit(1);
	}

/***** byte copy the IP adress from the h_addr field in the *****/
/***** hostent structure into an IP address structure       *****/

	bcopy(heptr->h_addr, &inet_telnum.sin_addr, heptr->h_length);
	inet_telnum.sin_family = AF_INET;
	inet_telnum.sin_port = htons( (u_short)PORT );

/***** use the connect system call to open a TCP connection *****/

	if(connect(inet_sock, (struct sockaddr *)&inet_telnum, 
                               sizeof(struct sockaddr_in)) == -1){
          perror("inet_sock connect failed: ");
          exit(2);
        }



/***** set up the RECV request message for the server using *****/
/***** argv[2] as the full_path_name of the file to get     *****/

	make_header(&msg, RECV, (local_size=strlen(argv[2]) + 1));
	strcpy(msg.mbody, argv[2]);

/***** use the C library routine strtok() to parse the path *****/
/***** name of the file in order to get the basename to use *****/
/***** as a local file name in current working directory    *****/

	token_ptr = strtok(argv[2], "/");
	do{
	  if((last_token_ptr = token_ptr) == NULL){
	      printf("\n    *** Illegal path name, terminating\n\n");
	      exit(2);
	  }
	} while(token_ptr = strtok(NULL, "/"));

/***** using the basename of the path for a local file name *****/
/***** create a local file by that name and open for RDWR   *****/

	if((local_file=open(last_token_ptr, O_RDWR | O_CREAT | O_TRUNC, 0666))
                                                                       == -1){
	  perror("local_file open failed: ");
	  exit(3);
	}

/***** now send the RECV request message to the server      *****/

	if(write(inet_sock, &msg, local_size + (2*sizeof(int))) == -1){
          perror("inet_sock write failed: ");
          exit(3);
        }

/***** read header info of message returned by server       *****/

	while(1){
	  read_header(inet_sock, &raw.buf);

	  type_val = ntohl(raw.m.mtype);
	  size_val = ntohl(raw.m.msize);
	  read_val = size_val;
	  buffer_ptr   = raw.buf;



/***** what type of message has the server sent to us ??    *****/

	  switch(type_val){
	    case DATA:	

/***** if DATA read into buffer and write to local file     *****/

		converge_read(inet_sock, buffer_ptr, read_val);

	  	if(write(local_file, raw.buf, size_val) == -1){
	  	  perror("local write failed: ");
	  	  exit(3);
	  	}
		break;

	    case END_DATA:

/***** if END_DATA print success message to stdout and exit *****/

		printf("transfer for file %s completed successfully, goodbye\n",
				       full_file_path_name);
		exit(0);

/***** if unsupported kind of packet type exit with error   *****/

	    default:
		printf("unknown message type %d claims size of %d\n",
					type_val,         size_val);
		printf("this is an unrecoverable error, goodbye\n");
		exit(5);
	  } /* end switch */
	}   /* end while  */
}           /* end main   */
