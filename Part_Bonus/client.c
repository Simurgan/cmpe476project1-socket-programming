#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 14

int main(int argc, char const *argv[])
{
    // Extract client ID from command line arguments
    int port = atoi(argv[1]);
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;

    // Create socket file descriptor
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Initialize server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Connect to server
    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }

    char buffer[MAX_BUFFER_SIZE];
    printf("Connection established.\n");
    printf("Enter request : ");

    char str[50];
    fgets(str, 50, stdin);
    // Remove newline character from input
    int new_line_index = strlen(str) - 1;
    if (str[new_line_index] == '\n')
    {
        str[new_line_index] = '\0';
    }

    // Send request to server
    send(client_fd, str, strlen(str), 0);

    // Read and display server response
    valread = read(client_fd, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0';
    printf("\tResult: %s\n", buffer);
}