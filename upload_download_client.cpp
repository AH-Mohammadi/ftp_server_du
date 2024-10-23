#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

void uploadFile(int sock, const char* filename) {
    send(sock, "UPLOAD", strlen("UPLOAD"), 0);
    send(sock, filename, strlen(filename), 0);

    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error: Could not open file: " << filename << "\n";
        return;
    }

    char file_buffer[1024];
    while (!infile.eof()) {
        infile.read(file_buffer, sizeof(file_buffer));
        int bytes_read = infile.gcount();
        if (bytes_read > 0) {
            send(sock, file_buffer, bytes_read, 0);
        }
    }

    std::cout << "File successfully uploaded: " << filename << "\n";
    infile.close();
}

void downloadFile(int sock, const char* filename) {
    send(sock, "DOWNLOAD", strlen("DOWNLOAD"), 0);
    send(sock, filename, strlen(filename), 0);

    std::string newFilename = "downloaded_" + std::string(filename);

    std::ofstream outfile(newFilename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: Could not create file: " << newFilename << "\n";
        return;
    }

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = read(sock, file_buffer, sizeof(file_buffer))) > 0) {
        outfile.write(file_buffer, bytes);
    }

    std::cout << "File successfully downloaded: " << newFilename << "\n";
    outfile.close();
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error: Socket creation failed\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Error: Invalid address\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error: Connection failed\n";
        return -1;
    }

    uploadFile(sock, "example.txt");

    close(sock);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error: Socket creation failed\n";
        return -1;
    }
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error: Connection failed\n";
        return -1;
    }

    downloadFile(sock, "example.txt");

    close(sock);
    return 0;
}
