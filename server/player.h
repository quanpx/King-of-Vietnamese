#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_PLAYER 2
#ifndef ENUM_ROLE
#define ENUM_ROLE
typedef enum role
{
  GUESS,
  OWNER   
}Role;

#endif /* ENUM_PLAYER */
#ifndef STRUCT_PLAYER
#define STRUCT_PLAYER
typedef struct player
{
    char username[20];
    int point;
    int socket;
    Role role;
}Player;

#endif /* STRUCT_PLAYER */

Player *initPlayer(char* username,int socket);
int getTotalPoint(Player *player);
void printPlayer(Player *player);
Player *searchPlayer(Player *players[MAX_PLAYER],char *username);
void addPlayer(Player *players[MAX_PLAYER],Player *player);
void updatePoint(Player *player,int point);
Player *getPlayerBySocket(Player *player[MAX_PLAYER],int socket);
