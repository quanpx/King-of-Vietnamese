#include "room.h"
Room *room = NULL;
int room_id = 1;
Room *initRoom()
{
    Room *room = (Room *)malloc(sizeof(Room));
    if (room == NULL)
    {
        printf("Allocate memory failed!");
    }
    room->roomid = room_id;
    for (int i = 0; i < MAX_PLAYER; i++)
    {
        room->players[i] = NULL;
    }
    room->state = WAITING;
    room->no_player = 0;
    return room;
}
void printRoom(Room *room)
{
    printf("Room id: %d state %d\n", room->roomid, room->state);
    for(int i=0;i<room->no_player;i++)
    {
        if(room->players[i]!=NULL)
        {
            printf("Username: %s ( socket : %d )\n",room->players[i]->username,room->players[i]->socket);
        }
    }
}
void setState(Room *room, int state)
{
    room->state = state;
    printf("Update room %d to state %d", room->roomid, state);
}
void addRoom(Room *rooms[MAX_ROOM], Room *room)
{
    int i;
    for (i = 0; i < MAX_ROOM; i++)
    {
        if (!rooms[i])
        {
            rooms[i] = room;
            break;
        }
    }
    if (i == MAX_ROOM)
    {
        printf("MAX ROOM!");
    }
}
Room *searchRoom(Room *rooms[MAX_ROOM], int roomid)
{
    for (int i = 0; i < MAX_ROOM; i++)
    {
        if (rooms[i]->roomid == roomid)
        {
            return rooms[i];
        }
    }
    return NULL;
}
void addPlayerToRoom(Room *room, Player *player)
{
    int i;
    for (i = 0; i < MAX_PLAYER; i++)
    {
        if (room->players[i] == NULL)
        {
            room->players[i] = player;
            room->no_player++;
            break;
        }
    }
    if (i == MAX_PLAYER)

    {
        printf("Room full!");
    }
    return;
}
