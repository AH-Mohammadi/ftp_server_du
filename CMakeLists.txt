# Minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Project Name
project(FTPServerClient)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Add executables for the server and client
add_executable(server server.cpp)
add_executable(client client.cpp)
