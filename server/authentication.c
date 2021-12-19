#include "authentication.h"
#include <pthread.h>
#include "../utils/utils.h"
extern User *users;
extern Room *room;
extern Player *players;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
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
	Message *mess = split_message(client_message);
	char username[20];
	char password[20];
	bzero(username, 20);
	bzero(password, 20);
	char *token = strtok(mess->body, "-");
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
			// Kiểm tra nếu nhập quá số lần sẽ bị khóa tài khỏa
			token = strtok(NULL, "|");
			strcpy(password, token);
			// Tiến hành kiểm tra thông tin tài khoản mật khẩu

			// xác thực thông tin user có hợp lệ
			int check = authenticate(users, username, password);
			if (check)
			{

				strcpy(client_message, "succ");
				user->socket = socket;
				printf("%s at socket %d joined! \n", user->username, user->socket);
				// Nếu thông tin hợp lệ, tạo ra 1 player với tên giống user ;
				Player *player = initPlayer(username, socket);
				addPlayer(&players, player);
				pthread_mutex_lock(&mutex);
				addPlayerToRoom(room, player);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex);
				// Tăng số người chơi lên 1
			}
			// còn không thông báo đăng nhập thất bại
			else
			{
				user = NULL;
				strcpy(client_message, "fail");
			}
		
	}
	return user;
}