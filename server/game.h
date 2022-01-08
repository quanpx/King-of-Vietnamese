#include <pthread.h>
#include "../utils/utils.h"
#include "question.h"
#include "room.h"
#ifndef STRUCT_GAME
#define STRUCT_GAME
typedef struct game
{
	Question *questions[MAX_QUESTION];
	int isPlay;
	int isAnswered;
	pthread_cond_t *cond_players;
	pthread_cond_t *cond_answered;
	pthread_mutex_t *mutex;
	Room *room;
} Game;
#endif // STRUCT GAME
void playGame();
Game *initGame();
