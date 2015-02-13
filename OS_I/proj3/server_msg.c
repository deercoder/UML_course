/********  message queue server  *********/


#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <stdio.h>

#define MSGKEY  (1234567)

typedef struct{
   long mtype;
   char mtext[252];
} MSG;

int msg_id;

void  sig_handler(int signum){
  printf("going down on signal %d\n", signum);
  msgctl(msg_id, IPC_RMID, 0);
  exit(1);
}

int main(int argc, char * argv[]){
  MSG amessage;
  int *int_ptr, pid, msg_type= -10;
  struct sigaction   new_act;
  sigset_t   mask_sigs;
  int  i, nsigs;

   int sigs[] = {SIGHUP,SIGINT,SIGQUIT, SIGPIPE,
                 SIGTERM, SIGBUS, SIGSEGV, SIGFPE};

   nsigs = sizeof(sigs)/sizeof(int);
   sigemptyset(&mask_sigs);
   for(i=0; i< nsigs; i++)
       sigaddset(&mask_sigs, sigs[i]);
   for(i=0; i< nsigs; i++){
      new_act.sa_handler = sig_handler;
      new_act.sa_mask = mask_sigs;
      new_act.sa_flags = SA_RESTART;
      if(sigaction(sigs[i], &new_act, NULL) == -1){
         perror("can't set signals: ");
         exit(1);
      }
   }

  if((msg_id = msgget((key_t)MSGKEY, IPC_CREAT | 0666)) == -1){
     perror("msgget failed :");
     exit(1);
  }
  for(;;){
    if(msgrcv(msg_id, &amessage,
            sizeof(amessage.mtext), msg_type, 0) == -1){
       perror("msgrcv failed :");
       exit(1);
    }
    printf("Client message type is %d and pid is %d\n", *(long *)&amessage.mtype, (pid = *(int *)amessage.mtext));
    printf("Client message is:  %s\n", (amessage.mtext + 4));
    amessage.mtype = (long)pid;

    strcpy(amessage.mtext, "THIS IS THE SERVER MESSAGE\n");

    if(msgsnd(msg_id, &amessage,
            sizeof(amessage.mtext), 0) == -1){
       perror("msgsnd failed :");
       exit(1);
    }
  }
}
