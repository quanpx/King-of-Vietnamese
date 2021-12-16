#include "question.h"
Question *initQuest(char *quest, char *answer, int point)
{
	Question *newQuest = (Question *)malloc(sizeof(Question));
	if (newQuest == NULL)
	{
		printf("Allocated error!\n");
		return NULL;
	}
	strcpy(newQuest->quest, quest);
	strcpy(newQuest->answer, answer);
	newQuest->point = point;
	newQuest->next = NULL;
	return newQuest;
}
void addQuest(Question **quests, Question *quest)
{
	Question *cur = *quests;
	if (*quests == NULL)
	{
		*quests = quest;
		return;
	}
	while (cur->next != NULL)
	{
		cur = cur->next;
	}
	cur->next = quest;
	return;
}
Question *searchQuest(Question *quests, char *quest)
{
	Question *cur = quests;
	while (cur != NULL)
	{
		if (strcmp(cur->quest, quest) == 0)
		{
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}

// process read file
void readQuestsFromFile(Question **quests, char *filename)
{
	char quest[20];
	char answer[20];
	int point;
	char line[50];
	FILE *fo = fopen(filename, "r");
	if (fo == NULL)
	{
		printf("Open file error\n");
		return;
	}
	while (fgets(line, 50, fo) != NULL)
	{
		char *token = strtok(line, "|");
		strcpy(quest, token);
		token = strtok(NULL, "|");
		strcpy(answer, token);
		token = strtok(NULL, "|");
		point = atoi(token);
		Question *question = initQuest(quest, answer, point);
		bzero(quest, 20);
		bzero(answer, 20);
		addQuest(quests, question);
	};
	fclose(fo);
	return;
}
void writeQuestsToFile(Question *quests, char *filename)
{
	FILE *fo = fopen(filename, "w");
	if (fo == NULL)
	{
		printf("File open failed\n");
		return;
	}
	Question *cur = quests;
	while (cur != NULL)
	{
		printf("%s %s %d\n", cur->quest, cur->answer, cur->point);
		fprintf(fo, "%s|%s|%d\n", cur->quest, cur->answer, cur->point);
		cur = cur->next;
	}
	fclose(fo);
	return;
}
void printQuests(Question *quests)
{
	Question *cur = quests;
	while (cur != NULL)
	{
		printf("%s %s %d\n", cur->quest, cur->answer, cur->point);
		cur = cur->next;
	}
}