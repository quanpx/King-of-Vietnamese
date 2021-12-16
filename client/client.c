#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include<arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
void joinedToServer(int socket);
int main(int argc, const char **argv)
{
    // create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port =5000;
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
        char response[256];
        char send_message[256];
        memset(response, 0, 256);
        // receive data from the server
        int bytes = 0;
        int count=0;
        while (count<2)
        {
            memset(send_message, 0, 256);

            printf("Enter user id:\n");
            fgets(userid, 20, stdin);
            printf("Enter pssword:\n");
            fgets(password, 20, stdin);
            userid[strlen(userid) - 1] = '\0';
            password[strlen(password) - 1] = '\0';

            strcat(send_message, userid);
            strcat(send_message, "|");
            strcat(send_message, password);
            
            int send_status = send(network_socket, send_message, strlen(send_message), 0);
            if (send_status != -1)
            {
                bytes += send_status;
            }
          
            recv(network_socket, response, sizeof(response), 0);
            char mess[10];
            // print out the server's response
            if(strcmp(response,"succ")==0)
            {
                printf("Login successful!\n");
               
                joinedToServer(network_socket);
                return 1;
                
            }
            else if(strcmp(response,"fail")==0)
            {
                printf("Invalid info! Try again!\n");
                 memset(response, 0, 256);
                 count++;
            }else if(strcmp(response,"lckd")==0)
            {
                printf("Your account is locking\n");
                printf("Try different account\n");
            }
            else 
                {
                printf("%s\n","User not exist!");
                printf("Please try a again\n");

            }
           
           
        }
        strcat(send_message,"|lock");
        bzero(response,256);
        send(network_socket,send_message,strlen(send_message),0);
        recv(network_socket, response, sizeof(response), 0);
        printf("%s","Your account's temporarily lock cause over 5 times!\n");
        close(network_socket);
        return 0;
    }
    // close the socket
    close(network_socket);

    return 0;
}
void answerQuestion(int server_socket)
{
    char answer[50];
    bzero(answer,50);
    printf("Your answers:\n");
    fgets(answer,50,stdin);
    answer[strlen(answer)-1]='\0';
    send(server_socket,answer,strlen(answer),0);

}
void joinedToServer(int server_socket)
{
    char client_message[256];
    char response[256];
    bzero(client_message,256);
    bzero(response,256);
    send(server_socket,"start",6,0);
    while(recv(server_socket,response,sizeof(response),0)>0)
    {
        if(strcmp(response,"done")==0)
        {
            printf("You finished your game!Bye\n");
            return;
        }else
        {
        printf("Question:%s\n",response);
        answerQuestion(server_socket);
        bzero(response,256);
       
        }
         

    }
}
