#include "authentication.h"
#include <pthread.h>
#include "../utils/utils.h"
int authenticate(User *users[MAX_USER],char *username, char *password)
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
User* handleLogin(User *users[MAX_USER],char *body, int socket,char *result)
{
	char username[20];
	char password[20];
	bzero(result,256);
	bzero(username, 20);
	bzero(password, 20);
	char *token = strtok(body, "-");
	strcpy(username, token);
	
	// Tìm kiếm user
	User *user = searchUser(users, username);
	if (user == NULL)
	{
		// noex: no exist user : người dùng không tồn tại
		strcpy(result, "noex");
	}
	else

	{
		// Tiến hành kiểm tra thông tin tài khoản mật khẩu
		// Lấy mật khẩu
		char *token=strtok(NULL,"-");
		strcpy(password,token);
		// xác thực thông tin user có hợp lệ
		int check = authenticate(users,username, password);
		if (check)
		{

			strcpy(result, "succ");
			user->socket = socket;
			user->status=1;
			printf("%s at socket %d joined! \n", user->username, user->socket);
		}
		// còn không thông báo đăng nhập thất bại
		else
		{
			user = NULL;
			strcpy(result, "fail");
		}
	}
	return user;
}
int isLogined(User *user)
{
	return user->status==1?1:0;
}

