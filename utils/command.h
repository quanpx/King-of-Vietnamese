
#ifndef ENUM_COMMAND
#define ENUM_COMMAND
typedef enum commnand
{
    LOGIN,  // Đăng nhập
    LOGOT,  // Đăng xuất
    SIGNU,  // Đăng ký
    CRTRM,  // Tạo phòng
    GETRM,  // Lấy danh sách phòng
    JOINR,  // Tham gia vào một phòng
    STATG,  // Bắt đầu trò chơi
    ANSWR,  // Trả lời câu hỏi
    CHAT,   // Nhắn tin
    BACK,   // Quay trở vể home menu
    QUEST,  // Gủi câu hỏi tới client
    MESSG,  // Gửi thông điêp tới client
    WAIT,   // gửi thông tin đến client cần đợi 
    READY,  // gửi thông tin đến client là người chơi sắn sàng 
    RESULT, //gửi kết quả đến client 
    MESSG_NOT_FOUND, // gửi kết quả không tìm thấy đến client 
    BACK_OK, // quay trở lại thành công đến client 
    OPONENT_JOIN, // gửi thông báo đến client là người chơi đã tham gia phòng,
    CORRECT,  // Gửi thông điệp client trả lời đúng
    INCORRECT, // Gửi thông điệp client trả lời sai
    ACCOUNT_EXIST, // Gửi thông điệp tới client tài khoản đã tồn tại,
    ACCOUNT_SUCCESS //Đăng ký thành công 

}Command;

#endif //ENUM_COMMAND