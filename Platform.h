#pragma once

#include <cstdint>
#include <SDL2/SDL.h>
#include <glad/gl.h>


class Platform
{
    friend class Imgui;

public:
    Platform(char const* title, int windowWidth, int windowHeight);
    ~Platform();
    void Update(void const* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);

private:
    SDL_Window* window{};
    SDL_GLContext gl_context{};
    GLuint framebuffer_texture;
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
    uint32_t pixels[2048]{};
};