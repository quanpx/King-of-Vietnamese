#include "authentication.h"
#include "game.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../utils/utils.h"
#include "../utils/command.h"
#include <fcntl.h>
#define MAX_BUFFER 1024
#define MESS_BUFFER 256

User *users = NULL;
Room *room = NULL;
int num_question=0;
typedef struct game
{
	Question *questions[MAX_QUESTION];
	int isPlay;
	int isAnswered;
	pthread_cond_t *cond_players;
	pthread_cond_t *cond_answered;
	pthread_mutex_t *mutex;
} Game;
typedef struct server
{
	fd_set serverFds;
	pthread_mutex_t *clientMutex;
	int sockfd;
	int numClients;
	int clientSockets[MAX_BUFFER];
	Game *game;
} Server;
typedef struct client
{
	int clientSocket;
	Server *server;
} Client;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// Xử lý chơi game
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void bindSocket(struct sockaddr_in *server_addr, int socketFd);
void removeClient(Server *server, int clientSocketFd);

void *clientHandler(void *data);
void *gameHandler(void *data);
void startServer(int server_socket);
void *connection_handler(void *data);

// Remember to use -pthread when compiling this server
int main(int argc, char **argv)
{
	readUsersFromFile(&users, "../file/user.txt");
	room = initRoom();
	//Đọc user từ file
	struct sockaddr_in server_addr;
	int yes = 1;
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
	if (server_socket == -1)
	{
		perror("Socket initialisation failed");
		exit(EXIT_FAILURE);
	}
	else
		printf("Server socket created successfully\n");

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// bind the socket to the specified IP addr and port
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
	{
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");
	// bindSocket(&server_addr, server_socket);
	if (listen(server_socket, 3) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");

	startServer(server_socket);

	close(server_socket);
	return 0;
}
void startServer(int server_socket)
{
	// create game
	Game *game = (Game *)malloc(sizeof(Game));
	getQuestions(game->questions, "../file/question.txt");
	game->isPlay = 0;
	game->isAnswered = 0;
	game->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (game->mutex == NULL)
	{
		printf("Allocate failed!\n");
		exit(0);
	}
	pthread_mutex_init(game->mutex, NULL);
	game->cond_players = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	if (game->cond_players == NULL)
	{
		printf("Allocate failed!\n");
		exit(0);
	}
	pthread_cond_init(game->cond_players, NULL);
	game->cond_answered = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	if (game->cond_answered == NULL)
	{
		printf("Allocate failed!\n");
		exit(0);
	}
	pthread_cond_init(game->cond_answered, NULL);
	// config server
	Server server;
	server.numClients = 0;
	server.clientMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	server.sockfd = server_socket;
	server.game = game;
	pthread_mutex_init(server.clientMutex, NULL);

	pthread_t connection_handler_thread;
	if (pthread_create(&connection_handler_thread, NULL, (void *)&connection_handler, (void *)&server) == 0)
	{
		fprintf(stderr, "Connection handler started\n");
	}

	FD_ZERO(&(server.serverFds));
	FD_SET(server.sockfd, &(server.serverFds));

	pthread_t game_handler_thread;
	if (pthread_create(&game_handler_thread, NULL, (void *)&gameHandler, (void *)&server) == 0)
	{
		fprintf(stderr, "Game handler started\n");
	}
	pthread_join(connection_handler_thread, NULL);
	pthread_join(game_handler_thread, NULL);

	pthread_mutex_destroy(server.clientMutex);
	free(server.clientMutex);
}

void *connection_handler(void *data)
{
	Server *server = (Server *)data;
	while (1)

	{
		int clientSocketFd = accept(server->sockfd, NULL, NULL);
		if (clientSocketFd > 0)
		{
			fprintf(stderr, "Server accepted new client. Socket: %d\n", clientSocketFd);

			// Obtain lock on clients list and add new client in
			pthread_mutex_lock(server->clientMutex);
			if (server->numClients < MAX_BUFFER)
			{

				for (int i = 0; i < MAX_BUFFER; i++)
				{
					if (!FD_ISSET(server->clientSockets[i], &(server->serverFds)))
					{
						server->clientSockets[i] = clientSocketFd;
						i = MAX_BUFFER;
					}
				}

				FD_SET(clientSocketFd, &(server->serverFds));

				// Spawn new thread to handle client's messages
				Client client;

				client.clientSocket = clientSocketFd;
				client.server = server;
				pthread_t clientThread;
				if ((pthread_create(&clientThread, NULL, (void *)&clientHandler, (void *)&client)) == 0)
				{
					server->numClients++;
					fprintf(stderr, "Client has joined chat. Socket: %d\n", clientSocketFd);
				}
				else
					close(clientSocketFd);
			}
			pthread_mutex_unlock(server->clientMutex);
		}
		// Add new client to list
	}
}

void *gameHandler(void *data)
{
	char message[MESS_BUFFER];
	Server *server = (Server *)data;
	// Server *server = game->server;
	Game *game = server->game;

	int *clientSockets = server->clientSockets;
	while (1)
	{
		bzero(message, MESS_BUFFER);
		pthread_mutex_lock(game->mutex);
		while (game->isPlay == 0)
		{
			pthread_cond_wait(game->cond_players, game->mutex);
		}
		Question *quest = game->questions[num_question];
		pthread_mutex_unlock(game->mutex);
		fprintf(stderr, "Broadcasting message: %s\n", quest->quest);
		modify_message(QUESTION, quest->quest, message);
		for (int i = 0; i < server->numClients; i++)
		{
			int socket = clientSockets[i];
			if (socket != 0 && write(socket, message, MESS_BUFFER - 1) == -1)
				perror("Socket write failed: ");
		}
		pthread_mutex_lock(game->mutex);

		while (game->isAnswered == 0)
		{
			pthread_cond_wait(game->cond_answered, game->mutex);
		}
		game->isAnswered = 0;
		pthread_mutex_unlock(game->mutex);
	}

	return NULL;
}
void *clientHandler(void *data)
{
	Client *client = (Client *)data;
	Server *server = (Server *)client->server;
	Game *game = (Game *)server->game;
	Question *question;
	int clientSockfd = client->clientSocket;
	char message[MESS_BUFFER];
	char result[256];
	char socketStr[2];
	while (1)
	{
		bzero(message, MESS_BUFFER);
		read(clientSockfd, message, MESS_BUFFER - 1);
		Message *mess = split_message(message);
		switch (mess->cmdType)
		{
		case LOGIN:
			bzero(result, 256);
			handleLogin(mess->body, clientSockfd, result);
			modify_message(LOGIN, result, message);
			write(clientSockfd, message, strlen(message));
			break;
		case LOGOUT:
			fprintf(stderr, "Client on socket %d has disconnected.\n", clientSockfd);
			removeClient(server, clientSockfd);
			break;
		case START:
			if (server->numClients < 2)
			{
				printf("Wait for someone!\n");
				bzero(message, MESS_BUFFER);
				modify_message(MESSAGE, "wait", message);
				write(clientSockfd, message, strlen(message));
			}
			else
			{
				bzero(message, MESS_BUFFER);
				modify_message(MESSAGE, "start", message);
				for (int i = 0; i < server->numClients; i++)
				{
					int socket = server->clientSockets[i];
					if (socket != 0 && write(socket, message, MESS_BUFFER - 1) == -1)
						perror("Socket write failed: ");
				}
				game->isPlay = 1;
				pthread_cond_signal(game->cond_players);
			}

			break;
		case ANSWER:
			question = game->questions[num_question];
			printQuestion(question);
			printf("%s\n",mess->body);
			if (strcmp(question->answer, mess->body) == 0)
			{
				bzero(result, MESS_BUFFER);
				sprintf(socketStr, "%d", clientSockfd);
				strcat(result, "Client ");
				strcat(result, socketStr);
				strcat(result, " ansered correctly!");
				bzero(message,MESS_BUFFER);
				modify_message(ANSWER, result, message);
				for (int i = 0; i < server->numClients; i++)
				{
					int socket = server->clientSockets[i];
					if (socket != 0 && socket != clientSockfd)
					{
						write(socket, message, strlen(message));
					}
				}
				bzero(message, MESS_BUFFER);
				bzero(result, 256);
				modify_message(ANSWER, "Correct!", message);
				write(clientSockfd, message, strlen(message));
				
				pthread_mutex_lock(game->mutex);
				num_question++;
				game->isAnswered = 1;
				pthread_mutex_unlock(game->mutex);
				pthread_cond_signal(game->cond_answered);
			}
			else
			{
				bzero(message, MESS_BUFFER);
				modify_message(ANSWER, "Inorrect!", message);
				write(clientSockfd, message, strlen(message));
			}

		default:
			break;
		}
		bzero(message, MESS_BUFFER);
	}
}
void removeClient(Server *server, int clientSocketFd)
{
	pthread_mutex_lock(server->clientMutex);
	for (int i = 0; i < MAX_BUFFER; i++)
	{
		if (server->clientSockets[i] == clientSocketFd)
		{
			server->clientSockets[i] = 0;
			close(clientSocketFd);
			server->numClients--;
			break;
		}
	}
	pthread_mutex_unlock(server->clientMutex);
}
