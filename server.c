#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int connection_count = 0;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 128) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[*] Server is listening on port %d (FD: %d)\n", PORT, server_fd);
    printf("[*] Current process PID: %d\n", getpid());
    printf("[*] Waiting for connections...\n\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        if (client_fd < 0) {
            printf("[!] Accept failed! Error code: %d (%s)\n", errno, strerror(errno));
            if (errno == EMFILE) {
                printf("[CRITICAL] EMFILE triggered: Process reached its maximum open files limit!\n");
            }
            sleep(5);
            continue;
        }

        connection_count++;
        printf("[+] Client connected! Total: %d | Client FD: %d\n", connection_count, client_fd);
    }

    close(server_fd);
    return 0;
}
