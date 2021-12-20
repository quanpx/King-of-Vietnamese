#include "utils.h"
Message* split_message(char *mess)
{
    Message *message=(Message*)malloc(sizeof(Message));

   char *token=strtok(mess,"|");
   message->cmdType=atoi(token);
   token=strtok(NULL,"|");
   message->length=atoi(token);
   token=strtok(NULL,"|");
   strcpy(message->body,token);
   return message;

}
void modify_message(int cmd, char *body, char *message)
{
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
