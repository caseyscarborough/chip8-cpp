#pragma once

#include <cstdint>
#include <SDL2/SDL.h>
#include "Chip8.h"

class Platform
{
public:
    Platform(char const* title, int windowWidth, int windowHeight);
    ~Platform();
    void Update(const uint32_t buffer[]);
    bool ProcessInput(uint8_t* keys);
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
    uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT]{};
private:
    SDL_Window* window{};
};