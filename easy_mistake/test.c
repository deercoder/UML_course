#include <stdio.h>

int main()
{
   char s[9] = {};	
   int a = 222;

   snprintf(s, 6, "%d\n\n", a);
   printf("%s", s);
   return 0;
}
