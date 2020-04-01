cmake_minimum_required(VERSION 3.15)
project(RTP C)

set(CMAKE_C_STANDARD 11)

add_executable(RTP main.c events.h events.c rtp.c rtp.h)