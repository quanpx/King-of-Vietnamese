#include "question.h"
#include<pthread.h>
#include "../utils/utils.h"

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
	newQuest->isAnswered=0;
	newQuest->mutex=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if(newQuest->mutex==NULL)
	{
		printf("Allocate failed!\n");
		exit(0);
	}
	pthread_mutex_init(newQuest->mutex,NULL);
	newQuest->notAnswer=(pthread_cond_t*)malloc(sizeof(pthread_cond_t));
	if(newQuest->notAnswer==NULL)
	{
		printf("Allocate failed!\n");
		exit(0);
	}
	pthread_cond_init(newQuest->notAnswer,NULL);
	return newQuest;
}
void destroyQuestions(Question *question)
{
	pthread_mutex_destroy(question->mutex);
    pthread_cond_destroy(question->notAnswer);
    free(question->mutex);
	free(question->notAnswer);
    free(question);
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
	for(int i=0;i<MAX_QUESTION;i++)
	{
		quests[i]=NULL;
	}
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
	 char message[256];
	 bzero(message,256);
	Room *room;
	for (int i = 0; i < room->no_player; i++)
	{
		if (room->players[i] != NULL)
		{
			modify_message(1,quest->quest,message);
			send(room->players[i]->socket, message, strlen(message), 0);
		}
	}
}
/*
Nhận câu trả lời từ client và kiểm tra kết quả
*/
// void receiveAnswer(Player *player, Question *quest)
// {
// 	char message[256];
// 	bzero(message, 256);
// 	int no_question;
// 	while(read(player->socket, message, sizeof(message)) >= 0)
// 	{
// 		/*
// 		Nếu checkAnswer nếu đúng trả về 1 thì cập nhật số điểm của players
// 		*/
// 		Message *mess=split_message(message);
// 		printf("%s\n",mess->body);
// 		int check = checkAnswer(quest, mess->body);
// 		if (check)
// 		{
// 			updatePoint(player, no_question, quest->point);
// 			printPlayers(player);
// 			printf("True at number %d\n",no_question);
// 		}
// 		else
// 		{
// 			printPlayers(player);
// 			printf("False at number %d\n",no_question);
// 		}
// 		bzero(message,256);
// 		break;
// 	}
// }
void getQuestions(Question *questions[MAX_QUESTION],char *file)
{
	for(int i=0;i<MAX_QUESTION;i++)
	{
		questions[i]=NULL;
	}
	readQuestsFromFile(questions,file);
}