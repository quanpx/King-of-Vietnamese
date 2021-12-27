#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef STRUCT_PLAYER
#define STRUCT_PLAYER
typedef struct player
{
    char username[20];
    int point[14];
    int socket;
    struct player *next;
}Player;

#endif /* STRUCT_PLAYER */

Player *initPlayer(char* username,int socket);
int getTotalPoint(Player *player);
void printPlayers(Player *player);
Player *searchPlayer(Player *players,char *username);
void addPlayer(Player **players,Player *player);
void updatePoint(Player *player,int num_question,int point);
