cmake_minimum_required(VERSION 4.3)
project(loglib C)

set(CMAKE_C_STANDARD 11)

include_directories(loglib)

add_executable(loglib
        loglib/src/loglib.c
        loglib/loglib.h
        tests/test_logging.c
        loglib/src/emiter.c
        loglib/src/escaping.c)
