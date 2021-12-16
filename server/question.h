#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct question
{
    char quest[50];
    char answer[50];
    int point;
    struct question *next;
}Question;

Question *initQuestion(char *quest, char *answer, int point);
void addQuestname(Question **quests,Question *quest);
Question* searchQuest(Question *quests,char *quest);
void readQuestsFromFile(Question **quests,char *filename);
void writeQuestsToFile(Question *quests,char *filename);
void printQuests(Question *quests);