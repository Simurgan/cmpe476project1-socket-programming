#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 14

int main(int argc, char const *argv[])
{
    // Extract client ID from command line arguments
    const char *client_id = argv[1];
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;

    // Create socket file descriptor
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Initialize server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Connect to server
    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }

    char buffer[MAX_BUFFER_SIZE];

    // Send client ID to server
    send(client_fd, client_id, strlen(client_id), 0);
    printf("Waiting for the server to accept connection...\n");

    // Read acknowledgment from server to understand the server will serve now
    read(client_fd, buffer, MAX_BUFFER_SIZE);
    printf("Connection established.\n");
    printf("This is client #%s\nEnter request (negative to terminate): ", client_id);

    char str[50];
    while (fgets(str, 50, stdin) != NULL)
    {
        // Remove newline character from input
        int new_line_index = strlen(str) - 1;
        if (str[new_line_index] == '\n')
        {
            str[new_line_index] = '\0';
        }

        // Send request to server
        send(client_fd, str, strlen(str), 0);
        if (str[0] == '-')
        {
            // Terminate if the value is negative
            printf("\tWill terminate\n");
            break;
        }

        // Read and display server response
        valread = read(client_fd, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0';
        printf("\tResult: %s\n", buffer);
        printf("Enter request (negative to terminate): ");
    }
}