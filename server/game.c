#include "game.h"
#include "pthread.h"
#include "../utils/utils.h"
extern pthread_mutex_t mutex;
extern int no_question;
extern Player *players;
extern Room *room;
extern pthread_cond_t cond;
extern Question *questions[MAX_QUESTION];
Question *quest = NULL;
void playGame(Player *player)
{

    
    int no = 0;
    while (no < 4)
    {
        no=no_question;
        
        pthread_mutex_lock(&mutex);
        quest = questions[no];
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

    Message *mess;
    char message[256];
    bzero(message, 256);
    while (recv(user->socket, message, sizeof(message), 0) > 0)
    {
        mess = split_message(message);
        // Khi nhận được yêu câu băt đầu từ client , bắt đấù chơi game
        if (strcmp(mess->body, "start") == 0)
        {
            Player *player = searchPlayer(players, user->username);
            if (player == NULL)
            {
                printf("Player not found\n");
            }
            else
            { // play game
                pthread_mutex_lock(&mutex);
                while (room->no_player < 2)
                {
                    bzero(message, 256);
                    modify_message(1, "wait", message);
                    send(player->socket, message, strlen(message), 0);
                    pthread_cond_wait(&cond, &mutex);
                }
                pthread_mutex_unlock(&mutex);
                if (questions[0] == NULL)
                {
                    getQuestions(questions, "../file/question.txt");
                }

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