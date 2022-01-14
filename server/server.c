#include "server.h"
int num_question = 0;
User *users[MAX_USER];
Room *rooms[MAX_ROOM];
Question *questions[MAX_QUESTION];
int num_rooms;
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
	for (int i = 0; i < MAX_BUFFER; i++)
	{
		server->clientSockets[i] = -1;
	}

	pthread_mutex_init(server->clientMutex, NULL);
}
void startServer(int server_socket)
{

	// create 1 biến trò chơi
	Game *game = initGame();

	// cấu hình server
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

				// Tạo ra luồng mời đễ xử lý yêu cầu từ client
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

/*
	Luồng xử lý game
	có tham số là data là dữ liệu từ biến server
*/
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

		// Nếu chưa chơi, thì sẽ tạm block luồng này
		pthread_mutex_lock(game->mutex);
		while (game->isPlay == 0)
		{
			pthread_cond_wait(game->cond_players, game->mutex);
		}
		pthread_mutex_unlock(game->mutex);

		// Lấy câu hỏi từ danh sách câu hỏi
		Question *quest = game->questions[num_question];

		// Nếu hết câu hỏi , sẽ tiền hành xử lý kết quả và trả về client
		if (quest == NULL)
		{
			printRoom(room);
			bzero(message, MAX_BUFFER);
			bzero(lose_result, 256);
			bzero(win_result, 256);
			bzero(win_message, MAX_BUFFER);
			bzero(lose_message, MAX_BUFFER);

			// Hàm xử lý kết quả
			final_result(room->players[0], room->players[1], win_result, lose_result);

			// Gửi thông tin về client
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

			// Reset lại thông tin trờ chơi
			pthread_mutex_lock(game->mutex);
			game->room->flag = 0;
			game->isPlay = 0;
			num_question = 0;
			for (int i = 0; i < room->no_player; i++)
			{
				room->players[i]->point = 0;
			}
			pthread_cond_signal(game->cond_players);
			pthread_mutex_unlock(game->mutex);
		}

		/*
			Nếu chưa hết câu hỏi
		*/
		else
		{
			pthread_mutex_unlock(game->mutex);
			fprintf(stderr, "Gửi câu hỏi: %s\n", quest->quest);
			// modify_message(QUEST, quest->quest, message);

			for (int i = 0; i < server->numClients; i++)
			{
				int socket = clientSockets[i];
				sendQuestion(quest,socket);
			}

			// Tạm block luồng nếu câu hỏi chưa được trả lời
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

/*
	Luồng xử lý yêu cầu từ client
	Tham số data là dữ liệu chứa thông tin server và socket của client
*/
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
		if (read(clientSockfd, message, MESS_BUFFER - 1) == 0)
		{
			removeClient(server, clientSockfd);
		}
		else
		{
			// read(clientSockfd, message, MESS_BUFFER - 1);
			/*
			0|10|quan-123
			*/
			printf("%s\n", message);
			Message *mess = split_message(message);

			switch (mess->cmdType)
			{

			/*

			Xử  lý yêu cầu đăng nhập
			Kiếm tra thông tin xem tài khoàn đã được đăng nhập chưa
			Nếu chưa sẽ tiến hành xử lý đăng nhập
			*/
			case LOGIN:
				user = searchUserByUsername(users, mess->body);
				if (user != NULL && isLogined(user) == 1)
				{
					bzero(message, MESS_BUFFER);
					modify_message(LOGIN_FAIL, "Tài khoản đã được sử dụng!", message);
					write(clientSockfd, message, strlen(message));
				}
				else
				{
					bzero(result, 256);
					user = handleLogin(users, mess->body, clientSockfd, result);
					modify_message(LOGIN, result, message);
					write(clientSockfd, message, strlen(message));
				}
				break;

			/*

			Xử  lý yêu cầu đăng xuất
			Kiếm tra thông tin xem tài khoàn đã được đăng nhập chưa
			Nếu chưa sẽ yêu cầu đăng nhập
			Nếu rồi tiến hành xử lý yêu cầu đăng xuất
			*/
			case LOGOT:
				user = searchUserBySocket(users, clientSockfd);
				if (user == NULL || isLogined(user) == 0)
				{
					sendMessageRequireLogin(clientSockfd);
				}
				else
				{
					fprintf(stderr, "Client on socket %d has disconnected.\n", clientSockfd);
					bzero(message, MESS_BUFFER);
					modify_message(LOGOT_SUCC, "Đăng xuất thành công!", message);
					write(clientSockfd, message, strlen(message));
					user->status = UN_LOGIN;
				}
				break;

			/*

			Xử  lý yêu cầu đăng ký
			Kiếm tra thông tin xem tài khoàn đã được đăng ký chưa
			Nếu chưa sẽ tiến hành xử lý đăng ký
			5|10|quan 123
			*/
			case SIGNU:
				if (validateUniqueUsername(users, mess->body))
				{
					bzero(message, MESS_BUFFER);
					modify_message(ACCOUNT_EXIST, "Tài khoản đã tồn tại! Thử tên khác!", message);
					write(clientSockfd, message, strlen(message));
				}
				else
				{
					handleSignUp(users, mess->body);
					bzero(message, MESS_BUFFER);
					modify_message(ACCOUNT_SUCCESS, "Đăng ký thành công! Dùng 'login' để đăng nhập!", message);
					write(clientSockfd, message, strlen(message));
				}

				break;

			/*

			Xử  lý yêu cầu tạo phòng
			Kiếm tra thông tin xem tài khoàn đã được đăng nhập chưa
			Nếu chưa sẽ yêu cầu đăng nhập
			Nếu rồi sẽ tiền hành xử lý tạo phòng

			*/
			case CRTRM:
				user = searchUserBySocket(users, clientSockfd);
				if (user == NULL || isLogined(user) == 0)
				{
					sendMessageRequireLogin(clientSockfd);
				}
				else
				{
					// Tạo một phòng
					room = createRoom();

					pthread_mutex_lock(game->mutex);
					game->room = room;
					pthread_mutex_unlock(game->mutex);

					// Thêm thông tin phòng vào danh sách phòng hiện tại
					addRoom(rooms, room);

					/* Mỗi khi người người dùng tham gia phòng
					sẽ trở thành một người chơi (player)
					sau đó thêm người chơi đó vào phòng
					*/
					user->status = IN_ROOM;

					player = initPlayer(user->username, user->socket);
					addPlayerToRoom(room, player);

					bzero(message, MESS_BUFFER);
					convert_number_to_string(room->roomid, num_in_string);

					modify_message(CRTRM, num_in_string, message);
					write(clientSockfd, message, strlen(message));

					fprintf(stdout, "Client %d request create a room.\n", clientSockfd);
				}
				break;

				/*

				Xử  lý yêu cầu lấy danh sách phòng
				Hàm room_to_string xử chuyển thông tin của danh sách phòng
				rooms thành thông tin dạng chuỗi
				*/

			case GETRM:
				bzero(message, MESS_BUFFER);
				bzero(rooms_info, MESS_BUFFER);
				rooms_to_string(rooms, rooms_info);
				modify_message(GETRM, rooms_info, message);
				write(clientSockfd, message, strlen(message));
				break;

			/*
			Kiếm tra thông tin xem tài khoàn đã được đăng nhập chưa
			Nếu chưa sẽ yêu cầu đăng nhập
			Nếu rồi sẽ tiền hành xử lý yêu cầu tham gia phòng
			*/
			case JOINR:
				user = searchUserBySocket(users, clientSockfd);
				if (user == NULL || isLogined(user) == 0)
				{
					sendMessageRequireLogin(clientSockfd);
				}
				else
				{
					if (user->status == IN_ROOM)
					{
						bzero(message, MESS_BUFFER);
						modify_message(MESSG, "Bạn không thể tham gia phòng khác\nKhi đang ở trong phòng!", message);
						write(clientSockfd, message, strlen(message));
					}
					else
					{

						// Tìm phòng theo id đã gửi lên
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
						
							if (room->no_player == 2)
							{
								bzero(message, MESS_BUFFER);
								modify_message(ROOM_FULL, "Phòng đã đầy!", message);
								//30|12|Phòng đã đầy!
								write(clientSockfd, message, strlen(message));
							}
							else
							{

								// Mỗi game sẽ thuộc một phòng
								pthread_mutex_lock(game->mutex);
								game->room = room;
								pthread_mutex_unlock(game->mutex);

								// Thêm thông tin người chơi vào phòng

								user->status = IN_ROOM;
								player = initPlayer(user->username, user->socket);
								// pthread_mutex_lock(game->mutex);
								addPlayerToRoom(room, player);
								// pthread_mutex_unlock(game->mutex);
								printRoom(room);

								// Gửi thông tin về client
								bzero(message, MESS_BUFFER);
								modify_message(JOINR, mess->body, message);
								write(clientSockfd, message, strlen(message));

								// Gửi thông tin về người chơi cùng phòng
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
						}
					}
				}
				break;

			/*

			Xử  lý yêu cầu bắt đầu trò chơi

			*/
			case STATG:

				// Tìm phòng theo id đã gửi lên
				room_in_digit = atoi(mess->body);
				room = searchRoom(rooms, room_in_digit);

				// Nếu số người chơi chưa đủ sẽ yêu cầu đợi
				if (room->no_player < 2)
				{
					bzero(message, MESS_BUFFER);
					modify_message(WAIT, "Chưa đủ người để bắt đầu !", message);
					write(clientSockfd, message, strlen(message));
				}
				else
				{
					/*
					Nếu đối thủ chưa sẵn sàng , yêu cầu đợi
					 và thông báo lại cho người chơi còn lại
					*/
					if (room->flag < 1)
					{
						/*
						flag (Biến đễ xác định đủ số lượng đã sẵn sàng)
						Mỗi khi có 1 yêu cầu start game thì flag tăng thêm 1
						*/
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

					/*
					Nếu cả 2 sẵn sàng
					Tiến hành bắt đầu game, chuyển sang thread gameHandler
					Tạm block thread này
					*/

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

						// block thread client handler
						pthread_mutex_lock(game->mutex);
						while (game->isPlay == 1)
						{
							pthread_cond_wait(game->cond_players, game->mutex);
						}

						// Chuyển sang trạng thái đang chơi
						game->isPlay = 1;
						
						pthread_cond_signal(game->cond_players);
						pthread_mutex_unlock(game->mutex);

						
					}
				}

				break;

			/*

			Xử  lý yêu cầu trả lời câu hỏi
			Biến num_question là chỉ số để lấy câu hỏi tử danh sách câu hỏi
			So sánh đáp án từ người chơi, và đáp án trong hệ thống
			Nếu đúng gửi thông báo về người chơi, chuyển sang câu hỏi khác
			nếu sai gửi thông báo về người trả lời, tiếp tục ở câu hỏi cũ

			*/
			case ANSWR:

				// Lấy câu hỏi
				pthread_mutex_lock(game->mutex);
				question = game->questions[num_question];
				pthread_mutex_unlock(game->mutex);

				// Lấy thông tin người chơi trả lời cầu hỏi
				room = game->room;
				player = getPlayerBySocket(room->players, clientSockfd);

				// So sánh đáp án , gửi thông báo về người chơi
				if (strcmp(question->answer, mess->body) == 0)
				{
					bzero(result, MESS_BUFFER);
					bzero(socketStr, 2);
					sprintf(socketStr, "%d", clientSockfd);
					strcat(result, "Người chơi ");
					strcat(result, player->username);
					strcat(result, " trả lời đúng!");
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
					modify_message(CORRECT, "Chính xác!", message);

					pthread_mutex_lock(game->mutex);
					updatePoint(player,question->point);
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
					modify_message(INCORRECT, "Không chính xác!", message);
					write(clientSockfd, message, strlen(message));
				}
				break;

			/*

			Xử  lý yêu cầu chat
			Kiếm tra thông tin xem tài khoàn đã được đăng nhập chưa
			Nếu chưa sẽ yêu cầu đăng nhập
			Nếu rồi, sẽ lấy thông tin chat, gửi lại cho người chơi
			còn lại
			*/
			case CHAT:
				user = searchUserBySocket(users, clientSockfd);
				if (user == NULL || isLogined(user) == 0)
				{
					sendMessageRequireLogin(clientSockfd);
				}
				else
				{
					if (user->status == IN_ROOM)
					{

						room = game->room;
						if (room != NULL)
						{
							if (room->no_player < 2)
							{
								modify_message(WAIT, "Đợi người chơi tham gia phòng!", message);
								write(clientSockfd, message, strlen(message));
							}
							else
							{
								player = getPlayerBySocket(room->players, clientSockfd);
								if (player != NULL)
								{
									bzero(message, MESS_BUFFER);
									bzero(result, MESS_BUFFER);

									strcat(result, player->username);
									strcat(result, ": ");
									strcat(result, mess->body);
									modify_message(READY, result, message);
									for (int i = 0; i < room->no_player; i++)
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
						}
					}
					else if (user->status == OUT_ROOM)
					{
						bzero(message, MESS_BUFFER);
						modify_message(NOT_IN_ROOM, "Bạn không ở trong phòng!", message);
						write(clientSockfd, message, strlen(message));
					}
				}
				break;

			/*

			Xử  lý yêu cầu quay trở lại
			Nếu người chơi ở ngoài phòng sẽ trở về trang chủ
			Nếu người chơi đã tham gia phòng
				Quay về trang chủ, gửi thông báo cho người cùng phòng biết là
				bạn đã thoát
			*/
			case BACK:
				user = searchUserBySocket(users, clientSockfd);

				room = game->room;
				if (room == NULL)
				{
					bzero(message, MESS_BUFFER);
					modify_message(BACK_TO_HOME, "BACK TO HOME", message);
					write(clientSockfd, message, MESS_BUFFER - 1);
				}
				else
				{
					if (user->status == IN_ROOM)
					{

						user->status = OUT_ROOM;
						bzero(message, MESS_BUFFER);
						modify_message(BACK_OK, "BACK_OK", message);
						write(clientSockfd, message, MESS_BUFFER - 1);
						for (int i = 0; i < room->no_player; i++)
						{
							if (room->players[i] != NULL)
							{
								if (room->players[i]->socket == clientSockfd)
								{
									//pthread_mutex_lock(game->mutex);
									room->players[i] = NULL;
									room->no_player--;
									room->flag = 0;
									//pthread_mutex_unlock(game->mutex);
									break;
								}
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
									break;
								}
							}
						}
						printRoom(room);
					}
					else
					{
						bzero(message, MESS_BUFFER);
						modify_message(BACK_TO_HOME, "BACK TO HOME", message);
						write(clientSockfd, message, MESS_BUFFER - 1);
					}
				}
				break;

			/*

			Xử  lý yêu cầu exit
			Kiếm tra thông tin xem tài khoàn đã được đăng nhập chưa
			Nếu chưa gửi thông báo thoát thành công mà không có thông tin người dùng
			Nếu rồi thì cập nhập lại trạng thái của user đã thoát
			rồi gửi thông báo lại cho client
			*/
			case EXIT:
				user = searchUserBySocket(users, clientSockfd);
				if (user == NULL || isLogined(user) == 0)
				{
					bzero(message, MESS_BUFFER);
					modify_message(EXIT_NOT_LOGIN, "Thoát thành công!", message);
					write(clientSockfd, message, strlen(message));
					printf("Người chơi số %d đã thoát!\n", clientSockfd);
				}
				else
				{
					user->status = UN_LOGIN;
					printf("%s đã thoát!\n", user->username);
					bzero(message, MESS_BUFFER);
					modify_message(EXIT_SUCC, "Thoát thành công!", message);
					write(clientSockfd, message, strlen(message));
				}
				break;

			default:
				break;
			}
			bzero(message, MESS_BUFFER);
		}
	}
}

/*
	Hàm xử lý kết quả sau khi trờ chơi kết thúc,
	hàm trả về 2 giá trị
		win_result : thông tin người thăng
		lose_result: thông tin người thua
*/
void final_result(Player *player1, Player *player2, char *win_result, char *lose_result)
{
	printPlayer(player1);
	printPlayer(player2);
	char point[2];
	if (player1->point > player2->point)
	{

		// win
		bzero(point, 2);
		strcat(win_result, "Bạn thắng với ");
		sprintf(point, "%d", player1->point);
		strcat(win_result, point);
		strcat(win_result, " điểm");

		// lose
		bzero(point, 2);
		strcat(lose_result, "Bạn thua với ");
		sprintf(point, "%d", player2->point);
		strcat(lose_result, point);
		strcat(lose_result, " điểm");
		printf("Player %s won! With %d points \n", player1->username, player1->point);
	}
	else if (player1->point < player2->point)
	{
		// win
		bzero(point, 2);
		strcat(win_result, "Bạn thắng với ");
		sprintf(point, "%d", player2->point);
		strcat(win_result, point);
		strcat(win_result, " điểm");

		// lose
		bzero(point, 2);
		strcat(lose_result, "Bạn thua với ");
		sprintf(point, "%d", player1->point);
		strcat(lose_result, point);
		strcat(lose_result, " điểm");
		printf("Player %s won! With %d points\n", player2->username, player2->point);
	}
	else
	{
		strcpy(lose_result, "Hòa!");
		strcpy(win_result, "Hòa!");
		printf("Draw with %d - %d\n", player1->point, player2->point);
	}
}

/*
	Hàm yêu cầu đăng nhập
*/
void sendMessageRequireLogin(int socketfd)
{
	char message[MESS_BUFFER];
	bzero(message, MESS_BUFFER);
	modify_message(NOT_LOGIN, "Bạn cần đăng nhập trước!", message);
	write(socketfd, message, strlen(message));
}

void removeClient(Server *server, int clientSocketFd)
{

	for (int i = 0; i < MAX_BUFFER; i++)
	{

		if (server->clientSockets[i] >= 0 && server->clientSockets[i] == clientSocketFd)
		{
			pthread_mutex_lock(server->clientMutex);
			server->clientSockets[i] = -1;
			server->numClients--;
			pthread_mutex_unlock(server->clientMutex);
			break;
		}
	}
}
