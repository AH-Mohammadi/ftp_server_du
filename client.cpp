#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

void uploadFile(int sock, const char* filename) {
    send(sock, "UPLOAD", strlen("UPLOAD"), 0);
    send(sock, filename, strlen(filename), 0);

    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "File not found\n";
        return;
    }

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = infile.readsome(file_buffer, sizeof(file_buffer))) > 0) {
        send(sock, file_buffer, bytes, 0);
    }
    std::cout << "File uploaded successfully\n";
    infile.close();
}

void downloadFile(int sock, const char* filename) {
    send(sock, "DOWNLOAD", strlen("DOWNLOAD"), 0);
    send(sock, filename, strlen(filename), 0);

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to create file\n";
        return;
    }

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = read(sock, file_buffer, 1024)) > 0) {
        outfile.write(file_buffer, bytes);
    }
    std::cout << "File downloaded successfully\n";
    outfile.close();
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    // Choose an operation here
    uploadFile(sock, "example.txt");     // Upload a file to server
    downloadFile(sock, "example.txt");   // Download the same file from server

    close(sock);
    return 0;
}
