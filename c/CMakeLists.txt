cmake_minimum_required(VERSION 3.15)
project(c C)

set(CMAKE_C_STANDARD 11)

add_executable(c main.c tokenizer.c tokenizer.h list.c list.h)