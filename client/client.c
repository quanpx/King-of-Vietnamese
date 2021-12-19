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
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#define STDIN_FILENO 0
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *joinedToServer(void *args);
void handle_signout();
void *receive(void *args);
void answerQuestion(char *quest, int socket);
int read_from_stdin(char *read_buffer, size_t max_len)
{
    memset(read_buffer, 0, max_len);

    ssize_t read_count = 0;
    ssize_t total_read = 0;

    do
    {
        read_count = read(STDIN_FILENO, read_buffer, max_len);
        if (read_count < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("read()");
            return -1;
        }
        else if (read_count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            break;
        }
        else if (read_count > 0)
        {
            total_read += read_count;
            if (total_read > max_len)
            {
                printf("Message too large and will be chopped. Please try to be shorter next time.\n");
                fflush(STDIN_FILENO);
                break;
            }
        }
    } while (read_count > 0);

    size_t len = strlen(read_buffer);
    if (len > 0 && read_buffer[len - 1] == '\n')
        read_buffer[len - 1] = '\0';

    printf("Read from stdin %zu bytes. Let's prepare message to send.\n", strlen(read_buffer));

    return 0;
}
int main(int argc, const char **argv)
{

    signal(SIGINT, handle_signout);
    // create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    // fcntl(network_socket,F_SETFL,O_NONBLOCK);
    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = 5000;
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // check for connection_status
    if (connection_status == -1)
    {
        printf("The connection has error\n\n");
    }

    if (connection_status == 0)
    {
        char userid[20];
        char password[20];
        char message[256];
        char username_password[100];
        // receive data from the server
        int bytes = 0;
        pthread_t th[2];
        while (1)
        {
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
            modify_message(1, username_password, message);
            int send_status = send(network_socket, message, strlen(message), 0);
            bzero(message, 256);
            if (send_status != -1)
            {
                bytes += send_status;
            }

            if (recv(network_socket, message, sizeof(message), 0) > 0)
            {
                Message *mess = split_message(message);
                bzero(message, 256);
                // print out the server's response
                if (strcmp(mess->body, "succ") == 0)
                {
                    printf("Login successful!\n");

                    pthread_create(&th[0], NULL, joinedToServer, &network_socket);
                    //   pthread_create(&th[1], NULL, receive, &network_socket);
                    pthread_join(th[0], NULL);
                    //  pthread_join(th[1], NULL);
                    return 1;
                }
                else if (strcmp(mess->body, "fail") == 0)
                {
                    printf("Invalid info! Try again!\n");
                }
                else
                {
                    printf("%s\n", "User not exist!");
                    printf("Please try a again\n");
                }
            }
        }
    }
    // close the socket
    close(network_socket);

    return 0;
}

void answerQuestion(char *quest, int server_socket)
{
    char answer[50];
    char message[256];
    bzero(answer, 50);
    printf("Question: %s\n", quest);
    printf("Your answers:\n");
    fgets(answer, 50, stdin);
    answer[strlen(answer) - 1] = '\0';
    modify_message(1, answer, message);
    send(server_socket, message, strlen(message), 0);
}
void *joinedToServer(void *args)
{
    int server_socket = *(int *)args;
    int maxfd = server_socket;
    fd_set currentfds, readfds;
    FD_ZERO(&currentfds);
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &currentfds);
    FD_SET(server_socket, &currentfds);
    char message[256];
    bzero(message, 256);
    modify_message(1, "start", message);
    send(server_socket, message, strlen(message), 0);
    bzero(message, 256);
    Message *messg;

    while (1)
    {
        readfds = currentfds;
        int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        switch (activity)
        {
        case -1:
            printf("Select failed\n");
            break;
        case 0:
            printf("Time out\n");
            break;
        default:
            for (int i = 0; i < maxfd + 1; i++)
            {
                if (FD_ISSET(i, &readfds))
                {
                    if (i == STDIN_FILENO)
                    {
                        continue;
                    }
                    else
                    {
                        int rc = recv(server_socket, message, sizeof(message), 0);
                        if (rc > 0)
                        {
                            messg = split_message(message);
                            if (strcmp(messg->body, "done") == 0)
                            {
                                printf("You finished your game!Bye\n");
                            }
                            else if (strcmp(messg->body, "wait") == 0)
                            {
                                printf("Waiting other player...\n");
                            }
                            else
                            {
                                answerQuestion(messg->body, server_socket);
                                bzero(message, 256);
                            }
                        }
                    }
                }
            }
        }
    }
    // while (recv(server_socket, message, sizeof(message), 0) > 0)
    // {
    //     messg = split_message(message);
    //     if (strcmp(messg->body, "done") == 0)
    //     {
    //         printf("You finished your game!Bye\n");
    //     }
    //     else if (strcmp(messg->body, "wait") == 0)
    //     {
    //         printf("Waiting other player...\n");
    //     }
    //     else
    //     {
    //         answerQuestion(messg->body, server_socket);
    //         bzero(message, 256);
    //     }
    // }
    return NULL;
}
void handle_signout()
{
    printf("You signed out!");
    exit(0);
}