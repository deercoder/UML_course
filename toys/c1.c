#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int MAX=1000;

void strip(char *s)
{
	int len;
	len = strlen(s);
	if(s[len - 2] == '\r')
		s[len -2] = '\0';

	else if(s[len - 1] == '\n')
		s[len -1] = '\0';
}// end strip

void clean(int argc, char **argv)
{
	int i;
	for (i = 0; i < argc; i++){
		free(argv[i]);
		argv[i] = NULL;
	}
	free(argv);
}

void printargs(int argc, char **argv)
{
	int x;
	for(x = 0; x < argc; x++)	
		printf("%s\n", argv[x]);
}

int makeargs(char *s, char *** argv)
{
	int len = (int)strlen(s);	
	if(len < 1){
		return -1;
	}
	
	int i;
	int tokenCount = 1; 
	for (i = 0; i < len; i++){
		if(s[i]==' ') tokenCount++;
	}	
	
	(*argv) = calloc(tokenCount, sizeof(char*));
	char *token = strtok(s, " ");
	
	//(*argv) = calloc(len+1, sizeof(char));
	i = 0;
	while(token != NULL){
		(*argv)[i++]=strdup(token);
		token=strtok(NULL, " ");
	}
	
    return tokenCount;

}// end makeArgs
	

int main(int argc, char **argv)
{
    int count_num = 0;
    int sleep_time = 0;
    int i = 0; 

    if(argc == 3) // 3 parameters, ./a.out 4 10
    {
        //printf("There are %d tokens.\nThe tokens are:\n", argc);
	sleep_time = atoi(argv[1]);
	count_num = atoi(argv[2]);
	//printf("sleep_time = %d, count = %d", sleep_time, count_num);
    }
    else 
    {
	printf("No argument or argument number is not correct.\n");
    }
	
    for(i = 1; i <= count_num; i++){
		printf("Executing c1, process id: %d, iteration number: %d\n", (int)getpid(), i);
		sleep(sleep_time);
    }
	
   printf("c1 is now exiting");

}// end main
