#include "room.h"
#include "../utils/utils.h"
Room *createRoom()
{
    FILE *fo = fopen("../file/room.txt", "r");
    if (fo == NULL)
    {
        printf("Open file failed!");
        exit(0);
    }
    int num_room;
    fscanf(fo, "%d", &num_room);
    fclose(fo);
    Room *room = (Room *)malloc(sizeof(Room));
    if (room == NULL)
    {
        printf("Allocate memory failed!");
    }
    room->roomid = num_room + 1;
    for (int i = 0; i < MAX_PLAYER; i++)
    {
        room->players[i] = NULL;
    }
    room->state = WAITING;
    room->no_player = 0;
    room->flag=0;
    return room;
}
void printRoom(Room *room)
{
    printf("Room id: %d state %d\n", room->roomid, room->state);
    for (int i = 0; i < room->no_player; i++)
    {
        if (room->players[i] != NULL)
        {
            printf("Username: %s ( socket : %d )\n", room->players[i]->username, room->players[i]->socket);
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
        if (rooms[i]!=NULL && rooms[i]->roomid == roomid)
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
void makeRoomsNull(Room *rooms[MAX_ROOM])
{
    for (int i = 0; i < MAX_ROOM; i++)
    {
        rooms[i] = NULL;
    }
}
void printAllRooms(Room *rooms[MAX_ROOM])
{
    for (int i = 0; i < MAX_ROOM; i++)
    {
        if (rooms[i] != NULL)
        {
            printRoom(rooms[i]);
        }
    }
}
void rooms_to_string(Room *rooms[MAX_ROOM], char *message)
{
    char room_info[50];
    char num_in_string[2];
    for (int i = 0; i < MAX_ROOM; i++)
    {
        bzero(room_info, 50);
        if (rooms[i] != NULL)
        {
            convert_number_to_string(rooms[i]->roomid,num_in_string);
            strcat(room_info, "room id:");
            strcat(room_info, num_in_string);
            strcat(room_info, "-");
            strcat(room_info, "players:");
            bzero(num_in_string, 2);
            convert_number_to_string(rooms[i]->no_player,num_in_string);
            strcat(room_info, num_in_string);
            strcat(room_info, "\n");
        }
        strcat(message, room_info);
    }
}
void readRoomsFromFile(Room *rooms[MAX_ROOM])
{
    FILE *fo = fopen("../file/room.txt", "r");
    if (fo == NULL)
    {
        printf("Open file failed!");
        exit(0);
    }
    int num_room;
    fscanf(fo, "%d", &num_room);
    int roomId, roomState, numPlayers;
    while (fscanf(fo, "%d %d %d", &roomId, &roomState, &numPlayers) != EOF)
    {
        Room *room = (Room *)malloc(sizeof(Room));
        if (room == NULL)
        {
            printf("Allocate memory failed!");
        }
        room->roomid=roomId;
        room->no_player=numPlayers;
        room->state=roomState==1?PLAYING:WAITING;
        addRoom(rooms,room);
    }
    fclose(fo);
}
int get_num_of_rooms(Room* rooms[MAX_ROOM])
{
    int num_rooms=0;
    for(int i=0;i<MAX_ROOM;i++)
    {
        if(rooms[i]!=NULL)
        {
            num_rooms++;
        }
    }
    return num_rooms;
}
void writeRoomsToFile(Room *rooms[MAX_ROOM])
{
    int num_rooms=get_num_of_rooms(rooms);
    FILE *fi=fopen("../file/room.txt","w");
    if(fi==NULL)
    {
        printf("Open file failed!");
        exit(0);
    }
    fprintf(fi,"%d\n",num_rooms);
    for(int i=0;i<MAX_ROOM;i++)
    {
        if(rooms[i]!=NULL)
        {
            fprintf(fi,"%d %d %d\n",rooms[i]->roomid,rooms[i]->state,rooms[i]->no_player);
        }
    }
    fclose(fi);
}
// void main()
// {
//     Room *rooms[MAX_ROOM];
//     makeRoomsNull(rooms);
//     readRoomsFromFile(rooms);
//     Room *room=createRoom();
//     addRoom(rooms,room);
//     printAllRooms(rooms);
//     writeRoomsToFile(rooms);
// }