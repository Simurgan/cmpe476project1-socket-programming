#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CONNECTIONS 3
#define MAX_BUFFER_SIZE 14

int client_count = 0;

void *handle_connection(void *);

int main(int argc, char const *argv[])
{
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, MAX_CONNECTIONS);

    printf("Server has started\nWaiting for connections\n");
    while (1)
    {
        if (client_count < MAX_CONNECTIONS)
        {
            int socket_id = accept(server_fd, (struct sockaddr *)&address, &addrlen);
            pthread_t thread;
            int ret = pthread_create(&thread, NULL, handle_connection, (void *)&socket_id);
            ret = pthread_detach(thread);
            client_count++;
        }
        else
        {
            int socket_id = accept(server_fd, (struct sockaddr *)&address, &addrlen);
            close(socket_id);
        }
    }

    exit(0);
}

void *handle_connection(void *socket_id_ptr)
{
    int socket_id = *((int *)socket_id_ptr);
    char buffer[MAX_BUFFER_SIZE];
    ssize_t byte_length = read(socket_id, buffer, MAX_BUFFER_SIZE);
    int client_id = atoi(buffer);
    printf("Incoming request from client #%d\n", client_id);

    buffer[0] = 'E';
    while (buffer[0] != '-')
    {
        byte_length = read(socket_id, buffer, MAX_BUFFER_SIZE);
        buffer[byte_length] = '\0';
        printf("(client #%d) Request=%s", client_id, buffer);
        if (buffer[0] == '-')
        {
            printf(" dropping connection\n");
            close(socket_id);
            client_count--;
            break;
        }
        printf("\n");

        int val = atoi(buffer);
        char response[MAX_BUFFER_SIZE];
        sprintf(response, "%d", val * val);
        send(socket_id, response, strlen(response), 0);
    }
}
