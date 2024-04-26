#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define PORT_SQUARE 5010
#define PORT_CUBE 5020
#define MAX_CONNECTIONS 10 // Change this to limit different number of waiting connections in the queue
#define MAX_BUFFER_SIZE 14

void handle_request(int, const char *);
void sigchld_handler(int);

int main(int argc, char const *argv[])
{
    int server_fd_square, server_fd_cube;

    // Create sockets
    server_fd_square = socket(AF_INET, SOCK_STREAM, 0);
    server_fd_cube = socket(AF_INET, SOCK_STREAM, 0);

    // Set socket options to reuse addresses and ports
    int opt_square = 1, opt_cube = 1;
    setsockopt(server_fd_square, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_square, sizeof(opt_square));
    setsockopt(server_fd_cube, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_cube, sizeof(opt_cube));

    // Define server addresses
    struct sockaddr_in address_square, address_cube;
    socklen_t addrlen_square = sizeof(address_square);
    socklen_t addrlen_cube = sizeof(address_cube);
    address_square.sin_family = AF_INET;
    address_cube.sin_family = AF_INET;
    address_square.sin_addr.s_addr = INADDR_ANY;
    address_cube.sin_addr.s_addr = INADDR_ANY;
    address_square.sin_port = htons(PORT_SQUARE);
    address_cube.sin_port = htons(PORT_CUBE);

    // Bind the sockets to the addresses
    bind(server_fd_square, (struct sockaddr *)&address_square, sizeof(address_square));
    bind(server_fd_cube, (struct sockaddr *)&address_cube, sizeof(address_cube));

    // Listen for incoming connections (waiting connections may queue up to number of "MAX_CONNECTIONS")
    // However, the limiting may not work as expected for some environments
    listen(server_fd_square, MAX_CONNECTIONS);
    listen(server_fd_cube, MAX_CONNECTIONS);

    // Server setup message
    printf("(inetd) Deamon has started\n(inetd) Waiting for requests coming to ports 5010 & 5020\n");

    // Install SIGCHLD handler
    signal(SIGCHLD, sigchld_handler);

    // Accept and handle incoming connections
    while (1)
    {
        // Set of file descriptors to monitor
        fd_set readfds;

        // Clear the socket set
        FD_ZERO(&readfds);

        // Add both sockets to the set
        FD_SET(server_fd_square, &readfds);
        FD_SET(server_fd_cube, &readfds);

        // Wait for activity on any of the sockets
        int activity = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
        printf("activity is: %d\n", activity);

        // If there is activity on the square socket
        if (FD_ISSET(server_fd_square, &readfds))
        {
            printf("request came to 5010\n");
            // int socket_id = accept(server_fd_square, (struct sockaddr *)&address_square, &addrlen_square);
            // pid_t pid = fork();

            // if (pid == 0)
            // {
            //     dup2(socket_id, STDIN_FILENO);
            //     dup2(socket_id, STDOUT_FILENO);
            //     close(socket_id);
            //     execl("square", "square", NULL);
            // }
            // close(socket_id);
        }

        // If there is activity on the cube socket
        if (FD_ISSET(server_fd_cube, &readfds))
        {
            printf("request came to 5020\n");
            // int socket_id = accept(server_fd_cube, (struct sockaddr *)&address_cube, &addrlen_cube);
            // pid_t pid = fork();

            // if (pid == 0)
            // {
            //     dup2(socket_id, STDIN_FILENO);
            //     dup2(socket_id, STDOUT_FILENO);
            //     close(socket_id);
            //     execl("cube", "cube", NULL);
            // }
            // close(socket_id);
        }
    }

    exit(0);
}

// Function to handle requests
// void handle_request(int client_socket, const char *program)
// {
//     pid_t pid = fork();

//     if (pid == 0)
//     {
//         close(server);
//         dup2(client, STDIN_FILENO);
//         dup2(client, STDOUT_FILENO);
//         dup2(client, STDERR_FILENO);
//         execvp(cmd, argv + 2);
//     }
//     close(client);
// }

// Signal handler for SIGCHLD
void sigchld_handler(int signo)
{
    // Reap all child processes to prevent them from becoming zombies
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}
