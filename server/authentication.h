#include "user.h"
#include "room.h"
#include "player.h"
int authenticate(User *users, char *username, char *password);
void handleLogin(char *client_message, int socket,char *result);