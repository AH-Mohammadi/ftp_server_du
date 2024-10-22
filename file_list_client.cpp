#include <iostream>
#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8081

void handleFileList(int new_socket) {
    const char* directoryPath = "."; // Specify the directory path
    DIR* dir;
    struct dirent* ent;

    // Open the directory
    if ((dir = opendir(directoryPath)) != nullptr) {
        std::string fileList;
        
        // Read the directory entries
        while ((ent = readdir(dir)) != nullptr) {
            // Ignore the current and parent directory entries
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                fileList += ent->d_name;
                fileList += "\n"; // Add newline for separation
            }
        }
        
        closedir(dir);

        // Send the file list to the client
        send(new_socket, fileList.c_str(), fileList.size(), 0);
        std::cout << "Sent file list to client:\n" << fileList;
    } else {
        std::cerr << "Could not open directory: " << directoryPath << "\n";
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }

    // Forcefully attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "setsockopt error\n";
        return -1;
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed\n";
        return -1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed\n";
        return -1;
    }

    while (true) {
        std::cout << "Waiting for a connection...\n";
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed\n";
            return -1;
        }

        char command[1024] = {0};
        read(new_socket, command, 1024);  // Read the command (e.g., "GET_FILE_LIST")

        if (strcmp(command, "GET_FILE_LIST") == 0) {
            handleFileList(new_socket);  // Handle file list request
        }

        close(new_socket);  // Close the client connection after each operation
    }

    return 0;
}
