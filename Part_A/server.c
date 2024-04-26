#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CONNECTIONS 3 // Change this to limit different number of waiting connections in the queue
#define MAX_BUFFER_SIZE 14

void handle_connection(int);

int main(int argc, char const *argv[])
{
    int server_fd;

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Set socket options to reuse address and port
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // Define server address
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen for incoming connections (waiting connections may queue up to number of "MAX_CONNECTIONS")
    // However, the limiting may not work as expected for some environments
    listen(server_fd, MAX_CONNECTIONS);

    // Server setup message
    printf("Server has started\nWaiting for connections\n");

    // Accept and handle incoming connections
    while (1)
    {
        int socket_id = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        handle_connection(socket_id);
    }

    exit(0);
}

// Function to handle each connection
void handle_connection(int socket_id)
{
    char buffer[MAX_BUFFER_SIZE]; // Buffer to store incoming data

    // Read data from the client (first message is client id)
    ssize_t byte_length = read(socket_id, buffer, MAX_BUFFER_SIZE);

    buffer[byte_length] = '\0';                 // Cut the incoming data from its end to prevent issues due to overriding old messages
    send(socket_id, buffer, strlen(buffer), 0); // Echo client_id to the client back to let it know the server will start serving him / connection established
    int client_id = atoi(buffer);
    printf("Connection established with client #%d\n", client_id);

    buffer[0] = 'E';
    while (buffer[0] != '-') // As long as connection is not terminated by the client
    {
        byte_length = read(socket_id, buffer, MAX_BUFFER_SIZE); // Read incoming message
        buffer[byte_length] = '\0';                             // Cut the incoming data from its end to prevent issues due to overriding old messages

        printf("(client #%d) Request=%s", client_id, buffer);
        if (buffer[0] == '-') // If the connection is terminated by the client
        {
            printf(" dropping connection\n");
            close(socket_id);
            break;
        }
        printf("\n");

        int val = atoi(buffer);

        // Prepare response
        char response[MAX_BUFFER_SIZE];
        sprintf(response, "%d", val * val);

        // Send response to the client
        send(socket_id, response, strlen(response), 0);
    }
}
