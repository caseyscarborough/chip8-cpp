cmake_minimum_required(VERSION 3.22.1)
project(chip8)

# Set variables
set(CMAKE_CXX_STANDARD 17)
set(SOURCES src/main.cpp src/chip8.cpp src/platform.cpp)

# Add SDL2 Cmake Module
set(CMAKE_PREFIX_PATH cmake/sdl2)

# Setup executable ./chip8
add_executable(${PROJECT_NAME} ${SOURCES})

# Show all warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

# Find and link SDL2
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})