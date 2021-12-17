#include "question.h"
extern Room *room;
int no_question = 1;
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
	return newQuest;
}
void addQuest(Question *quests[MAX_QUESTION], Question *quest)
{
	
	for (int i = 0; i < MAX_QUESTION; i++)
	{
		if (quests[i] == NULL)
		{
			quests[i] = quest;
			break;
		}
	}
}
Question *searchQuest(Question *quests[MAX_QUESTION], char *quest)
{
	for (int i = 0; i < MAX_QUESTION; i++)
	{
		if (quests[i] != NULL)
		{
			if (strcmp(quests[i]->quest, quest) == 0)
			{
				return quests[i];
			}
		}
	}
	return NULL;
}

// process read file
void readQuestsFromFile(Question *quests[MAX_QUESTION], char *filename)
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
	Question *question=NULL;
	while (fgets(line, 50, fo) != NULL)
	{
		char *token = strtok(line, "|");
		strcpy(quest, token);
		token = strtok(NULL, "|");
		strcpy(answer, token);
		token = strtok(NULL, "|");
		point = atoi(token);
		question = initQuest(quest, answer, point);
		bzero(quest, 20);
		bzero(answer, 20);
		addQuest(quests, question);
	};
	fclose(fo);
	return;
}
void printQuestion(Question *question)
{
	printf("%s %s %d\n",question->quest,question->answer,question->point);
}
void writeQuestsToFile(Question *quests[MAX_QUESTION], char *filename)
{
	FILE *fo = fopen(filename, "w");
	if (fo == NULL)
	{
		printf("File open failed\n");
		return;
	}
	for (int i = 0; i < MAX_QUESTION; i++)
	{
		if (quests[i] != NULL)
		{
			printf("%s %s %d\n", quests[i]->quest, quests[i]->answer, quests[i]->point);
			fprintf(fo, "%s|%s|%d\n", quests[i]->quest, quests[i]->answer, quests[i]->point);
		}
	}
	fclose(fo);
	return;
}
void printQuests(Question *quests[MAX_QUESTION])
{
	int i=0;
	while ( i<MAX_QUESTION)
	{
		if(quests[i]!=NULL)
		{
			printf("%s %s %d\n", quests[i]->quest, quests[i]->answer, quests[i]->point);
		}
		i++;
	}
}
int checkAnswer(Question *quest, char *answer)
{
	return strcmp(quest->answer, answer) == 0 ? 1 : 0;
}
/* Gửi câu hỏi đến client nhân thâm số
là user va câu hỏi
*/
void sendQuestion(Question *quest)
{
	for (int i = 0; i < room->no_player; i++)
	{
		if (room->players[i] != NULL)
		{
			send(room->players[i]->socket, quest->quest, strlen(quest->quest), 0);
		}
	}
}
/*
Nhận câu trả lời từ client và kiểm tra kết quả
*/
void receiveAnswer(Player *player, Question *quest)
{
	char answer[50];
	bzero(answer, 50);
	if (recv(player->socket, answer, sizeof(answer), 0) > 0)
	{
		/*
		Nếu checkAnswer nếu đúng trả về 1 thì cập nhật số điểm của players
		*/
		int check = checkAnswer(quest, answer);
		if (check)
		{
			updatePoint(player, no_question, quest->point);
			printPlayers(player);
			printf("True\n");
		}
		else
		{
			printf("False\n");
		}
	}
}
void getQuestions(Question *questions[MAX_QUESTION],char *file)
{
	for(int i=0;i<MAX_QUESTION;i++)
	{
		questions[i]=NULL;
	}
	readQuestsFromFile(questions,file);
}