#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 500) < 0) {
        perror("Error listening on socket");
        close(server_fd);
        return 1;
    }

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
        if (client_fd < 0) {
            perror("Error accepting connection");
            continue;
        }
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received < 0) {
            perror("Error receiving data");
            close(client_fd);
            continue;
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
            close(client_fd);
            continue;
        }

        // Null-terminate the received data to safely print as string
        buffer[bytes_received] = '\0';

        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
        send(client_fd, response, strlen(response), 0);

        close(client_fd);
    }

    // Close the server socket (never reached due to infinite loop)
    close(server_fd);

    return 0;
}
