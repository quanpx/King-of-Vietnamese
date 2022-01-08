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
void answerQuestion(char *quest, int socket);
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
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

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
    char enter[5];
    char roomId[2];
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

                            messg = split_message(message);
                            switch (messg->cmdType)
                            {
                            case LOGIN:
                                if (strcmp(messg->body, "succ") == 0)
                                {
                                    printf("Log in success!\n");
                                    sleep(1);
                                    system("clear");
                                    homeMenu();
                                }
                                else
                                {
                                    printf("Log in failed!\nTry again! \n");
                                    setBlock(0);
                                    login(server_socket);
                                }
                                break;

                            case CRTRM:
                                strcpy(roomId, messg->body);
                                printf("You have created room %s\n", messg->body);
                                sleep(1);
                                system("clear");
                                showMenuInRoom(roomId);
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
                            default:
                                break;
                            }
                            bzero(message, MESS_BUFFER);
                        }
                    }
                    else if (fd == 0) // read from keyboard (stdin) and send to server
                    {
                        int select = 0;
                        scanf("%d", &select);
                        while (getchar() != '\n')
                        {
                        }

                        switch (select)
                        {
                        case 1: // create room
                            createRoom(server_socket);
                            break;
                        case 2: // Join room
                            getRoom(server_socket);
                            selectRoom(server_socket);
                            break;
                        case 3: // start game
                            startGame(server_socket, roomId);
                            playGame(server_socket);
                            break;
                        case 4: // join room
                            getRoom(server_socket);
                            break;
                        case 5: // start game

                            break;
                        case 6:

                            break;
                        case 7:

                            break;
                        case 8:

                            break;
                        case 9:

                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
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

void selectRoom(int server_socket)
{
    char message[256];
    bzero(message, 256);
    Message *messg;
    setNonBlock(0);
    setNonBlock(server_socket);
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

                            messg = split_message(message);
                            switch (messg->cmdType)
                            {
                            case GETRM:
                                printf("You request to get rooms\n");
                                sleep(1);
                                system("clear");
                                printf("Available rooms\n");
                                printf("%s\n", messg->body);
                                printf("Enter room id to join room.Enter 0 for exist\n");
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
                            default:

                                break;
                            }
                            bzero(message, MESS_BUFFER);
                        }
                    }
                    else if (fd == 0) // read from keyboard (stdin) and send to server
                    {

                        int select = 0;
                        char choice[2];
                        // fflush(stdin);
                        // select = atoi(fgets(choice, 2, stdin));
                        scanf("%d", &select);
                        while (getchar() != '\n')
                        {
                        }

                        bzero(choice, 2);
                        convert_number_to_string(select, choice);
                        bzero(message, MESS_BUFFER);
                        modify_message(JOINR, choice, message);
                        write(server_socket, message, strlen(message));

                        playGame(server_socket);
                    }
                }
            }
        }
    }
}
void startGame(int server_socket, char *roomId)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(STATG, roomId, message);
    write(server_socket, message, strlen(message));
}

void homeMenu()
{
    printf("============ Welcome to King of Vietnamese ================\n");
    printf("Please select one!\n");
    printf("1. Create a room\n");
    printf("2. Join a room\n");
    printf("0. Exit\n");
    printf("==================== Thank you! ===========================\n");
}
void showMenuInRoom(char *roomId)
{
    printf("============ Welcome to Room %s ================\n", roomId);
    printf("Please select one!\n");
    printf("3. Start game\n");
    printf("4. Back\n");
    printf("0. Exit\n");
    printf("==================== Thank you! ===========================\n");
}
void createRoom(int sock)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(CRTRM, "create rooom", message);
    write(sock, message, strlen(message));
}
void getRoom(int sockfd)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(GETRM, "get", message);
    write(sockfd, message, strlen(message));
}
void playGame(int server_socket)
{

    char message[256];
    bzero(message, 256);
    char answer[50];
    bzero(message, 256);
    Message *messg;
    fd_set clientFds;
    setNonBlock(0);
    setNonBlock(server_socket);
    char roomId[2];
    bzero(roomId, 2);
    while (1)
    {
        // Reset the fd set each time since select() modifies it
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

                            messg = split_message(message);

                            switch (messg->cmdType)
                            {
                            case JOINR:
                                strcpy(roomId, messg->body);
                                printf("You have joined room\n");
                                sleep(1);
                                printf("%s\n", messg->body);
                                system("clear");
                                showMenuInRoom(roomId);

                                break;

                            case QUEST:
                                printf("Question :%s\n", messg->body);
                                printf("Your answers:\n");

                                break;
                            case MESSG:

                                if (strcmp(messg->body, "wait") == 0)
                                {
                                    printf("Waiting other player...\n");
                                    sleep(1);
                                    system("clear");
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
                                    sleep(1);
                                    system("clear");
                                }
                                else if (strcmp(messg->body, "back") == 0)
                                {
                                    printf("%s\n", messg->body);
                                }
                                else if (strcmp(messg->body, "ready") == 0)
                                {
                                    printf("Oponent has ready!");
                                }
                                else
                                {
                                    printf("%s\n", messg->body);
                                    printf("Enter 'back'.\n");
                                    fflush(stdout);
                                }
                                break;
                            case ANSWR:
                                printf("%s\n", messg->body);
                                sleep(2);
                                system("clear");
                                break;
                            default:
                                break;
                            }
                            bzero(message, MESS_BUFFER);
                        }
                    }
                    else if (fd == 0) // read from keyboard (stdin) and send to server
                    {

                        fgets(answer, 50, stdin);
                        answer[strlen(answer) - 1] = '\0';
                        if (strcmp(answer, "back") == 0)
                        {
                            sendBackRequest(server_socket);
                        }
                        else
                        {
                            bzero(message, MESS_BUFFER);
                            modify_message(ANSWR, answer, message);
                            write(server_socket, message, strlen(message));
                        }
                    }
                }
            }
        }
    }
}

void sendBackRequest(int server_socket)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(BACK, "back", message);
    write(server_socket, message, strlen(message));
}