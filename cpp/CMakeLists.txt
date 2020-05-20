cmake_minimum_required(VERSION 3.13)
project(SPCourse)

set(CMAKE_CXX_STANDARD 14)

add_executable(SPCourse Delimiter.h Lexem.h main.cpp TokenDelimeter.cpp TokenType.h AssemblyFuncs.cpp Assembly.h ByteFuncs.cpp Bytes.cpp Bytes.h)

target_link_libraries(SPCourse m)