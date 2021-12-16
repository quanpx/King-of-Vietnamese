#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct user
{
	char username[10];
	char password[10];
	int status;
	int socket;
	struct user *next;
} User;


User *initUser(char *username, char *password, int status);
void addUser(User **users,User *user);
User* searchUser(User *users,char *username);
void readUsersFromFile(User **users,char *filename);
void writeUsersToFile(User *user,char *filename);
void printUser(User *user);