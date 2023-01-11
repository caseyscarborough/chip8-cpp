cmake_minimum_required(VERSION 3.22.1)
project(chip8)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES src/main.cpp src/chip8.cpp src/platform.cpp)

set(CMAKE_PREFIX_PATH cmake)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8 ${SOURCES})

target_compile_options(chip8 PRIVATE -Wall)
target_link_libraries(chip8 PRIVATE ${SDL2_LIBRARIES})