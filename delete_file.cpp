#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>

void handleDeleteFile(int new_socket) {
    char filename[1024] = {0};
    read(new_socket, filename, 1024);  // Receive filename from client

    if (remove(filename) == 0) {
        const char* response = "File deleted successfully!";
        send(new_socket, response, strlen(response), 0);
        std::cout << "Deleted file: " << filename << "\n";
    } else {
        const char* response = "File deletion failed!";
        send(new_socket, response, strlen(response), 0);
        std::cerr << "Failed to delete file: " << filename << "\n";
    }
}
