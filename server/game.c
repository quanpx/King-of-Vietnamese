#include "game.h"
extern int no_question;
extern Player *players;
void playGame(Player *player)
{
    Question *questions[MAX_QUESTION];
    getQuestions(questions,"../file/question.txt");
    int no = 0;
    Question *quest = questions[no];
   
    while (no <= 3)
    {
        // gửi câu hỏi tới client
        sendQuestion(quest);
        // Nhận câu hỏi từ client
        receiveAnswer(player, quest);
        quest = questions[++no];
        no_question++;
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