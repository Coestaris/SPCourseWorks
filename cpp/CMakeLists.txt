cmake_minimum_required(VERSION 3.13)
project(SPCourse)

set(CMAKE_CXX_STANDARD 14)

add_executable(SPCourse Delimiter.h Lexem.h main.cpp Token_and_Delimeter.cpp TokenType.h Assembly_functions.cpp Assembly.h)

target_link_libraries(SPCourse m)