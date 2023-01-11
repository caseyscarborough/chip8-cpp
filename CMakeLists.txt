cmake_minimum_required(VERSION 3.24)
project(chip8)

set(CMAKE_CXX_STANDARD 17)

include_directories(/opt/homebrew/include)
add_subdirectory(thirdparty/glad/)
add_subdirectory(thirdparty/imgui-cmake/)

find_library(GLFW_LIBRARY glfw)
find_library(SDL2_LIBRARY sdl2)
message([STATUS] "SDL2_INCLUDE_DIR" ${SDL2_INCLUDE_DIR})
message([STATUS] "SDL2_LIBRARY" ${SDL2_LIBRARY})
message([STATUS] "GLFW_LIBRARY" ${GLFW_LIBRARY})

add_executable(chip8 main.cpp Chip8.h Chip8.cpp Platform.h Platform.cpp)

target_compile_options(chip8 PRIVATE -Wall)
target_link_libraries(chip8 PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit" ${SDL2_LIBRARY} ${GLFW_LIBRARY} glad imgui)