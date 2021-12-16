#include "user.h"
#include "question.h"
#include "room.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

User *users = NULL;
User *activeUsers = NULL;
Question *questions = NULL;
Player *players = NULL;
Room *room=NULL;
int no_question = 0;
int no_players = 0;
int authenticate(User *users, char *username, char *password)
{
	User *user = searchUser(users, username);
	if (user == NULL)
	{
		printf("User not found\n");
		return 0;
	}
	if (strcmp(user->password, password) == 0)
	{
		return 1;
	}
	return 0;
}
User *handleLogin(char *client_message, int socket)
{
	char username[20];
	char password[20];
	bzero(username, 20);
	bzero(password, 20);
	char *token = strtok(client_message, "|");
	strcpy(username, token);
	// Tìm kiếm user
	User *user = searchUser(users, username);
	if (user == NULL)
	{
		// noex: no exist user : người dùng không tồn tại
		strcpy(client_message, "noex");
	}
	else

	{
		if (user->status == 0)
		{
			// Tài khoản đang tạm khóa
			strcpy(client_message, "lckd");
		}
		else
		{
			// Kiểm tra nếu nhập quá số lần sẽ bị khóa tài khỏa
			token = strtok(NULL, "|");
			strcpy(password, token);
			if ((token = strtok(NULL, "|")) != NULL)
			{

				user->status = 0;
				strcpy(client_message, "lock");
				writeUsersToFile(users, "../file/test.txt");
			}
			// Tiến hành kiểm tra thông tin tài khoản mật khẩu
			else
			{

				// xác thực thông tin user có hợp lệ
				int check = authenticate(users, username, password);
				if (check)
				{

					strcpy(client_message, "succ");
					user->socket = socket;
					// Nếu thông tin hợp lệ, tạo ra 1 player với tên giống user ;
					Player *player = initPlayer(username,socket);
					addPlayer(&(room->player), player);
					printRoom(room);
					// Tăng số người chơi lên 1
					no_players++;
				}
				// còn không thông báo đăng nhập thất bại
				else
				{
					user=NULL;
					strcpy(client_message, "fail");
				}
			}
		}
	}
	return user;
}
int checkAnswer(Question *quest, char *answer)
{
	return strcmp(quest->answer, answer) == 0 ? 1 : 0;
}

/* Gửi câu hỏi đến client nhân thâm số
là user va câu hỏi
*/
void sendQuestion(Question *quest)
{
	Player *player = room->player;
	while (player != NULL)
	{
		send(player->socket, quest->quest, strlen(quest->quest),0);
		player = player->next;
	}
}

/*
Nhận câu trả lời từ client và kiểm tra kết quả
*/
void receiveAnswer(User *user, Question *quest)
{
	char answer[50];
	bzero(answer, 50);
	if (recv(user->socket, answer, sizeof(answer), 0) > 0)
	{
		/*
		Nếu checkAnswer nếu đúng trả về 1 thì cập nhật số điểm của players
		*/
		int check = checkAnswer(quest, answer);
		if (check)
		{
			Player *player = searchPlayer(room->player, user->username);
			updatePoint(player, no_question, quest->point);
			printPlayers(players);
			printf("True\n");
		}
		else
		{
			printf("False\n");
		}
	}
}
// Xử lý chơi game
void playGame(User *user)
{
	// Dọc câu hỏi từ file
	readQuestsFromFile(&questions, "../file/question.txt");
	Question *quest = questions;
	int no = 1;
	while (no <= 3)
	{
		// gửi câu hỏi tới client
		sendQuestion(quest);
		// Nhận câu hỏi từ client
		receiveAnswer(user, quest);
		quest = quest->next;
		no++;
		no_question++;
	}
	send(user->socket, "done", strlen("done"), 0);
	return;
}
void clientJoined(User *user)
{
	char request[256];
	char response[256];
	bzero(response, 256);
	bzero(request, 256);
	while (recv(user->socket, request, sizeof(request), 0) > 0)
	{
		// Khi nhận được yêu câu băt đầu từ client , bắt đấù chơi game
		if (strcmp(request, "start") == 0)
		{

			// play game
			playGame(user);
		}
		else
		{
			close(user->socket);
			break;
		}
	}
}
void *connection_handler(void *client_socket)
{
	int socket = *(int *)client_socket;
	char client_message[100];
	while (recv(socket, client_message, 100, 0) > 0)
	{
		// Xử lý thông tin đăng nhập, sau khi đăng nhập thành công trả về thông tin của user
		User *user = handleLogin(client_message, socket);
		if(user!=NULL)
		{
			printf("%s at socket %d joined! \n", user->username, user->socket);
		}
		
		send(socket, client_message, strlen(client_message), 0);
		if (strcmp(client_message, "succ") == 0)
		{
			// Sau khi đăng nhập thành công , client tham gia vào phòng
			clientJoined(user);
			
		}
		bzero(client_message, 100);
	}

	return 0;
}

// Remember to use -pthread when compiling this server
int main(int argc, char **argv)
{
	//Đọc user từ file
	room=initRoom();
	readUsersFromFile(&users, "../file/user.txt");
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket == -1)
	{
		perror("Socket initialisation failed");
		exit(EXIT_FAILURE);
	}
	else
		printf("Server socket created successfully\n");

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = 5000;
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to the specified IP addr and port
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
	{
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	if (listen(server_socket, 3) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	int no = 0;
	pthread_t threads[2];
	printf("Listening...\n");
	while (no < 3)
	{
		int client_socket = accept(server_socket, NULL, NULL);
		printf("Connection accepted\n");
		if (pthread_create(&threads[no], NULL, &connection_handler, &client_socket) < 0)
		{
			perror("Could not create thread");
			return 1;
		}
		if (client_socket < 0)
		{
			printf("server acccept failed...\n");
			exit(0);
		}
		else
			printf("Server acccept the client...\n");
		puts("Handler assigned");
	}
	for (int i = 0; i < 3; i++)
	{
		if (pthread_join(threads[i], NULL) != 0)
		{
			printf("Join thread error!\n");
		}
	}

	// int send_status;
	// send_status=send(client_socket, server_message, sizeof(server_message), 0);
	close(server_socket);

	return 0;
}
