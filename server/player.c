#include "player.h"
Player *players = NULL;
Player *initPlayer(char *username, int socket)
{
	Player *player = (Player *)malloc(sizeof(Player));
	if (player == NULL)
	{
		printf("Allocate memory failed\n");
		return NULL;
	}
	strcpy(player->username, username);
	player->point=0;
	player->socket = socket;
	return player;
}
int getTotalPoint(Player *player)
{
	
	return player->point;
}
void addPlayer(Player *players[MAX_PLAYER], Player *player)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (players[i] == NULL)
		{
			players[i] = player;
		}
	}
}
Player *searchPlayer(Player *players[MAX_PLAYER], char *username)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (players[i] != NULL)
		{
			if (strcmp(players[i]->username, username) == 0)
			{
				return players[i];
			}
		}
	}
}

void printPlayer(Player *player)
{

	printf("%s has %d points\n", player->username, getTotalPoint(player));
}
void updatePoint(Player *player)
{
	player->point+=1;
}

Player *getPlayerBySocket(Player *players[MAX_PLAYER],int socket)
{
	for(int i=0;i<MAX_PLAYER;i++)
	{
		if(players[i]!=NULL)
		{
			if(players[i]->socket==socket)
			{
				return players[i];
			}
		}
	}
}