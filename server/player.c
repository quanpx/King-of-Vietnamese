#include "player.h"
Player *players=NULL;
Player *initPlayer(char *username,int socket)
{
    Player *player=(Player*)malloc(sizeof(Player));
    if(player==NULL)
    {
        printf("Allocate memory failed\n");
        return NULL;
    }
    strcpy(player->username,username);
	for(int i=0;i<14;i++)
	{
		player->point[i]=0;
	}
	player->socket=socket;
    return player;
}
int getTotalPoint(Player *player)
{
    int point=0;
    for(int i=0;i<14;i++)
    {
        point += player->point[i];
    }
    return point;
}
void addPlayer(Player **players, Player *player)
{
	Player *cur = *players;
	if (*players == NULL)
	{
		*players= player;
		return;
	}
	while (cur->next != NULL)
	{
		cur = cur->next;
	}
	cur->next =player;
	return;
}
Player *searchPlayer(Player *players, char *username)
{
	Player *cur = players;
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

void printPlayers(Player *players)
{
	Player *cur = players;
	while (cur != NULL)
	{
		printf("%s has %d points\n", cur->username, getTotalPoint(cur));
		cur = cur->next;
	}
}
void updatePoint(Player *player,int num_question,int point)
{
    player->point[num_question]=point;
}