######################################################################################################################
# Setup
######################################################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.11)    # define the minimum required version of CMake to be used
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
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    ADD_DEFINITIONS(-DOS_MAC)
ELSE()
    MESSAGE(FATAL_ERROR "Your operating system ${CMAKE_SYSTEM_NAME} is not supported.")
ENDIF()

# Use Logger or not
IF (${LOG_LEVEL} MATCHES "NOLOG")
    MESSAGE("Build in ${LOG_LEVEL} mode without Logging")
    ADD_DEFINITIONS(-DNOLOG)
ELSE()
    MESSAGE("Build with Logging enabled")
ENDIF()

# Use Timer or not
IF (DEFINED TIMER)
    MESSAGE("Build using timer of expiration ${TIMER}")
    ADD_DEFINITIONS(-DTIMER_EXPIRATION=${TIMER})
ELSE()
    MESSAGE("Build without timer")
    ADD_DEFINITIONS(-DTIMER_EXPIRATION=0)
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
            NAMES clang-format clang-format-14
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

# clang-tidy
IF (NOT DEFINED CLANG_TIDY_BIN)
FIND_PROGRAM(CLANG_TIDY_BIN
        NAMES clang-tidy clang-tidy-14
        HINTS ${TURTLE_SERVER_CLANG_SEARCH_PATH})
ENDIF ()
IF ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    MESSAGE(WARNING "TurtleServer/main couldn't find clang-tidy.")
ELSE ()
    SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    MESSAGE(STATUS "TurtleServer/main found clang-tidy at ${CLANG_TIDY_BIN}")
ENDIF ()

######################################################################################################################
# Include
######################################################################################################################

# Includes header file path
SET(TURTLE_SERVER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
SET(TURTLE_SERVER_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
SET(TURTLE_MYSQL_INCLUDE_DIR "/usr/include/cppconn") # Linux only, might not be portable
SET(TURTLE_SERVER_TEST_DIR ${PROJECT_SOURCE_DIR}/test)
SET(TURTLE_SERVER_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
SET(TURTLE_SERVER_DEMO_DIR ${PROJECT_SOURCE_DIR}/demo)

######################################################################################################################
# Build
######################################################################################################################
SET(CMAKE_COMPILER_FLAG -O3 -Wall -Wextra -pedantic -Werror)
SET(MYSQL_COMPILER_FLAG -I/usr/include/cppconn -L/usr/lib -lmysqlcppconn)

# Build the turtle core library
FILE(GLOB TURTLE_CORE_SOURCE RELATIVE ${CMAKE_SOURCE_DIR} "src/core/*.cpp")
ADD_LIBRARY(turtle_core ${TURTLE_CORE_SOURCE})
TARGET_LINK_LIBRARIES(turtle_core turtle_log Threads::Threads)
TARGET_COMPILE_OPTIONS(turtle_core PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        turtle_core
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
)

# Build the turtle http library
FILE(GLOB TURTLE_HTTP_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/http/*.cpp")
ADD_LIBRARY(turtle_http ${TURTLE_HTTP_SOURCES})
TARGET_LINK_LIBRARIES(turtle_http turtle_log)
TARGET_COMPILE_OPTIONS(turtle_http PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        turtle_http
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
)

# Build the turtle db library
FILE(GLOB TURTLE_DB_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/db/*.cpp")
ADD_LIBRARY(turtle_db ${TURTLE_DB_SOURCES})
TARGET_LINK_LIBRARIES(turtle_db turtle_log mysqlcppconn)
TARGET_COMPILE_OPTIONS(turtle_db PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        turtle_db
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
        PUBLIC ${TURTLE_MYSQL_INCLUDE_DIR}
)

# Build the turtle logging library
FILE(GLOB TURTLE_LOG_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/log/*.cpp")
ADD_LIBRARY(turtle_log ${TURTLE_LOG_SOURCES})
TARGET_LINK_LIBRARIES(turtle_db Threads::Threads)
TARGET_COMPILE_OPTIONS(turtle_log PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        turtle_log
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
)

# Build the echo server
ADD_EXECUTABLE(echo_server ${TURTLE_SERVER_DEMO_DIR}/echo/echo_server.cpp)
TARGET_LINK_LIBRARIES(echo_server turtle_core)
TARGET_COMPILE_OPTIONS(echo_server PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        echo_server
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
)

# Build the echo client
ADD_EXECUTABLE(echo_client ${TURTLE_SERVER_DEMO_DIR}/echo/echo_client.cpp)
TARGET_LINK_LIBRARIES(echo_client turtle_core)
TARGET_INCLUDE_DIRECTORIES(
        echo_client
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
)

# Build the kvstore dict server
ADD_EXECUTABLE(dict_server ${TURTLE_SERVER_DEMO_DIR}/kvstore/dict_server.cpp)
TARGET_LINK_LIBRARIES(dict_server turtle_core turtle_http)
TARGET_COMPILE_OPTIONS(dict_server PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        dict_server
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
)

# Build the kvstore dict client
ADD_EXECUTABLE(dict_client ${TURTLE_SERVER_DEMO_DIR}/kvstore/dict_client.cpp)
TARGET_LINK_LIBRARIES(dict_client turtle_core turtle_http)
TARGET_INCLUDE_DIRECTORIES(
        dict_client
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
)

# Build the http server
ADD_EXECUTABLE(http_server ${TURTLE_SERVER_SRC_DIR}/http/http_server.cpp)
TARGET_LINK_LIBRARIES(http_server turtle_core turtle_http)
TARGET_COMPILE_OPTIONS(http_server PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        http_server
        PUBLIC ${TURTLE_SERVER_SRC_INCLUDE_DIR}
)
######################################################################################################################
# Test (Catch2)
######################################################################################################################

# fetch the Catch2 from github
INCLUDE(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.3.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

# build and link individual tests with Catch2
ADD_EXECUTABLE(buffer_test ${TURTLE_SERVER_TEST_DIR}/core/buffer_test.cpp)
TARGET_LINK_LIBRARIES(buffer_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(cache_test ${TURTLE_SERVER_TEST_DIR}/core/cache_test.cpp)
TARGET_LINK_LIBRARIES(cache_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(timer_test ${TURTLE_SERVER_TEST_DIR}/core/timer_test.cpp)
TARGET_LINK_LIBRARIES(timer_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(net_address_test ${TURTLE_SERVER_TEST_DIR}/core/net_address_test.cpp)
TARGET_LINK_LIBRARIES(net_address_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(socket_test ${TURTLE_SERVER_TEST_DIR}/core/socket_test.cpp)
TARGET_LINK_LIBRARIES(socket_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(connection_test ${TURTLE_SERVER_TEST_DIR}/core/connection_test.cpp)
TARGET_LINK_LIBRARIES(connection_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(poller_test ${TURTLE_SERVER_TEST_DIR}/core/poller_test.cpp)
TARGET_LINK_LIBRARIES(poller_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(looper_test ${TURTLE_SERVER_TEST_DIR}/core/looper_test.cpp)
TARGET_LINK_LIBRARIES(looper_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(acceptor_test ${TURTLE_SERVER_TEST_DIR}/core/acceptor_test.cpp)
TARGET_LINK_LIBRARIES(acceptor_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(thread_pool_test ${TURTLE_SERVER_TEST_DIR}/core/thread_pool_test.cpp)
TARGET_LINK_LIBRARIES(thread_pool_test PRIVATE Catch2::Catch2WithMain turtle_core)

ADD_EXECUTABLE(header_test ${TURTLE_SERVER_TEST_DIR}/http/header_test.cpp)
TARGET_LINK_LIBRARIES(header_test PRIVATE Catch2::Catch2WithMain turtle_core turtle_http)

ADD_EXECUTABLE(request_test ${TURTLE_SERVER_TEST_DIR}/http/request_test.cpp)
TARGET_LINK_LIBRARIES(request_test PRIVATE Catch2::Catch2WithMain turtle_core turtle_http)

ADD_EXECUTABLE(response_test ${TURTLE_SERVER_TEST_DIR}/http/response_test.cpp)
TARGET_LINK_LIBRARIES(response_test PRIVATE Catch2::Catch2WithMain turtle_core turtle_http)

ADD_EXECUTABLE(cgier_test ${TURTLE_SERVER_TEST_DIR}/http/cgier_test.cpp)
TARGET_LINK_LIBRARIES(cgier_test PRIVATE Catch2::Catch2WithMain turtle_core turtle_http)

ADD_EXECUTABLE(mysqler_test ${TURTLE_SERVER_TEST_DIR}/db/mysqler_test.cpp)
TARGET_LINK_LIBRARIES(mysqler_test PRIVATE Catch2::Catch2WithMain turtle_db)

# helper program to test cgi module, used the `cgi_test`
ADD_EXECUTABLE(add ${CMAKE_SOURCE_DIR}/http_dir/cgi-bin/add.c)
ADD_EXECUTABLE(helloworld ${CMAKE_SOURCE_DIR}/http_dir/cgi-bin/helloworld.c)

# automatic discover and run all the tests with 'ctest' or 'make test'
LIST(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
INCLUDE(CTest)
INCLUDE(Catch)

# Core Module
CATCH_DISCOVER_TESTS(buffer_test)
CATCH_DISCOVER_TESTS(cache_test)
CATCH_DISCOVER_TESTS(timer_test)
CATCH_DISCOVER_TESTS(net_address_test)
CATCH_DISCOVER_TESTS(socket_test)
CATCH_DISCOVER_TESTS(connection_test)
CATCH_DISCOVER_TESTS(poller_test)
CATCH_DISCOVER_TESTS(looper_test)
CATCH_DISCOVER_TESTS(acceptor_test)
CATCH_DISCOVER_TESTS(thread_pool_test)

# HTTP Module
CATCH_DISCOVER_TESTS(header_test)
CATCH_DISCOVER_TESTS(request_test)
CATCH_DISCOVER_TESTS(response_test)
CATCH_DISCOVER_TESTS(cgier_test)

# DB Module
CATCH_DISCOVER_TESTS(mysqler_test)

######################################################################################################################
# Format + Lint Check + Statistics Count
######################################################################################################################

SET(FORMAT_STYLE google)

string(CONCAT TURTLR_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        "${CMAKE_CURRENT_SOURCE_DIR}/demo,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
        )

# "make format"
ADD_CUSTOM_TARGET(format ${TURTLE_SERVER_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${TURTLE_SERVER_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${TURTLR_FORMAT_DIRS}
        --fix
        --quiet
)

# "make cpplint"
FILE(GLOB_RECURSE TURTLE_SERVER_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
        )

ADD_CUSTOM_TARGET(cpplint
        echo '${TURTLE_SERVER_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/include_subdir,-readability/casting,-readability/check
        )

# "make tidy"
ADD_CUSTOM_TARGET(tidy ${TURTLE_SERVER_BUILD_SUPPORT_DIR}/run_clang_tidy.py
        -clang-tidy-binary ${CLANG_TIDY_BIN}
        ${TURTLE_SERVER_SRC_DIR}
        -p ${CMAKE_BINARY_DIR}
)

# "make linecount"
ADD_CUSTOM_TARGET(linecount
        cloc --by-file ${TURTLE_SERVER_SRC_DIR}
        )



# 'make benchmark'
# will run http_server
SET(WEBBENCH_DIR ${PROJECT_SOURCE_DIR}/webbench)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(CONCURRENCY 10500)  # concurrent client number
    SET(DURATION 5)  # stress test duration
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(CONCURRENCY 105)
    SET(DURATION 10)
ENDIF()

ADD_CUSTOM_TARGET(benchmark
        WORKING_DIRECTORY ${WEBBENCH_DIR}
        COMMAND sh ./benchmark.sh ${CONCURRENCY} ${DURATION}
        DEPENDS http_server
        )