#include "user.h"

User *initUser(char *username, char *password, int status)
{
	User *newUser = (User *)malloc(sizeof(User));
	if (newUser == NULL)
	{
		printf("Allocated error!\n");
		return NULL;
	}
	strcpy(newUser->username, username);
	strcpy(newUser->password, password);
	newUser->status = status;
	newUser->socket = -1;
	return newUser;
}
void addUser(User *users[MAX_USER], User *user)
{
	for (int i = 0; i < MAX_USER; i++)
	{
		if (users[i] == NULL)
		{
			users[i] = user;
			return;
		}
	}
	return;
}
User *searchUser(User *users[MAX_USER], char *username)
{
	for (int i = 0; i < MAX_USER; i++)
	{
		if (users[i] != NULL)
		{
			if (strcmp(users[i]->username, username) == 0)
			{
				return users[i];
			}
		}
	}

	return NULL;
}

// process read file
void readUsersFromFile(User **users, char *filename)
{
	char username[20];
	char password[20];
	int status;
	char line[50];
	FILE *fo = fopen(filename, "r");
	if (fo == NULL)
	{
		printf("Open file error\n");
		return;
	}
	while (fgets(line, 50, fo) != NULL)
	{
		char *token = strtok(line, "|");
		strcpy(username, token);
		token = strtok(NULL, "|");
		strcpy(password, token);
		token = strtok(NULL, "|");
		status = atoi(token);
		User *user = initUser(username, password, status);
		bzero(username, 20);
		bzero(password, 20);
		addUser(users, user);
	};
	fclose(fo);
	return;
}
void writeUsersToFile(User *users[MAX_USER])
{
	FILE *fo = fopen("../file/user.txt", "w");
	if (fo == NULL)
	{
		printf("File open failed\n");
		return;
	}
	int i = 0;
	while (users[i] != NULL)
	{
		fprintf(fo, "%s|%s|%d\n", users[i]->username, users[i]->password, users[i]->status);
		i++;
	}
	fclose(fo);
	return;
}
void printUser(User *user)
{

	printf("%s %s %d\n", user->username, user->password, user->status);
}
void makeUsersNull(User *users[MAX_USER])
{
	for (int i = 0; i < MAX_USER; i++)
	{
		users[i] = NULL;
	}
}

void printAllUsers(User *users[MAX_USER])
{
	for (int i = 0; i < MAX_USER; i++)
	{
		if (users[i] != NULL)
		{
			printUser(users[i]);
		}
	}
}

User *searchUserBySocket(User *users[MAX_USER], int socket)
{
	for (int i = 0; i < MAX_USER; i++)
	{
		if (users[i] != NULL && users[i]->socket > 0)
		{
			if (users[i]->socket == socket)
			{
				return users[i];
			}
		}
	}
	return NULL;
}
void handleSignUp(User *users[MAX_USER], char *info)
{
	char username[20];
	char password[20];
	bzero(username, 20);
	bzero(password, 20);

	split_command(info, username, password);
	printf("%s %s\n", username, password);
	
	User *user = initUser(username, password, 0);
	addUser(users, user);
	writeUsersToFile(users);
}
User *searchUserByUsername(User *users[MAX_USER], char *info)
{
	char username[20];
	bzero(username, 20);
	char temp_info[50];
	bzero(temp_info, 50);
	strcpy(temp_info, info);
	char *token = strtok(temp_info, "-");
	if (token != NULL)
	{
		strcpy(username, token);
	}
	for (int i = 0; i < MAX_USER; i++)
	{
		if (users[i] != NULL)
		{
			if (strcmp(users[i]->username, username) == 0)
			{
				printUser(users[i]);
				return users[i];
			}
		}
	}

	return NULL;
}
int validateUniqueUsername(User *users[MAX_USER], char *info)
{
	char username[20];
	bzero(username, 20);
	char temp_info[50];
	bzero(temp_info, 50);
	strcpy(temp_info, info);
	char *token = strtok(temp_info, " ");
	if (token != NULL)
	{
		strcpy(username, token);
	}
	for (int i = 0; i < MAX_USER; i++)
	{
		if (users[i] != NULL)
		{
			if (strcmp(users[i]->username, username) == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}
// void main()
// {
// 	User *users[MAX_USER];
// 	// makeUsersNull(users);
// 	readUsersFromFile(users, "../file/user.txt");
// 	// users[1]->socket=2;
// 	// User *user=searchUserBySocket(users,2);
// 	// printUser(user);
// 	// char info[50] = "quanphung crquan07";
// 	// User *user = handleSignUp(info);
// 	// printUser(user);
// 	// char username[20] = "quanify";
// 	// int check = validateUniqueUsername(users,username);
// 	// printf("%d\n", check);
// 	User *user = searchUserByUsername(users, "quan");
// 	if (user != NULL)
// 	{
// 		printUser(user);
// 	}
// }
