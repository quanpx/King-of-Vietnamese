#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_USER 10
#ifndef STRUCT_USER
#define STRUCT_USER

typedef struct user
{
	char username[10];
	char password[10];
	int status;
	int socket;
	struct user *next;
} User;
#endif /*STRUCT_USER */

User *initUser(char *username, char *password, int status);
void addUser(User *users[MAX_USER],User *user);
User* searchUser(User *users[MAX_USER],char *username);
void readUsersFromFile(User *users[MAX_USER],char *filename);
void writeUsersToFile(User *user[MAX_USER],char *filename);
void printUser(User *user);
void makeUsersNull(User *users[MAX_USER]);
void printAllUsers(User *users[MAX_USER]);
