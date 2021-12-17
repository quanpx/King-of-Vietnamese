#include "user.h"
#include "room.h"
#include "player.h"
int authenticate(User *users, char *username, char *password);
User *handleLogin(char *client_message, int socket);