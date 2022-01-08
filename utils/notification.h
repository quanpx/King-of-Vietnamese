#include<stdio.h>
#include<stdlib.h>
#define MAX_BUFFER 1024
#define MAX_TYPE_NOTI 100
typedef enum noti_type
{
    LOGIN_SUCCESS,
    LOGIN_FAILED,
    ACCOUNT_NOT_EXIST,
    WON,
    LOSED,

}noti_type;

char *getNotification(noti_type type);