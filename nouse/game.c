#include "game.h"
#include "pthread.h"
#include "../utils/utils.h"
void playGame(Player *player)
{
    int no_question;
    char message[256];
    bzero(message, 256);

    int no = no_question; // 0
    while (no < 4)
    {
    }

    modify_message(1, "done", message);
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
            //     pthread_mutex_lock(&mutex);
            //     while (room->no_player < 2)
            //     {
            //     bzero(message, 256);
            //     modify_message(1, "wait", message);
            //     send(player->socket, message, strlen(message), 0);
            //     pthread_cond_wait(&cond, &mutex);
            //     }
            //     pthread_mutex_unlock(&mutex);
            //     if (questions[0] == NULL)
            //     {
            //         getQuestions(questions, "../file/question.txt");
            //     }

            //     playGame(player);
            //
        }
    }
}