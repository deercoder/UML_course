/*    server.c  a minimal ftp      */

#include "ftp.h"

static int  test_id = 0;
	
void child_handler(int signum){
wait(NULL);
}

int main(int argc, char *argv[])
{
        MSG     		msg;
	MBUF			raw;
        int    	 		inet_sock, new_sock, local_file;
	int			type_val, size_val, read_val, sleep_interval;
        int     		i,j,k;
	socklen_t		fromlen;
	char    		*buffer_ptr;
        struct sockaddr_in 	inet_telnum;
	struct hostent 		*heptr, *gethostbyname();
	int			wild_card = INADDR_ANY;
	struct sigaction 	sigstrc;
	sigset_t		mask;

/***** set up sigaction structure to clean zombies *****/

	sigemptyset(&mask);

	sigstrc.sa_handler = child_handler;
	sigstrc.sa_mask = mask;
	sigstrc.sa_flags = 0;

	sigaction(SIGCHLD, &sigstrc, NULL);

/***** if there's an argument on the command line  *****/
/***** use it for a test sleep interval (testing)  *****/

	if(argc == 2)
          sleep_interval = atoi(argv[1]);
	else 
	  sleep_interval = 1;

/***** allocate a socket to communicate with *****/

        if((inet_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1){
          perror("inet_sock allocation failed: ");
          exit(1);
        }

/***** byte copy the wild_card IP address INADDR_ANY into   *****/
/***** IP address structure, along with port and family and *****/
/***** use the structure to give yourself a connect address *****/

	bcopy(&wild_card, &inet_telnum.sin_addr, sizeof(int));
	inet_telnum.sin_family = AF_INET;
	inet_telnum.sin_port = htons( (u_short)PORT );

	if(bind(inet_sock, (struct sockaddr *)&inet_telnum, 
                            sizeof(struct sockaddr_in)) == -1){
          perror("inet_sock bind failed: ");
          exit(2);
        }

/***** allow client connect requests to arrive: call-wait 5 *****/

        listen(inet_sock, 5);



/***** forever more, answer the phone and create a child to *****/
/***** handle each client connection (use EINTR check)      *****/

        for(;;){  /* forever */

/***** set sizeof(struct sockaddr) into fromlen to specify  *****/
/***** original buffer size for returned address (the       *****/
/***** actual size of the returned address then goes here)  *****/

	 fromlen = sizeof(struct sockaddr);

         while((new_sock = accept(inet_sock, (struct sockaddr *)&inet_telnum, 
                                          &fromlen)) == -1 && errno == EINTR);
         if(new_sock == -1){
		perror("accept failed: ");
		exit(2);
	 }
	 switch(fork()){

/***** parent takes this case if fork succeeds               *****/

	   default:  close(new_sock);
		  break;

/***** parent takes this case if fork fails                  *****/

	   case -1:  perror("fork failed: ");
		  exit(1);

/***** child takes this case if fork succeeds                *****/

	   case  0:  close(inet_sock);
             while(1){

/***** read header info of message sent by client            *****/

	       read_header(new_sock, &raw.buf);

               type_val = ntohl(raw.m.mtype);
               size_val = ntohl(raw.m.msize);
               read_val = size_val;
               buffer_ptr   = raw.buf;

/***** what type of message has the client sent to us ??     *****/

               switch(type_val){
                case RECV: 
		   converge_read(new_sock, buffer_ptr, read_val);

/***** this is a RECV message with body holding path name    *****/
/***** so use string in body to open the file for transfer   *****/

                   if((local_file = open(raw.buf, O_RDONLY, 0)) == -1){
                      perror("local open failed: ");
                      exit(3);
                   }

                   while(1){
		    switch(j=read(local_file, msg.mbody, MSG_BODY)){

/***** if data read from path set up packet, send to client  *****/

			default:  make_header(&msg, DATA, j);
				  if(write(new_sock, &msg, (2*sizeof(int)+j))
									== -1){
				    perror("new_sock write failed: ");
				    exit(3);
				  }
				  break;




/***** if EOF read from path send END_DATA packet to client   *****/

		        case  0:  make_header(&msg, END_DATA, 0);
                                  if(write(new_sock, &msg, (2*sizeof(int)))
                                                                        == -1){
                                    perror("new_sock write failed: ");
                                    exit(3);
                                  }
				  printf("server child finished sending file %s\n", raw.buf);
				  exit(0);

                                  break;


/***** if -1 from read problem with file, print error, exit   *****/

			case -1:  perror("local_file read failed: ");
				  exit(3);
		     }
		   }

/***** TEST cases not used by normal client, testing only     *****/

	       case TEST:
		converge_read(new_sock, buffer_ptr, read_val);

		printf("server child received test packet number %d as #%d\n", 
					*((int *)raw.buf),	test_id++);
		sleep(sleep_interval);
		break;

	       case END_TEST:
		printf("server child finished test run, goodbye\n\n");
		exit(0);

/***** unknown client message is an unrecoverable child error *****/

               default:
                printf("unknown message type %d claims size of %d\n",
                                        type_val,         size_val);
                printf("this is an unrecoverable error, goodbye\n");
                exit(5);
              }
        }
      }
   }
}


