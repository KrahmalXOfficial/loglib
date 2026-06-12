cmake_minimum_required(VERSION 4.3)
project(loglib C)

include_directories(/usr/include)

set(CMAKE_C_STANDARD 11)
set(SOURCES
    loglib/src/formating.c
    loglib/src/handlers.c
    loglib/src/escaping.c

)

add_library(loglib SHARED ${SOURCES})
target_include_directories(loglib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/loglib/include>
    $<INSTALL_INTERFACE:loglib/include>
)
