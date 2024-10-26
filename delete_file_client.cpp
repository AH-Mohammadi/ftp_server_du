#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

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
        std::cerr << "Invalid address/Address not supported\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    const char* command = "DELETE_FILE";
    send(sock, command, strlen(command), 0);

    std::string filename;
    std::cout << "Enter the filename to delete: ";
    std::getline(std::cin, filename);

    send(sock, filename.c_str(), filename.length(), 0);

    char buffer[1024] = {0};
    int bytes_read = read(sock, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        std::cout << "Server response: " << buffer << "\n";
    } else {
        std::cerr << "Failed to receive response\n";
    }

    close(sock);
    return 0;
}
