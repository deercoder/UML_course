#include "buf_mgr.h"
#include <utmpx.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


int main()
{
   // used to generate a random number
   ushort xsub1[3];
   struct timeval randtime;
   int j;
	
   // generate a random 
   gettimeofday(&randtime, (struct timezone *)0);
   xsub1[0] = (ushort) randtime.tv_usec;
   xsub1[1] = (ushort) randtime.tv_usec >> 16;
   xsub1[2] = (ushort) (getpid()); 

   j = nrand48(xsub1) & 3;

   printf("%d\n", j);
   return 0;
}
