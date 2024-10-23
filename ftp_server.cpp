#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <dirent.h>

#define PORT 8080

void handleUpload(int new_socket) {
    char filename[1024] = {0};
    read(new_socket, filename, 1024);  // Receive filename from client

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: Could not create file: " << filename << "\n";
        return;
    }

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = read(new_socket, file_buffer, sizeof(file_buffer))) > 0) {
        outfile.write(file_buffer, bytes);
    }

    std::cout << "File uploaded successfully: " << filename << "\n";
    outfile.close();
}

void handleDownload(int new_socket) {
    char filename[1024] = {0};
    read(new_socket, filename, 1024);  // Receive filename from client

    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error: File not found: " << filename << "\n";
        return;
    }

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = infile.readsome(file_buffer, sizeof(file_buffer))) > 0) {
        send(new_socket, file_buffer, bytes, 0);
    }

    std::cout << "File sent successfully: " << filename << "\n";
    infile.close();
}

void handleFileList(int new_socket) {
    const char* directoryPath = ".";
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(directoryPath)) != nullptr) {
        std::string fileList;

        while ((ent = readdir(dir)) != nullptr) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                fileList += ent->d_name;
                fileList += "\n";
            }
        }

        closedir(dir);
        send(new_socket, fileList.c_str(), fileList.size(), 0);
        std::cout << "File list sent to client.\n";
    } else {
        std::cerr << "Error: Could not open directory: " << directoryPath << "\n";
    }
}

void handleSearchFile(int new_socket) {
    char filename[1024] = {0};
    read(new_socket, filename, 1024);

    std::ifstream infile(filename);
    if (infile) {
        const char* response = "File found!";
        send(new_socket, response, strlen(response), 0);
    } else {
        const char* response = "File not found!";
        send(new_socket, response, strlen(response), 0);
    }

    infile.close();
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Socket options setup failed\n";
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Socket bind failed\n";
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Socket listen failed\n";
        return -1;
    }

    while (true) {
        std::cout << "Waiting for a connection...\n";
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed\n";
            return -1;
        }

        char command[1024] = {0};
        read(new_socket, command, 1024);

        if (strcmp(command, "UPLOAD") == 0) {
            handleUpload(new_socket);
        } else if (strcmp(command, "DOWNLOAD") == 0) {
            handleDownload(new_socket);
        } else if (strcmp(command, "GET_FILE_LIST") == 0) {
            handleFileList(new_socket);
        } else if (strcmp(command, "SEARCH_FILE") == 0) {
            handleSearchFile(new_socket);
        }

        close(new_socket);
    }

    return 0;
}
