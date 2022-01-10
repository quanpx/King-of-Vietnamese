#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include "../utils/utils.h"
#include "../utils/command.h"
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/ioctl.h>
#define STDIN_FILENO 0
#define MESS_BUFFER 256
static int network_socket;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void setNonBlock(int fd);
void setBlock(int fd);
void handle_signout();
void *receive(void *args);
void answerQuestion(int socket, char *answer);
void interruptHandler(int sig_unused);
void login(int sockff);
void playGround(int sock);
void createRoom();
void joinRoom(int socket, char *roomId);
void startGame(int sock, char *roomId);
void startClient(int sock);

// Tham gia vao phong da tao
void joinRoomCreated(int sock);
void getRoom(int socket);
void inRoom(int socket, char *roomId);
void sendBackRequest(int server_socket);

// menu
void homeMenu();
void showMenuInRoom(char *roomId);

// void playGame(int server_socket);
int main(int argc, const char **argv)
{
    char roomId[2];

    signal(SIGINT, interruptHandler);
    // create a socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // fcntl(network_socket,F_SETFL,O_NONBLOCK);
    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5000);
    server_address.sin_addr.s_addr = inet_addr("192.168.56.102");

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // check for connection_status
    if (connection_status == -1)
    {
        printf("The connection has error\n\n");
    }

    if (connection_status == 0)
    {
        login(network_socket);
        setNonBlock(0);
        setNonBlock(network_socket);
        startClient(network_socket);
    }
    return 0;
}
void startClient(int server_socket)
{
    char message[256];
    bzero(message, 256);
    Message *messg;
    char command[20];
    char roomId[2];
    char type[20];
    char body[20];
    while (1)
    {
        // Reset the fd set each time since select() modifies it
        fd_set clientFds;
        FD_ZERO(&clientFds);
        FD_SET(server_socket, &clientFds);
        FD_SET(0, &clientFds);
        if (select(FD_SETSIZE, &clientFds, NULL, NULL, NULL) != -1) // wait for an available fd
        {
            for (int fd = 0; fd < FD_SETSIZE; fd++)
            {
                if (FD_ISSET(fd, &clientFds))
                {
                    if (fd == server_socket) // receive data from server
                    {

                        while (read(server_socket, message, sizeof(message)) > 0)
                        {
                            bzero(command, 20);

                            messg = split_message(message);
                            switch (messg->cmdType)
                            {
                            case LOGIN:
                                if (strcmp(messg->body, "succ") == 0)
                                {
                                    printf("Đăng nhập thành công!\n");
                                    sleep(1);
                                    system("clear");
                                    homeMenu();
                                }
                                else
                                {
                                    printf("Đăng nhập thất bại!\nThử lại! \n");
                                    setBlock(0);
                                    login(server_socket);
                                }
                                break;

                            case CRTRM:
                                strcpy(roomId, messg->body);
                                printf("Bạn đã tạo phòng %s\n", messg->body);
                                sleep(1);
                                system("clear");
                                showMenuInRoom(roomId);
                                break;
                            case GETRM:
                                printf("You request to get rooms\n");
                                sleep(1);
                                system("clear");
                                printf("Phòng có sẵn \n");
                                printf("%s\n", messg->body);
                                printf("Nhập 'joinr <room id> ' để tham gia phòng!\n");
                                break;
                            case JOINR:
                                strcpy(roomId, messg->body);
                                printf("Bạn đã tham gia phòng\n");
                                printf("%s\n", messg->body);
                                sleep(1);
                                system("clear");
                                showMenuInRoom(roomId);

                                break;
                            case OPONENT_JOIN:
                                printf("%s\n", messg->body);
                                break;
                            case MESSG:
                                if (strcmp(messg->body, "wait") == 0)
                                {
                                    printf("Waiting other player...\n");
                                }
                                else if (strcmp(messg->body, "done") == 0)
                                {
                                    printf("You finished your game!Bye\n");
                                }
                                else if (strcmp(messg->body, "start") == 0)
                                {
                                    for (int i = 3; i >= 1; i--)
                                    {
                                        printf("%d\n", i);
                                        sleep(1);
                                    }
                                    printf("Start!\n");
                                }

                                else
                                {
                                    printf("%s\n", message);
                                }
                                break;
                            case QUEST:
                                system("clear");
                                printf("Câu hỏi:%s\n", messg->body);
                                printf("Nhập 'answr <đáp án>' để trả lời câu hỏi!\n");
                                printf("Đáp án của bạn:\n");

                                break;
                            case ANSWR:
                                printf("%s\n", messg->body);
                                sleep(2);
                                system("clear");
                                break;
                            case RESULT:
                                printf("%s\n", messg->body);
                                printf("Trò chơi kết thúc!\n");
                                sleep(2);
                                system("clear");
                                showMenuInRoom(roomId);

                                break;
                            case WAIT:
                                printf("%s\n", messg->body);
                                printf("Nhập 'start' để bắt đầu!\n");
                                break;
                            case READY:
                                printf("%s\n", messg->body);
                                break;
                            case MESSG_NOT_FOUND:
                                printf("%s\n", messg->body);
                                break;
                            case BACK:
                                printf("%s\n", messg->body);
                                break;
                            case BACK_OK:
                                system("clear");
                                homeMenu();
                                break;
                            default:
                                break;
                            }
                            bzero(message, MESS_BUFFER);
                        }
                    }
                    else if (fd == 0) // read from keyboard (stdin) and send to server
                    {

                        bzero(command, 20);
                        fgets(command, 50, stdin);
                        command[strlen(command) - 1] = '\0';
                        bzero(type, 20);
                        bzero(body, 20);
                        if (strlen(command) < 1)
                        {
                            printf("Câu lệnh không phù hợp!\n");
                        }
                        else
                        {
                            split_command(command, type, body);

                            if (strcmp(type, "crtrm") == 0)
                            {

                                createRoom(server_socket);
                            }
                            else if (strcmp(type, "listr") == 0)
                            {
                                getRoom(server_socket);
                            }
                            else if (strcmp(type, "joinr") == 0)
                            {
                                if (body == NULL || strlen(body) < 1)
                                {
                                    printf("Bạn cần nhập id phòng!\n");
                                }
                                else
                                {
                                    joinRoom(server_socket, body);
                                }
                            }
                            else if (strcmp(command, "start") == 0)
                            {
                                startGame(server_socket, roomId);
                            }
                            else if (strcmp(type, "answr") == 0)
                            {
                                answerQuestion(server_socket, body);
                            }
                            else if (strcmp(type, "back") == 0)
                            {
                                sendBackRequest(server_socket);
                            }
                        }
                    }
                }
            }
        }
    }
}
void homeMenu()
{
    printf("==================== %s =======================\n", "Chào mừng đến với Vua Tiếng Việt");
    printf("Hãy chọn chức năng!\n\n");
    printf("1. Tạo phòng : Nhập 'crtrm'\n");
    printf("2. Xem danh sách phòng 'listr'\n");
    printf("0. Exit\n");
    printf("============= Xin cảm ơn! =====================\n");
}
void showMenuInRoom(char *roomId)
{
    printf("============ Chào mừng đến phòng %s ===========\n", roomId);
    printf("Hãy chọn chức năng!\n\n");
    printf("1. Bắt đầu trò chơi : Nhập 'start'\n");
    printf("2. Quay trở lại : Nhập 'back'\n");
    printf("0. Exit\n");
    printf("============= Xin cảm ơn! =====================\n");
}
void getRoom(int sockfd)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(GETRM, "get", message);
    write(sockfd, message, strlen(message));
}
void createRoom(int sock)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(CRTRM, "create rooom", message);
    write(sock, message, strlen(message));
}
void joinRoom(int server_socket, char *roomId)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(JOINR, roomId, message);
    write(server_socket, message, strlen(message));
}
void login(int sockfd)
{
    char userid[20];
    char password[20];
    char username_password[100];
    char message[250];
    bzero(message, 250);
    bzero(username_password, 100);
    bzero(userid, 20);
    bzero(password, 20);

    printf("Enter user id:\n");
    fgets(userid, 20, stdin);
    printf("Enter pssword:\n");
    fgets(password, 20, stdin);
    userid[strlen(userid) - 1] = '\0';
    password[strlen(password) - 1] = '\0';
    strcat(username_password, userid);
    strcat(username_password, "-");
    strcat(username_password, password);
    modify_message(LOGIN, username_password, message);
    send(sockfd, message, strlen(message), 0);
    bzero(message, MESS_BUFFER);
}
void sendBackRequest(int server_socket)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(BACK, "back", message);
    write(server_socket, message, strlen(message));
}
void startGame(int server_socket, char *roomId)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(STATG, roomId, message);
    write(server_socket, message, strlen(message));
}
void answerQuestion(int server_socket, char *answer)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(ANSWR, answer, message);
    write(server_socket, message, strlen(message));
}
void setBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        perror("fcntl failed");
    flags &= ~O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}
void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        perror("fcntl failed");

    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}
void interruptHandler(int sig_unused)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(LOGOT, "exit", message);
    if (write(network_socket, message, strlen(message)) == -1)
        perror("write failed: ");
    printf("Signed out!\n");
    close(network_socket);
    exit(1);
}
