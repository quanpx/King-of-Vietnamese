
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
    BACK,
    WAIT,
    READY,
    RESULT,
    MESSG_NOT_FOUND,
    BACK_OK

}Command;

#endif //ENUM_COMMAND