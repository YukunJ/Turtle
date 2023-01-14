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
        DESCRIPTION "A C++ High Performance Network Web Server Framework"
        LANGUAGES C CXX
        )

# Detect the operating system
MESSAGE("Compiling on the operating system of ${CMAKE_SYSTEM_NAME}")
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    ADD_DEFINITIONS(-DOS_LINUX)
ELSE()
    MESSAGE(WARNING "Your operating system ${CMAKE_SYSTEM_NAME} is not supported.")
ENDIF()

######################################################################################################################
# Search Path Specification
######################################################################################################################

# Find the required thread package on the platform
SET(THREADS_PREFER_PTHREAD_FLAG ON)
FIND_PACKAGE(Threads REQUIRED)

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

# Build the turtle core library
FILE(GLOB TURTLE_CORE_SOURCE RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
ADD_LIBRARY(turtle_core ${TURTLE_CORE_SOURCE})
TARGET_LINK_LIBRARIES(turtle_core Threads::Threads)
TARGET_COMPILE_OPTIONS(turtle_core PRIVATE -Wall -Wextra -pedantic -Werror)

# Build the turtle http library
FILE(GLOB TURTLE_HTTP_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/http/*.cpp")
ADD_LIBRARY(turtle_http ${TURTLE_HTTP_SOURCES})
TARGET_COMPILE_OPTIONS(turtle_http PRIVATE -Wall -Wextra -pedantic -Werror)

# Build the CGI tester
ADD_EXECUTABLE(cgi_tester src/http/cgi_tester.cpp)
TARGET_LINK_LIBRARIES(cgi_tester turtle_http)

# Build the echo server
ADD_EXECUTABLE(echo_server demo/echo_server.cpp)
TARGET_LINK_LIBRARIES(echo_server turtle_core)
TARGET_COMPILE_OPTIONS(echo_server PRIVATE -Wall -Wextra -pedantic -Werror)

# Build the echo client
ADD_EXECUTABLE(echo_client demo/echo_client.cpp)
TARGET_LINK_LIBRARIES(echo_client turtle_core)

# Build the http server
ADD_EXECUTABLE(http_server src/http/http_server.cpp)
TARGET_LINK_LIBRARIES(http_server turtle_core turtle_http)
TARGET_COMPILE_OPTIONS(http_server PRIVATE -Wall -Wextra -pedantic -Werror)


######################################################################################################################
# Format + Lint Check + Statistics Count
######################################################################################################################

SET(FORMAT_STYLE google)

# "make format"
ADD_CUSTOM_TARGET(format
        find ${PROJECT_SOURCE_DIR} -name "*.cpp" -o -name "*.h" | sed 's| |\\ |g'
        |
        xargs clang-format -style=${FORMAT_STYLE} -i
)


# "make cpplint"
FILE(GLOB_RECURSE TURTLE_SERVER_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.cpp"
        )

ADD_CUSTOM_TARGET(cpplint echo '${TURTLE_SERVER_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/include_subdir,-readability/casting
        )

# "make linecount"
ADD_CUSTOM_TARGET(linecount
        cloc --by-file ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

# 'make benchmark'
# will run http_server
SET(WEBBENCH_DIR ${PROJECT_SOURCE_DIR}/webbench)
ADD_CUSTOM_TARGET(benchmark
        WORKING_DIRECTORY ${WEBBENCH_DIR}
        COMMAND sh ./benchmark.sh
        DEPENDS http_server
        )