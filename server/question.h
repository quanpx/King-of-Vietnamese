#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "player.h"
#include "room.h"
#include "user.h"
#define LIST_QUESTION 100
#define MAX_QUESTION 15
#ifndef STRUCT_QUESTION
#define STRUCT_QUESTION
typedef struct question
{
    char quest[50];
    char answer[50];
    int point;
}Question;
#endif /*STRUCT_QUESTION*/


Question *initQuestion(char *quest, char *answer, int point);
void addQuest(Question *quests[MAX_QUESTION],Question *quest);
Question* searchQuest(Question *quests[MAX_QUESTION],char *quest);
void readQuestsFromFile(Question *quests[MAX_QUESTION],char *filename);
void writeQuestsToFile(Question *quests[MAX_QUESTION],char *filename);
void printQuests(Question *quests[MAX_QUESTION]);
int checkAnswer(Question *quest, char *answer);
void sendQuestion(Question *quest);
void receiveAnswer(Player *player, Question *quest);
void getQuestions(Question *questions[MAX_QUESTION],char *file);
void printQuestion(Question *question);