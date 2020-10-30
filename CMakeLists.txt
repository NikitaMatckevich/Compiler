cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_COMPILER /usr/bin/x86_64-linux-gnu-gcc-9)
set(CMAKE_CXX_COMPILER /usr/bin/x86_64-linux-gnu-g++-9)

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

include_directories(../include)

add_executable(compiler ${SOURCE})

