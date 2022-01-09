
#ifndef ENUM_COMMAND
#define ENUM_COMMAND
typedef enum commnand
{
    LOGIN, //login
    LOGOT, //logout
    CRTRM, //create room
    GETRM,// Get list rooms
    JOINR, //join game
    STATG, //Start Game
    STOPG, //Stop game
    EXITG, //Exit game
    SURRG, //Surrend Game
    ANSWR, //Answer questin
    QUEST, //Question
    MESSG, //Message
    SCHRM, //SCHRM
    BACK, //Quay trở vể home menu
    WAIT, // gửi thông tin đến client cần đợi 
    READY, // gửi thông tin đến client là người chơi sắn sàng 
    RESULT, //gửi kết quả đến client 
    MESSG_NOT_FOUND, // gửi kết quả không tìm thấy đến client 
    BACK_OK, // quay trở lại thành công đến client 
    OPONENT_JOIN // guiwt thông báo đến client là người chơi đã tham gia phòn

}Command;

#endif //ENUM_COMMAND