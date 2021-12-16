#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct player
{
    char username[20];
    int point[14];
    int socket;
    struct player *next;
}Player;

Player *initPlayer(char* username,int socket);
int getTotalPoint(Player *player);
void printPlayers(Player *player);
Player *searchPlayer(Player *player,char *username);
void addPlayer(Player **players,Player *player);
void updatePoint(Player *player,int num_question,int point);
