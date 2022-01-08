#include "game.h"

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
Game *initGame()
{
    Game *game = (Game *)malloc(sizeof(Game));
    readQuestsFromFile(game->questions, "../file/question.txt");
    game->isPlay = 0;
    game->isAnswered = 0;
    game->room=NULL;
    game->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));

    if (game->mutex == NULL)
    {
        printf("Allocate failed!\n");
        exit(0);
    }
    pthread_mutex_init(game->mutex, NULL);
    game->cond_players = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    if (game->cond_players == NULL)
    {
        printf("Allocate failed!\n");
        exit(0);
    }
    pthread_cond_init(game->cond_players, NULL);
    game->cond_answered = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    if (game->cond_answered == NULL)
    {
        printf("Allocate failed!\n");
        exit(0);
    }
    pthread_cond_init(game->cond_answered, NULL);

    return game;
}