cmake_minimum_required(VERSION 3.10)

# Project name
project(FTP_Client_Server)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories if needed (currently assumes source files are in the same directory)
include_directories(${CMAKE_SOURCE_DIR})

# Add executable targets
add_executable(ftp_server ftp_server.cpp)
add_executable(upload_download_client upload_download_client.cpp)
add_executable(list_files_client list_files_client.cpp)
add_executable(find_file_client find_file_client.cpp)

# Add libraries if necessary (for example, linking to pthread for multithreading)
# target_link_libraries(ftp_server pthread)
# target_link_libraries(upload_download_client pthread)
# target_link_libraries(list_files_client pthread)
# target_link_libraries(find_file_client pthread)
