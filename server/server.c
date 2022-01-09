#include "server.h"
int num_question = 0;
User *users[MAX_USER];
Room *rooms[MAX_ROOM];
Question *questions[MAX_QUESTION];
int numQuestion = 0;

void bindSocket(struct sockaddr_in *server_addr, int socketFd);
void removeClient(Server *server, int clientSocketFd);
void *clientHandler(void *data);
void *gameHandler(void *data);
void startServer(int server_socket);
void *connection_handler(void *data);
void final_result(Player *player1, Player *player2, char *win_result, char *lose_result);

int main(int argc, char **argv)
{
	readUsersFromFile(users, "../file/user.txt");
	readRoomsFromFile(rooms);
	// readUsersFromFile(&users, "../file/user.txt");
	// room = initRoom();
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
	if (listen(server_socket, 5) != 0)
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
void configServer(Server *server, int server_socket)
{
	server->numClients = 0;
	server->clientMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	server->sockfd = server_socket;
	makeUsersNull(server->activeUsers);
	// makeRoomsNull(server.rooms);

	pthread_mutex_init(server->clientMutex, NULL);
}
void startServer(int server_socket)
{

	// create
	Game *game = initGame();
	// config server
	Server server;
	server.game = game;
	configServer(&server, server_socket);
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

			printf("Server accepted new client. Socket: %d\n", clientSocketFd);
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
	char message[MAX_BUFFER];
	char win_message[MESS_BUFFER];
	char lose_message[MESS_BUFFER];
	Server *server = (Server *)data;

	int *clientSockets = server->clientSockets;
	Game *game = server->game;
	Room *room = game->room;
	char result[80];
	char win_result[256];
	char lose_result[256];
	while (1)
	{
		room = (Room *)game->room;
		bzero(message, MESS_BUFFER);
		pthread_mutex_lock(game->mutex);
		while (game->isPlay == 0)
		{
			pthread_cond_wait(game->cond_players, game->mutex);
		}
		pthread_mutex_unlock(game->mutex);

		Question *quest = game->questions[num_question];
		if (quest == NULL)
		{
			printRoom(room);
			bzero(message, MAX_BUFFER);
			bzero(lose_result, 256);
			bzero(win_result, 256);
			bzero(win_message, MAX_BUFFER);
			bzero(lose_message, MAX_BUFFER);
			final_result(room->players[0], room->players[1], win_result, lose_result);
			modify_message(RESULT, win_result, win_message);
			modify_message(RESULT, lose_result, lose_message);

			if (room->players[0]->point > room->players[1]->point)
			{

				// win
				write(room->players[0]->socket, win_message, MESS_BUFFER - 1);

				// lose
				write(room->players[1]->socket, lose_message, MESS_BUFFER - 1);
			}
			else if (room->players[0]->point < room->players[1]->point)
			{
				// win
				write(room->players[0]->socket, lose_message, MESS_BUFFER - 1);

				// lose
				write(room->players[1]->socket, win_message, MESS_BUFFER - 1);
			}
			else
			{
				for (int i = 0; i < server->numClients; i++)
				{
					int socket = clientSockets[i];
					if (socket != 0 && write(socket, win_message, MESS_BUFFER - 1) == -1)
						perror("Socket write failed: ");
				}
			}

			modify_message(MESSG, "Game finished!", message);

			for (int i = 0; i < server->numClients; i++)
			{
				int socket = clientSockets[i];
				if (socket != 0 && write(socket, message, MESS_BUFFER - 1) == -1)
					perror("Socket write failed: ");
			}
			pthread_mutex_lock(game->mutex);
			game->room->flag=0;
			game->isPlay = 0;
			num_question = 0;
			pthread_cond_signal(game->cond_players);
			pthread_mutex_unlock(game->mutex);
		}
		else
		{
			pthread_mutex_unlock(game->mutex);
			fprintf(stderr, "Broadcasting message: %s\n", quest->quest);
			modify_message(QUEST, quest->quest, message);

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
	}

	return NULL;
}

void *clientHandler(void *data)
{
	Client *client = (Client *)data;
	Server *server = (Server *)client->server;
	Game *game = (Game *)server->game;
	Question *question = NULL;
	User *user = NULL;
	Room *room = NULL;
	Player *player = NULL;
	int clientSockfd = client->clientSocket;
	char message[MESS_BUFFER];
	char result[256];
	char num_in_string[5];
	int room_in_digit;
	char rooms_info[256];
	char socketStr[2];
	while (1)
	{
		bzero(message, MESS_BUFFER);
		read(clientSockfd, message, MESS_BUFFER - 1);
		printf("%s\n", message);
		Message *mess = split_message(message);

		switch (mess->cmdType)
		{
		case LOGIN:
			bzero(result, 256);
			user = handleLogin(mess->body, clientSockfd, result);
			pthread_mutex_lock(server->clientMutex);
			addUser(server->activeUsers, user);
			pthread_mutex_unlock(server->clientMutex);
			modify_message(LOGIN, result, message);
			write(clientSockfd, message, strlen(message));
			break;
		case LOGOT:
			fprintf(stderr, "Client on socket %d has disconnected.\n", clientSockfd);
			removeClient(server, clientSockfd);
			break;
		case CRTRM:
			room = createRoom();
			pthread_mutex_lock(game->mutex);
			game->room = room;
			pthread_mutex_unlock(game->mutex);
			addRoom(rooms, room);
			// pthread_mutex_lock(room->game->mutex);
			// pthread_mutex_unlock(game->mutex);
			user = searchUserBySocket(users, clientSockfd);
			player = initPlayer(user->username, user->socket);
			player->role = OWNER;
			addPlayerToRoom(room, player);
			bzero(message, MESS_BUFFER);
			convert_number_to_string(room->roomid, num_in_string);
			modify_message(CRTRM, num_in_string, message);
			write(clientSockfd, message, strlen(message));
			fprintf(stdout, "Client %d request create a room.\n", clientSockfd);
			break;
		case GETRM:
			bzero(message, MESS_BUFFER);
			bzero(rooms_info, MESS_BUFFER);
			rooms_to_string(rooms, rooms_info);
			modify_message(GETRM, rooms_info, message);
			write(clientSockfd, message, strlen(message));
			break;

		case JOINR:

			room_in_digit = atoi(mess->body);
			room = searchRoom(rooms, room_in_digit);
			if (room == NULL)
			{
				bzero(message, MESS_BUFFER);
				modify_message(MESSG_NOT_FOUND, "Không tìm thấy!", message);
				write(clientSockfd, message, strlen(message));
			}
			else
			{
				pthread_mutex_lock(game->mutex);
				game->room = room;
				pthread_mutex_unlock(game->mutex);
				user = searchUserBySocket(users, clientSockfd);
				player = initPlayer(user->username, user->socket);
				pthread_mutex_lock(game->mutex);
				addPlayerToRoom(room, player);
				pthread_mutex_unlock(game->mutex);
				bzero(message, MESS_BUFFER);
				modify_message(JOINR, mess->body, message);
				write(clientSockfd, message, strlen(message));

				bzero(message, MESS_BUFFER);
				modify_message(OPONENT_JOIN, "Người chơi tham gia phòng!\n Nhập 'start' để bắt đầu!", message);
				for (int i = 0; i < room->no_player; i++)
				{
					if (room->players[i] != NULL)
					{
						int socket = room->players[i]->socket;
						if (socket != 0)
						{
							if (socket != clientSockfd && write(socket, message, MESS_BUFFER - 1) == -1)
								perror("Socket write failed: ");
						}
					}
				}
			}
			break;

		case STATG:
			room_in_digit = atoi(mess->body);
			room = searchRoom(rooms, room_in_digit);
			printRoom(room);
			if (room->no_player < 2)
			{
				bzero(message, MESS_BUFFER);
				modify_message(WAIT, "Chưa đủ người để bắt đầu !", message);
				write(clientSockfd, message, strlen(message));
			}
			else
			{
				if (room->flag < 1)
				{
					pthread_mutex_lock(game->mutex);
					room->flag++;
					pthread_mutex_unlock(game->mutex);
					bzero(message, MESS_BUFFER);
					modify_message(WAIT, "Đợi người chơi sẵn sàng!", message);
					write(clientSockfd, message, strlen(message));
					bzero(message, MESS_BUFFER);
					modify_message(READY, "Đối thủ đã sẵn sàng!", message);
					for (int i = 0; i < room->no_player; i++)
					{
						int socket = room->players[i]->socket;
						if (socket != 0)
						{
							if (socket != clientSockfd && write(socket, message, MESS_BUFFER - 1) == -1)
								perror("Socket write failed: ");
						}
					}
					break;
				}
				else
				{
					bzero(message, MESS_BUFFER);
					modify_message(MESSG, "start", message);
					for (int i = 0; i < room->no_player; i++)
					{
						int socket = room->players[i]->socket;
						if (socket != 0 && write(socket, message, MESS_BUFFER - 1) == -1)
							perror("Socket write failed: ");
						printf("send to %d\n", socket);
					}
					pthread_mutex_lock(game->mutex);
					while (game->isPlay == 1)
					{
						pthread_cond_wait(game->cond_players, game->mutex);
					}
					game->isPlay = 1;
					pthread_cond_signal(game->cond_players);
					pthread_mutex_unlock(game->mutex);
				}
			}

			break;
		case ANSWR:
			pthread_mutex_lock(game->mutex);
			question = game->questions[num_question];
			pthread_mutex_unlock(game->mutex);
			printQuestion(question);
			printf("%s\n", mess->body);
			if (strcmp(question->answer, mess->body) == 0)
			{
				bzero(result, MESS_BUFFER);
				bzero(socketStr, 2);
				sprintf(socketStr, "%d", clientSockfd);
				strcat(result, "Client ");
				strcat(result, socketStr);
				strcat(result, " answered correctly!");
				bzero(message, MESS_BUFFER);
				modify_message(ANSWR, result, message);
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
				modify_message(ANSWR, "Correct!", message);
				player = getPlayerBySocket(room->players, clientSockfd);
				pthread_mutex_lock(game->mutex);
				updatePoint(player);
				pthread_mutex_unlock(game->mutex);
				printPlayer(player);
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
				modify_message(ANSWR, "Inorrect!", message);
				write(clientSockfd, message, strlen(message));
			}
			break;
		case BACK:

			bzero(message, MESS_BUFFER);
			modify_message(BACK_OK, "BACK_OK", message);
			write(clientSockfd, message, MESS_BUFFER - 1);
			for (int i = 0; i < room->no_player; i++)
			{
				if (room->players[i]->socket == clientSockfd)
				{
					pthread_mutex_lock(game->mutex);
					room->players[i] = NULL;
					room->no_player--;
					pthread_mutex_unlock(game->mutex);
					break;
				}
			}
			bzero(message, MESS_BUFFER);
			modify_message(BACK, "Người chơi đã thoát!", message);
			for (int i = 0; i < room->no_player; i++)
			{
				if (room->players[i] != NULL)
				{
					int socket = room->players[i]->socket;
					if (socket != 0)
					{
						if (write(socket, message, MESS_BUFFER - 1) == -1)
							perror("Socket write failed: ");
					}
				}
			}
			break;
		default:
			break;
		}
		bzero(message, MESS_BUFFER);
	}
}
void final_result(Player *player1, Player *player2, char *win_result, char *lose_result)
{
	printPlayer(player1);
	printPlayer(player2);
	char point[2];
	if (player1->point > player2->point)
	{

		// win
		bzero(point, 2);
		strcat(win_result, "You won with ");
		sprintf(point, "%d", player1->point);
		strcat(win_result, point);
		strcat(win_result, " points");

		// lose
		bzero(point, 2);
		strcat(lose_result, "You losed with ");
		sprintf(point, "%d", player2->point);
		strcat(lose_result, point);
		strcat(lose_result, " points");
		printf("Player %s won! With %d points \n", player1->username, player1->point);
	}
	else if (player1->point < player2->point)
	{
		// win
		bzero(point, 2);
		strcat(win_result, "You won with ");
		sprintf(point, "%d", player2->point);
		strcat(win_result, point);
		strcat(win_result, " points");

		// lose
		bzero(point, 2);
		strcat(lose_result, "You losed with ");
		sprintf(point, "%d", player1->point);
		strcat(lose_result, point);
		strcat(lose_result, " points");
		printf("Player %s won! With %d points\n", player2->username, player2->point);
	}
	else
	{
		strcpy(lose_result, "Draw!");
		strcpy(win_result, "Draw!");
		printf("Draw with %d - %d\n", player1->point, player2->point);
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
