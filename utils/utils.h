#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct message
{
    int cmd;
    int length;
    char body[256];
}Message;
Message* split_message(char *mess);
void modify_message(int cmd, char *body, char *message);