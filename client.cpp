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
        if (bytes_read > 0) {
            send(sock, file_buffer, bytes_read, 0);
        }
    }

    std::cout << "File uploaded successfully as " << filename << "\n";
    infile.close();
}

void downloadFile(int sock, const char* filename) {
    // Send download request
    send(sock, "DOWNLOAD", strlen("DOWNLOAD"), 0);

    // Send the filename to download
    send(sock, filename, strlen(filename), 0);

    // Prepare the new filename for download (with a different name)
    std::string newFilename = "downloaded_" + std::string(filename);

    // Open a file to save the downloaded content
    std::ofstream outfile(newFilename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to create file: " << newFilename << "\n";
        return;
    }

    // Receive data from the server and write it to the file
    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = read(sock, file_buffer, sizeof(file_buffer))) > 0) {
        outfile.write(file_buffer, bytes);
    }

    std::cout << "File downloaded successfully as " << newFilename << "\n";
    outfile.close();
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }

    // Configure the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported\n";
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    // Upload a file
    uploadFile(sock, "example.txt");

    // Reconnect for download (because the upload closes the socket)
    close(sock);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    // Download the file (it will be saved as downloaded_example.txt)
    downloadFile(sock, "example.txt");

    // Close the socket
    close(sock);

    return 0;
}
