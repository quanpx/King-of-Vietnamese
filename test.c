#include"question.h"
#include "player.h"
int main()
{
    // Question *questions=NULL;
    // readQuestsFromFile(&questions,"./file/question.txt");
    // printQuests(questions);
    int points[14];
    for(int i=0;i<14;i++)
    {
        points[i]=i;
    }
    Player *player=(Player*)malloc(sizeof(Player));
    strcpy(player->username,"Quan");
   for(int i=0;i<14;i++)
   {
       printf("%d ",points[i]);
       player->point[i]=points[i];
   }
   printf("Total point: %d",getTotalPoint(player));

}