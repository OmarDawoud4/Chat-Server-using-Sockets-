#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

int main() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;      // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    server_addr.sin_port = htons(8080);

    if (bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    if (listen(socketfd, 20) < 0) {
        perror("Listen failed");
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    int clientfd = accept(socketfd, NULL, NULL);
    if (clientfd < 0) {
        perror("Accept failed");
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[2] = {
        {0, POLLIN, 0},       // stdin
        {clientfd, POLLIN, 0} // client socket
    };

    while (1) {
        char buffer[BUFFER_SIZE] = {0};

        int poll_count = poll(fds, 2, 20000);
        if (poll_count < 0) {
            perror("Poll failed");
            break;
        } else if (poll_count == 0) {
            printf("Timeout occurred! No data received in 5 seconds.\n");
            continue;
        }

        if (fds[0].revents & POLLIN) {
            // Read from stdin
            ssize_t bytes_read = read(0, buffer, BUFFER_SIZE - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0'; // Null-terminate the string
                send(clientfd, buffer, bytes_read, 0);
            }
        }

        if (fds[1].revents & POLLIN) {
            // Read from the client
            ssize_t bytes_received = recv(clientfd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0'; // Null-terminate the string
                printf("Received from client: %s\n", buffer);
            } else if (bytes_received == 0) {
                // Client has closed the connection
                printf("Client disconnected.\n");
                break;
            } else {
                perror("Receive error");
                break;
            }
        }
    }

    // Cleanup
    close(clientfd);
    close(socketfd);
    return 0;
}
