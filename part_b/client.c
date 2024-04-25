#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CONNECTIONS 3
#define MAX_BUFFER_SIZE 14

int main(int argc, char const *argv[])
{
    const char *client_id = argv[1];
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER_SIZE];
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }

    char str[50];
    send(client_fd, client_id, strlen(client_id), 0);
    printf("This is client #%s\nEnter request (negative to terminate): ", client_id);
    while (fgets(str, 50, stdin) != NULL)
    {
        int new_line_index = strlen(str) - 1;
        if (str[new_line_index] == '\n')
        {
            str[new_line_index] = '\0';
        }
        send(client_fd, str, strlen(str), 0);
        if (str[0] == '-')
        {
            printf("\tWill terminate\n");
            break;
        }
        valread = read(client_fd, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0';
        printf("\tResult: %s\n", buffer);
        printf("Enter request (negative to terminate): ");
    }
}