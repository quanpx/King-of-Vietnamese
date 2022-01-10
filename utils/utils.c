#include "utils.h"
Message *split_message(char *mess)
{
    Message *message = (Message *)malloc(sizeof(Message));

    char *token = strtok(mess, "|");
    message->cmdType = atoi(token);
    token = strtok(NULL, "|");
    message->length = atoi(token);
    token = strtok(NULL, "|");
    strcpy(message->body, token);
    return message;
}
void modify_message(int cmd, char *body, char *message)
{
    bzero(message,256);
    char cmdType[5];
    char length[5];
    sprintf(length, "%ld", strlen(body));
    sprintf(cmdType, "%d", cmd);
    strcat(message, cmdType);
    strcat(message, "|");
    strcat(message, length);
    strcat(message, "|");
    strcat(message, body);

    // cmd|length|body
}
void convert_number_to_string(int number, char *num_in_string)
{
    sprintf(num_in_string, "%d", number);
}
char *split_command(char *command, char *type, char *body)
{
    // char *token = strtok(command, " ");
    // strcpy(type, token);
    // token = strtok(NULL, " ");
    // if (token != NULL)
    // {
    //     strcpy(body, token);
    // }
    int i;
    for(i=0;i<strlen(command);i++)
    {
       if(command[i]!=' ')
       {
           type[i]=command[i];
       }else
       {
           break;
       }
        
    }
    int k=0;
    for(int j=i+1;j<strlen(command);j++)
    {
        body[k]=command[j];
        k++;
    }
}
// void main()
// {
//     char body[10];
//     char type[10];
//     bzero(body, 10);
//     bzero(type, 10);
//     char command[40] = "answer con cua";
//     split_command(command, type, body);
//     printf("%s \n%s\n", type, body);
// }