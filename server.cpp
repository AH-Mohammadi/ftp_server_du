#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

void handleUpload(int new_socket) {
    char filename[1024] = {0};
    read(new_socket, filename, 1024);
    std::cout << "Receiving file: " << filename << std::endl;

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to create file\n";
        return;
    }

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = read(new_socket, file_buffer, 1024)) > 0) {
        outfile.write(file_buffer, bytes);
    }
    std::cout << "File uploaded successfully\n";
    outfile.close();
}

void handleDownload(int new_socket) {
    char filename[1024] = {0};
    read(new_socket, filename, 1024);
    std::cout << "Sending file: " << filename << std::endl;

    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "File not found\n";
        return;
    }

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = infile.readsome(file_buffer, sizeof(file_buffer))) > 0) {
        send(new_socket, file_buffer, bytes, 0);
    }
    std::cout << "File sent successfully\n";
    infile.close();
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        std::cout << "Waiting for connection...\n";
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        char operation[1024] = {0};
        read(new_socket, operation, 1024);

        if (strcmp(operation, "UPLOAD") == 0) {
            handleUpload(new_socket);
        } else if (strcmp(operation, "DOWNLOAD") == 0) {
            handleDownload(new_socket);
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
