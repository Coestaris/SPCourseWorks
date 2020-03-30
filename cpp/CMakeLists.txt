cmake_minimum_required(VERSION 3.15)
project(SPCourse)

set(CMAKE_CXX_STANDARD 14)

add_executable(SPCourse Delimeter.h Lexem.h main.cpp Token_and_Delimeter.cpp TokenType.h)

target_link_libraries(SPCourse m)