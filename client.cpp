#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

void uploadFile(int sock, const char* filename) {
    // Send upload request
    send(sock, "UPLOAD", strlen("UPLOAD"), 0);

    // Send the filename to the server
    send(sock, filename, strlen(filename), 0);

    // Open the file to be uploaded
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    // Read the file and send it to the server
    char file_buffer[1024];
    while (!infile.eof()) {
        infile.read(file_buffer, sizeof(file_buffer));
        int bytes_read = infile.gcount();
        send(sock, file_buffer, bytes_read, 0);
    }

    std::cout << "File uploaded successfully as " << filename << "\n";
    infile.close();
}

void downloadFile(int sock, const char* filename) {
    // Send download request
    send(sock, "DOWNLOAD", strlen("DOWNLOAD"), 0);

    // Send the filename to download
