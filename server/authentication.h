#include "user.h"
int authenticate(char *username, char *password);
User* handleLogin(char *client_message, int socket,char *result);