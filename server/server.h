#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include "user.h"
#include "authentication.h"
#include "game.h"
#include "room.h"
#include "player.h"
#include "../utils/utils.h"
#include "../utils/command.h"
#include "question.h"

#define MAX_BUFFER 1024
#define MESS_BUFFER 256

#ifndef STRUCT_SERVER
#define STRUCT_SERVER
typedef struct server
{
	fd_set serverFds;
	pthread_mutex_t *clientMutex;
	int sockfd;
	int numClients;
	int clientSockets[MAX_BUFFER];
	User *activeUsers[MAX_USER];
	Game *game;
  //	Room *rooms[MAX_ROOM];
} Server;
#endif //STRUCT_SERVER

#ifndef STRUCT_CLIENT
#define STRUCT_CLIENT
typedef struct client
{
	int clientSocket;
	Server *server;
} Client;
#endif //STRUCT_CLIENT