#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const int MAX=1000;

void strip(char *s)
{
    int len;
    len = (int) strlen(s);
    if(s[len - 2] == '\r')
        s[len -2] = '\0';
    
    else if(s[len - 1] == '\n')
        s[len -1] = '\0';
}// end strip

void clean(int argc, char **argv)
{
    int i;
    for (i = 0; i < argc; i++) {
        free(argv[i]);
        argv[i] = NULL;
    }
    
    free(argv);
}

void printargs(int argc, char **argv)
{
	int x;
	for(x = 0; x < argc; x++)
		printf("argv[%d] would be %s\n", x, argv[x]);
    
}

int makeargs(char *s, char *** argv)
{
    // http://www.cplusplus.com/reference/clibrary/cstring/strtok/
    int length = (int) strlen(s);
    if (length < 1) {
        return -1; // no args
    }
    
    int i;
    int count = 1; // count starts at 1 as we assume there is always one arguemnt
    for (i = 0; i < length; i++) {
        if (s[i] == ' ') {
            count++;
        }
    }
    
    *argv = malloc(sizeof(char *) * count);
    char out_ptr[MAX];
    char * token = strtok_r(s, " ", (char **)&out_ptr);
    i = 0;
    while (token != NULL) {
        (*argv)[i++] = strdup(token);
        token = strtok_r(NULL, " ", (char **)&out_ptr);
    }

    return count;
    
}// end makeArgs

void promptInput(char * s)
{
	printf("Please enter strings seperated by a single space (type 'q' to quit): ");
	fgets(s, MAX, stdin);
	strip(s);
}

int main()
{
    char **argv = NULL;
    char userString[MAX];
    int argc;

    promptInput(userString);
    while (strcmp(userString, "q") != 0) {
	argc = makeargs(userString, &argv);
	if(argc != -1)
	{
	        printf("There are %d tokens.\nThe tokens are:\n", argc);
	        printargs(argc, argv);
        
	}
	else
	{
		printf("No arugments");
	}
		
	printf("\n");
	promptInput(userString);

		// Clean input each time or some blocks will be lost because we will not 
		// have the correct count or pointers to free
	clean(argc, argv);
	argv = NULL;
    }
    
    printf("exiting...\n");
    
    return 0;
}// end main
