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
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this to the server's IP
    server_addr.sin_port = htons(8080);

    if (connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[2] = {
        {0, POLLIN, 0},       // stdin
        {socketfd, POLLIN, 0} // socket
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
                send(socketfd, buffer, bytes_read, 0);
            }
        }

        if (fds[1].revents & POLLIN) {
            // Read from the server
            ssize_t bytes_received = recv(socketfd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0'; // Null-terminate the string
                printf("Received from server: %s\n", buffer);
            } else if (bytes_received == 0) {
                // Server has closed the connection
                printf("Server disconnected.\n");
                break;
            } else {
                perror("Receive error");
                break;
            }
        }
    }

    // Cleanup
    close(socketfd);
    return 0;
}
