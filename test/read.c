#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
void main()
{
    char buff[20];
    read(0,buff,20);
    printf("%s\n",buff);
    printf("%d\n",strlen(buff));
}