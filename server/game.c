#include "game.h"
#include "pthread.h"
extern pthread_mutex_t mutex;
extern int no_question;
extern Player *players;
extern Room *room;
extern pthread_cond_t cond;
extern Question *questions[MAX_QUESTION];
void playGame(Player *player)
{
   pthread_exit(NULL);
    Question *quest = NULL;

    while (no_question < 4)
    {
        
        quest=questions[no_question];
        pthread_mutex_lock(&mutex);
        no_question++;
        pthread_mutex_unlock(&mutex);
         // gửi câu hỏi tới client
        sendQuestion(quest);
        // Nhận câu hỏi từ client
        receiveAnswer(player, quest);

    }
    send(player->socket, "done", strlen("done"), 0);
    return;
}
void clientJoined(User *user)
{

    char request[256];
    char response[256];
    bzero(response, 256);
    bzero(request, 256);
    while (recv(user->socket, request, sizeof(request), 0) > 0)
    {
        // Khi nhận được yêu câu băt đầu từ client , bắt đấù chơi game
        if (strcmp(request, "start") == 0)
        {
            Player *player = searchPlayer(players, user->username);
            if (player == NULL)
            {
                printf("Player not found\n");
            }
            else
            { // play game
                pthread_mutex_lock(&mutex);
                while(room->no_player<2)
                {
                   printf("Waiting player...!");
                   pthread_cond_wait(&cond,&mutex);
                }
                pthread_mutex_unlock(&mutex);
                getQuestions(questions,"../file/question.txt");
                playGame(player);
            }
        }
        else
        {
            close(user->socket);
            break;
        }
    }
}