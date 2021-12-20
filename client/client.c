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
#define STDIN_FILENO 0
#define MESS_BUFFER 256
static int network_socket;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void setNonBlock(int fd);
void joinedToServer(int sock);
void handle_signout();
void *receive(void *args);
void answerQuestion(char *quest, int socket);
void interruptHandler(int sig_unused);
void login(int sockff);
int main(int argc, const char **argv)
{

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
        joinedToServer(network_socket);
    }
    return 0;
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
}
void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        perror("fcntl failed");

    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}
void joinedToServer(int server_socket)
{

    char message[256];
    bzero(message, 256);
    char answer[50];
    bzero(message, 256);
    Message *messg;
    fd_set clientFds;
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
                            case LOGIN:
                                if (strcmp(messg->body, "succ") == 0)
                                {
                                    printf("Log in success!\n");
                                    bzero(message, 256);
                                    modify_message(START, "start", message);
                                    send(server_socket, message, strlen(message), 0);
                                }
                                else
                                {
                                    printf("Log in failed!\n");
                                }
                                break;
                            case QUESTION:
                                printf("Question :%s\n", messg->body);
                                printf("Your answers:\n");

                                break;
                            case MESSAGE:

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
                                break;
                            case ANSWER:
                                printf("%s\n", messg->body);
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
                        if (strcmp(answer, "exit") == 0)
                        {
                            printf("Sign out!\n");
                            interruptHandler(-1); // Reuse the interruptHandler function to disconnect the client
                        }

                        bzero(message, MESS_BUFFER);
                        modify_message(ANSWER, answer, message);
                        send(server_socket, message, strlen(message), 0);
                    }
                }
            }
        }
    }
    return;
}
// Notify the server when the client exits by sending "/exit"
void interruptHandler(int sig_unused)
{
    char message[MESS_BUFFER];
    bzero(message, MESS_BUFFER);
    modify_message(LOGOUT, "exit", message);
    if (write(network_socket, message, strlen(message)) == -1)
        perror("write failed: ");
    printf("Signed out!\n");
    close(network_socket);
    exit(1);
}