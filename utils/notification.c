#include "notification.h"
struct notification{
    noti_type messType;
    char instruction[MAX_BUFFER] ;
} Notifications[MAX_TYPE_NOTI]=
{
    {LOGIN_SUCCESS,"Đăng nhập thành công!"},
    {LOGIN_FAILED,"Đăng nhập thất bại!"},
    {ACCOUNT_NOT_EXIST,"Tài khoản không tồn tại"},
    {WON,"Bạn thắng"},
    {LOSED,"Bạn thua"}

};

char *getNotification(noti_type type)
{
    for(int i=0;i<MAX_TYPE_NOTI;i++)
    {
        if(Notifications[i].messType==type)
        {
            return Notifications[i].instruction;
        }
    }
}

// void main()
// {
//     printf("%s\n",getNotification(LOGIN_SUCCESS));
// }