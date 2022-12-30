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
# Search Path Specification
######################################################################################################################

# Formatting utility search path
set(TURTLE_SERVER_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(TURTLE_SERVER_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@8/bin" "/usr/local/Cellar/llvm/8.0.1/bin")

# clang-format
IF (NOT DEFINED CLANG_FORMAT_BIN)
    # attempt to find the binary if user did not specify
    FIND_PROGRAM(CLANG_FORMAT_BIN
            NAMES clang-format clang-format-8
            HINTS ${TURTLE_SERVER_CLANG_SEARCH_PATH})
ENDIF ()
IF ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    MESSAGE(WARNING "TurtleServer/main couldn't find clang-format.")
ELSE ()
    MESSAGE(STATUS "TurtleServer/main found clang-format at ${CLANG_FORMAT_BIN}")
ENDIF ()

# cpplint
FIND_PROGRAM(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS ${TURTLE_SERVER_BUILD_SUPPORT_DIR})
IF ("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
    MESSAGE(WARNING "TurtleServer/main couldn't find cpplint.")
ELSE ()
    MESSAGE(STATUS "TurtleServer/main found cpplint at ${CPPLINT_BIN}")
ENDIF ()

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

# Build the acceptor
ADD_LIBRARY(acceptor src/acceptor.cpp)

# Build the http utils
ADD_LIBRARY(http_utils src/http/http_utils.cpp)

# Build the http header
ADD_LIBRARY(header src/http/header.cpp)

# Build the http request
ADD_LIBRARY(request src/http/request.cpp)

# Build the http tester
ADD_EXECUTABLE(http_tester src/http/http_tester.cpp)
TARGET_LINK_LIBRARIES(http_tester request header http_utils)

# Build the echo server
ADD_EXECUTABLE(echo_server demo/echo_server.cpp)
TARGET_LINK_LIBRARIES(echo_server buffer net_address socket connection poller looper acceptor thread_pool)

# Build the echo client
ADD_EXECUTABLE(echo_client demo/echo_client.cpp)
TARGET_LINK_LIBRARIES(echo_client buffer net_address socket connection poller looper acceptor thread_pool)

######################################################################################################################
# Format + Check
######################################################################################################################

SET(FORMAT_STYLE google)

# "make format"
add_custom_target(format
        find ${PROJECT_SOURCE_DIR} -name "*.cpp" -o -name "*.h" | sed 's| |\\ |g'
        |
        xargs clang-format -style=${FORMAT_STYLE} -i
)


# "make cpplint"
file(GLOB_RECURSE TURTLE_SERVER_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.cpp"
        )

add_custom_target(cpplint echo '${TURTLE_SERVER_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/include_subdir,-readability/casting
        )