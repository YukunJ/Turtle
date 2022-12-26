######################################################################################################################
# Setup
######################################################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)    # define the minimum required version of CMake to be used
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # For clang-tidy.
SET(BUILD_SHARED_LIBS OFF)              # We expect external libraries to be linked statically.
SET(CMAKE_CXX_STANDARD 17)              # Compile as C++17. GoogleTest requires at least C++14
SET(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++17 support.

# Define the project meta information
PROJECT(TurtleServer
        VERSION 2023.02
        DESCRIPTION "A C++ High Performance Web HTTP Server ever evolving"
        LANGUAGES C CXX
        )

######################################################################################################################
# Include
######################################################################################################################

# Includes header file path
SET(TURTLE_SERVER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
SET(TURTLE_SERVER_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)

INCLUDE_DIRECTORIES(${TURTLE_SERVER_SRC_INCLUDE_DIR})
INCLUDE_DIRECTORIES(BEFORE src) # This is needed for gtest.

######################################################################################################################
# Build
######################################################################################################################

# Build the buffer
ADD_LIBRARY(buffer src/buffer.cpp)

# Build the net address
ADD_LIBRARY(net_address src/net_address.cpp)

# Build the socket
ADD_LIBRARY(socket src/socket.cpp)
TARGET_LINK_LIBRARIES(socket net_address)

# Build the connection
ADD_LIBRARY(connection src/connection.cpp)
TARGET_LINK_LIBRARIES(connection buffer)

# Build the poller
ADD_LIBRARY(poller src/poller.cpp)

# Build the looper
ADD_LIBRARY(looper src/looper.cpp)

# Build the thread pool
ADD_LIBRARY(thread_pool src/thread_pool.cpp)

# Build the echo server
ADD_EXECUTABLE(echo_server echo_server.cpp)
TARGET_LINK_LIBRARIES(echo_server buffer net_address socket connection poller looper thread_pool)

# Build the echo client
ADD_EXECUTABLE(echo_client echo_client.cpp)
TARGET_LINK_LIBRARIES(echo_client buffer net_address socket connection poller looper thread_pool)

# Build the main driver
ADD_EXECUTABLE(${PROJECT_NAME} turtle_server.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} buffer net_address socket connection poller looper thread_pool)

######################################################################################################################
# Format + Check
######################################################################################################################

SET(FORMAT_STYLE google)

# Runs clang format and updates files in place.
add_custom_target(format
        find ${PROJECT_SOURCE_DIR} -name "*.cpp" -o -name "*.h" | sed 's| |\\ |g'
        |
        xargs clang-format -style=${FORMAT_STYLE} -i
)
