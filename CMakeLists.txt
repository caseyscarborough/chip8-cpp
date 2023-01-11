cmake_minimum_required(VERSION 3.24)
project(chip8)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES src/main.cpp src/chip8.cpp src/platform.cpp)

include_directories(/opt/homebrew/include)

find_library(SDL2_LIBRARY sdl2)

add_executable(chip8 ${SOURCES})

target_compile_options(chip8 PRIVATE -Wall)
target_link_libraries(chip8 PRIVATE ${SDL2_LIBRARY})