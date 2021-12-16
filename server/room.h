#include "player.h"
typedef enum State
{
    READY,
    PLAYING,
    WAITING
}State;

typedef struct room
{
    int roomid;
    Player *player;
    State state;
} Room;

Room *initRoom();
void printRoom(Room *room);