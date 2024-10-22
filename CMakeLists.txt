cmake_minimum_required(VERSION 3.10)

# Set the project name
project(FileTransferServer)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories (if any)
# include_directories(include)

# Source files for the upload/download server
add_executable(file_transfer_server file_transfer_server.cpp)

# Source files for the file list server
add_executable(file_list_server file_list_server.cpp)
