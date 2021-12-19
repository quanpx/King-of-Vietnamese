#include <stdio.h>
#include <sys/select.h>
void main()
{
    int maxfd=0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(0,&readfds);

    int ready=select(maxfd+1,&readfds,NULL,NULL,NULL);
    if(ready<0)
    {
        perror("select failed\n");
    }else if(ready==0)
    {
        perror("Time out\n");
    }else
    {
        printf("A key was pressed\n");
    }
}