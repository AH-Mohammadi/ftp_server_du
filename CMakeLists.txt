cmake_minimum_required(VERSION 3.10)

# Set the project name
project(FileTransferServer)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable for the file transfer server
add_executable(server server.cpp)

# Add executable for the client
add_executable(client client.cpp)


# Add executable for the file list client
add_executable(file_list_client file_list_client.cpp)
