#include "user.h"

User *users;
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
	newUser->next = NULL;
	return newUser;
}
void addUser(User **users, User *user)
{
	User *cur = *users;
	if (*users == NULL)
	{
		*users = user;
		return;
	}
	while (cur->next != NULL)
	{
		cur = cur->next;
	}
	cur->next = user;
	return;
}
User *searchUser(User *users, char *username)
{
	User *cur = users;
	while (cur != NULL)
	{
		if (strcmp(cur->username, username) == 0)
		{
			return cur;
		}
		cur = cur->next;
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
void writeUsersToFile(User *users, char *filename)
{
	FILE *fo = fopen(filename, "w");
	if (fo == NULL)
	{
		printf("File open failed\n");
		return;
	}
	User *cur = users;
	while (cur != NULL)
	{
		printf("%s %s %d\n", cur->username, cur->password, cur->status);
		fprintf(fo, "%s|%s|%d\n", cur->username, cur->password, cur->status);
		cur = cur->next;
	}
	fclose(fo);
	return;
}
void printUser(User *users)
{
	User *cur = users;
	while (cur != NULL)
	{
		printf("%s %s %d\n", cur->username, cur->password, cur->status);
		cur = cur->next;
	}
}