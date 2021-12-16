#include"room.h"
int roomid=0;
Room *initRoom()
{
    Room *room=(Room*)malloc(sizeof(Room));
    if(room==NULL)
    {
        printf("Allocate memory failed!");
    }
    room->roomid=roomid;
    room->player=NULL;
    room->state=WAITING;
    return room;
}
void printRoom(Room *room)
{
    printf("Room id: %d state %d\n",room->roomid,room->state);
    if(room->player!=NULL)
    {
        printPlayers(room->player);
    }
}