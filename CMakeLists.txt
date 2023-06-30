cmake_minimum_required(VERSION 3.12)
project(gta5devmode)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES main.cpp memory/process.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(${PROJECT_NAME} PRIVATE -static)
endif()
