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
	for (int i = 0; i < 14; i++)
	{
		player->point[i] = 0;
	}
	player->socket = socket;
	return player;
}
int getTotalPoint(Player *player)
{
	int point = 0;
	for (int i = 0; i < 14; i++)
	{
		point += player->point[i];
	}
	return point;
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
void updatePoint(Player *player, int num_question, int point)
{
	player->point[num_question] = point;
}