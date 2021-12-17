#include "authentication.h"
#include "game.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
extern User* users;
extern Room* room;
// Xử lý chơi game
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
void *connection_handler(void *client_socket)
{
	int socket = *(int *)client_socket;
	char client_message[100];
	while (recv(socket, client_message, 100, 0) > 0)
	{
		// Xử lý thông tin đăng nhập, sau khi đăng nhập thành công trả về thông tin của user
		User *user = handleLogin(client_message, socket);
		if(user!=NULL)
		{
			printf("%s at socket %d joined! \n", user->username, user->socket);
		}
		
		send(socket, client_message, strlen(client_message), 0);
		if (strcmp(client_message, "succ") == 0)
		{
			// Sau khi đăng nhập thành công , client tham gia vào phòng
			clientJoined(user);
			
		}
		bzero(client_message, 100);
	}

	return 0;
}

// Remember to use -pthread when compiling this server
int main(int argc, char **argv)
{

	//Đọc user từ file
	room=initRoom();
	readUsersFromFile(&users, "../file/user.txt");

	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket == -1)
	{
		perror("Socket initialisation failed");
		exit(EXIT_FAILURE);
	}
	else
		printf("Server socket created successfully\n");

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = 5000;
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to the specified IP addr and port
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
	{
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	if (listen(server_socket, 3) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	int no = 0;
	pthread_t threads[2];
	printf("Listening...\n");
	while (no < 3)
	{
		int client_socket = accept(server_socket, NULL, NULL);
		printf("Connection accepted\n");
		if (pthread_create(&threads[no], NULL, &connection_handler, &client_socket) < 0)
		{
			perror("Could not create thread");
			return 1;
		}
		if (client_socket < 0)
		{
			printf("server acccept failed...\n");
			exit(0);
		}
		else
			printf("Server acccept the client...\n");
		puts("Handler assigned");
	}
	for (int i = 0; i < 3; i++)
	{
		if (pthread_join(threads[i], NULL) != 0)
		{
			printf("Join thread error!\n");
		}
	}

	// int send_status;
	// send_status=send(client_socket, server_message, sizeof(server_message), 0);
	close(server_socket);

	return 0;
}
