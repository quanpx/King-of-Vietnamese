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
void joinedToServer(int socket);
void handle_signout();
int main(int argc, const char **argv)
{
    signal(SIGINT, handle_signout);
    // create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

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
        memset(username_password, 0, 100);
        // receive data from the server
        int bytes = 0;
        while (1)
        {
            memset(message, 0, 256);

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
            bzero(message,256);
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
                    joinedToServer(network_socket);
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
        return 0;
    }
    // close the socket
    close(network_socket);

    return 0;
}
void answerQuestion(int server_socket)
{
    char answer[50];
    char message[256];
    bzero(answer, 50);
    printf("Your answers:\n");
    fgets(answer, 50, stdin);
    answer[strlen(answer) - 1] = '\0';
    modify_message(1, answer, message);
    send(server_socket, message, strlen(message), 0);
}
void joinedToServer(int server_socket)
{
    Message *messg;
    char message[256];
    bzero(message, 256);
    modify_message(1, "start", message);
    send(server_socket, message, strlen(message), 0);
    bzero(message, 256);
    while (recv(server_socket, message, sizeof(message), 0) > 0)
    {
        messg = split_message(message);
        if (strcmp(messg->body, "done") == 0)
        {
            printf("You finished your game!Bye\n");
            return;
        }
        else if(strcmp(messg->body,"wait")==0)
        {
            printf("Waiting other player...\n");
        }else
        {
            printf("Question:%s\n", messg->body);
            answerQuestion(server_socket);
            bzero(message, 256);
        }
    }
}
void handle_signout()
{
    printf("You signed out!");
    exit(0);
}