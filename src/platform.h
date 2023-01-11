#pragma once

#include <cstdint>
#include <SDL2/SDL.h>
#include "chip8.h"

class Platform
{
public:
    Platform(char const* title, int windowWidth, int windowHeight);
    ~Platform();
    void update(const uint32_t buffer[]);
    bool process_input(uint8_t* keys);
private:
    uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT]{};
    SDL_Texture* texture{};
    SDL_Renderer* renderer{};
    SDL_Window* window{};
};