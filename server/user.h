#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "../utils/utils.h"
#define MAX_USER 10

#ifndef ENUM_POSITION
#define ENUM_POSITION

typedef enum Status
{
	UN_LOGIN,
	LOGINED,
	IN_ROOM,
	OUT_ROOM,
	IN_GAME
} Status;
#endif /*ENUM_POSITION */
#ifndef STRUCT_USER
#define STRUCT_USER

typedef struct user
{
	char username[10];
	char password[10];
	int socket;
	Status status;
} User;
#endif /*STRUCT_USER */

User *initUser(char *username, char *password, int status);
void addUser(User *users[MAX_USER], User *user);
User *searchUser(User *users[MAX_USER], char *username);
void readUsersFromFile(User *users[MAX_USER], char *filename);
void writeUsersToFile(User *user[MAX_USER]);
void printUser(User *user);
void makeUsersNull(User *users[MAX_USER]);
void printAllUsers(User *users[MAX_USER]);
User *searchUserBySocket(User *users[MAX_USER], int socket);
User *searchUserByUsername(User *users[MAX_USER], char *info);
void handleSignUp(User *users[MAX_USER], char *info);
int validateUniqueUsername(User *user[MAX_USER], char *info);