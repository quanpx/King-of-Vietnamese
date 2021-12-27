#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef STRUCT_MESSAGE
#define STRUCT_MESSAGE
typedef struct message
{
    int cmdType;
    int length;
    char body[256];
}Message;

#endif //STRUCT_MESSAGE 
Message* split_message(char *mess);

void modify_message(int cmd, char *body, char *message);
void convert_number_to_string(int number,char *num_in_string);