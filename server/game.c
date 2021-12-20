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
    char message[256];
    bzero(message,256);

    int no=no_question; //0 
    while (no < 4)
    {

        pthread_mutex_lock(&mutex);
        quest = questions[ no_question++];
        // gửi câu hỏi tới client
        printQuestion(quest);
       
        pthread_mutex_unlock(&mutex);
        sendQuestion(quest);
        // Nhận câu hỏi từ client
        receiveAnswer(player, quest);
         no++;

    }

    modify_message(1,"done",message);
    write(player->socket, message, strlen(message));
    return;
}
void clientJoined(Player *player)
{

    Message *mess;
    char message[256];
    bzero(message, 256);
    while (read(player->socket, message, sizeof(message)) > 0)
    {
        mess = split_message(message);
        // Khi nhận được yêu câu băt đầu từ client , bắt đấù chơi game
        if (strcmp(mess->body, "start") == 0)
        {
            // Player *player = searchPlayer(players, user->username);
            // if (player == NULL)
            // {
            //     printf("Player not found\n");
            // }
            // else
             // play game
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
}