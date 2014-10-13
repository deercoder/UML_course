#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>



#define MAX 100
#define MAX_ROW 20
#define MAX_COL 20

void clean(int argc, char **argv);
void printargs(int argc, char **argv);
int makeargs(char *s, char *** argv);
void forkIt(int argc, char **argv);


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
    char **argv = NULL, s[MAX];
    int argc;

    printf("Please enter a command (exit to exit) ");
    fgets(s, MAX, stdin);

    while(strcmp(s, "exit\n") != 0)
    {
        argc = makeargs(s, &argv);
        if(argc != -1)
        {
            printf("There are %d tokens.\nThe tokens are:\n", argc);
            printargs(argc, argv);
        }// end if

        forkIt(argc, argv);

        clean(argc, argv);
        argv = NULL;

        printf("Please enter a command (exit to exit) ");
        fgets(s, MAX, stdin);

    }// end while

}// end main



void forkIt(int argc, char **argv)
{
    pid_t cpid, w;
    int status;
    char tmp[MAX];
    char **arg_list; // need to construct the parameters
    int j = 0;
    int i = 0;

    arg_list = (char **)malloc( MAX_COL * sizeof(char *));
    for(i = 0; i < MAX_COL; i++)
        arg_list[i]=(char *)malloc( MAX_ROW * sizeof(char));

    char str1[] = "ls";
    char str2[] = "-al";
    char str3[] = "/etc/passwd";


    ///// NOTE: must use pointers to the arg_list, not the content, so strcpy has a problem, need just assing
    ///// arg_list[0] = argv[0]; .....
    /*
       strcpy(arg_list[0], argv[0]);
       strcpy(arg_list[1], argv[1]);
       strcpy(arg_list[2], argv[2]); /// this must be const str3, not arg[2], but arg[2] is also /etc/passwd
       */
    i = 0;
    while(argv[argc-1][i] != '\n')
        i++;
    argv[argc-1][i] = 0;	// set an end to the array


    /* set the array parameter list */
    for(i = 0; i < argc; i++){
        arg_list[i] = argv[i];
    }

    /*
       arg_list[0] = argv[0];
       arg_list[1] = argv[1];
       arg_list[2] = argv[2];
       */
    arg_list[argc]  = 0;
    //    printf("%d, %d, %d\n", argc, i, sizeof(str3));


    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {            /* Code executed by child, execute the command */
        printf("Child PID is %ld\n", (long) getpid());

        int result = 0;
        result = execvp(argv[0], arg_list); /// should be OK when constructed.

        /// NOTE: wrong state is handled by parent, here I keep it continue running, just comment exit,
        /// because the question require to continue inputting the commands
        if (result != 0)
            printf("Error when executing the command in child process!\n");
    } else {                    /* Code executed by parent, use waitpid */
        do {
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            /* could print necessary for debugging, these are important state */
            if (WIFEXITED(status)) {
                //printf("exited, status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                //printf("killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                //printf("stopped by signal %d\n", WSTOPSIG(status));
            } else if (WIFCONTINUED(status)) {
                //printf("continued\n");
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        // exit(EXIT_SUCCESS);
    }

}// end forkIt
