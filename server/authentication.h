#include "user.h"
int authenticate(User *users[MAX_USER],char *username, char *password);
User* handleLogin(User *users[MAX_USER],char *client_message, int socket,char *result);
int isLogined(User *user);
void sendMessageRequireLogin(int socket);