#include "player.h"
#define MAX_ROOM 10
#define MAX_PLAYER 2
#ifndef ENUM_STATE
#define ENUM_STATE
typedef enum State
{
    READY,
    PLAYING,
    WAITING
}State;
#endif /*ENUM_STATE */
#ifndef STRUCT_ROOM
#define STRUCT_ROOM
typedef struct room
{
    int roomid;
    Player *players[MAX_PLAYER];
    State state;
    int no_player;

} Room;
#endif /*STRUCT_ROOM*/

Room *initRoom();
void printRoom(Room *room);
void setState(Room *room,int state);
void addRoom(Room *rooms[MAX_ROOM],Room *room);
Room* searchRoom(Room *rooms[MAX_ROOM],int roomid);
void addPlayerToRoom(Room *room,Player *player);