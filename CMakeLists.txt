cmake_minimum_required(VERSION 3.10)
project(VoltaxDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/database.cpp
    src/server.cpp
)

# Add executable
add_executable(VoltaxDB ${SOURCES})

# Link Boost Asio (required for networking)
find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(VoltaxDB Boost::system pthread)

# Compiler warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(VoltaxDB PRIVATE -Wall -Wextra -Wpedantic)
endif()
