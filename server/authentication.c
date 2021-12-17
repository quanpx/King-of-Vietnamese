#include "authentication.h"
extern User *users;
extern Room *room;
extern Player *players;
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
			//lckd: locked Tài khoản đang tạm khóa 
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
					addPlayer(&players,player);
					addPlayerToRoom(room, player);
					printRoom(room);
					// Tăng số người chơi lên 1
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