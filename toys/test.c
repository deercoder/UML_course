#include <stdio.h>

#define MAX 100
#define MAX_ROW 20
#define MAX_COL 20

char str1[] = "ls";
char str2[] = "-al";
char str3[] = "/etc/passwd";


void test()
{
    char *argv[] = {"ls", "-al", "/etc/passwd", 0};
    execvp("ls", argv);
}

void test2()
{
    char *str = 0;
}


int main()
{
    char **arg_list; // need to construct the parameters
    int j = 0;
    int i = 0;

    arg_list = (char **)malloc( MAX_COL * sizeof(char *));
    for(i = 0; i < MAX_COL; i++)
        arg_list[i]=(char *)malloc( MAX_ROW * sizeof(char));

    strcpy(arg_list[0], str1);
    strcpy(arg_list[1], str2);
    strcpy(arg_list[2], str3);

    arg_list[3]  = 0;
    /*
       for(i = 0; i < MAX_ROW; i++)
       for(j = 0; j < MAX_COL; j++)
       {
       if (arg_list[i][j] != 0)
       printf("%c", arg_list[i][j]);
       }
       */
    printf("\n-------------------------\n");
    //test();
    //test2();
    execvp("ls", arg_list);

    return 0;

}
