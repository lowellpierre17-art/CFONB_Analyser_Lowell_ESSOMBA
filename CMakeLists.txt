cmake_minimum_required(VERSION 4.0.2)
project(CFONB_Analyzer C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
include_directories(include)


add_executable(CFONB_Analyzer main.c
        cfonb_stats.c
        cfonb_validator.c
        cfonb_parser.c
        cfonb_utils.h
        cfonb_stats.h
        cfonb_utils.c)
