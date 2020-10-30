cmake_minimum_required(VERSION 3.2)

project(Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Needed for vim code completion plugin

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -DDEBUG -g -Wall -Wextra -pedantic -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(SOURCE
    ../src/Main.cpp
    ../src/Errors.cpp
    ../src/Lexer.cpp
    ../src/Ast.cpp
    ../src/Visitors.cpp
)

include_directories(include)

message(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")

if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
    message(STATUS "clang-tidy on")
else()
    unset(CMAKE_CXX_CLANG_TIDY)
    message(STATUS "clang-tidy off")
endif()

add_executable(compiler ${SOURCE})

