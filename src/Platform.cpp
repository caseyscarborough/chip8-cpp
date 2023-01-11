#include "platform.h"
#include "SDL2/SDL.h"
#include "chip8.h"
#include <iostream>

// Keypad keymap
uint8_t keymap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};

Platform::Platform(char const *title, int windowWidth, int windowHeight) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not be initialized!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            windowWidth, windowHeight,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);

    // Create texture that stores frame buffer
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                VIDEO_WIDTH, VIDEO_HEIGHT);

}

Platform::~Platform() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::update(const uint32_t buffer[]) {
    // Store pixels in temporary buffer
    for (int i = 0; i < VIDEO_HEIGHT * VIDEO_WIDTH; ++i) {
        uint8_t pixel = buffer[i];
        pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
    }
    SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::process_input(uint8_t *keys) {
    bool quit = false;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
            std::exit(EXIT_SUCCESS);
        }

        for (int i = 0; i < 16; ++i) {
            if (e.key.keysym.sym == keymap[i]) {
                if (e.type == SDL_KEYDOWN) {
                    keys[i] = 1;
                } else if (e.type == SDL_KEYUP) {
                    keys[i] = 0;
                }
            }
        }
    }
    return quit;
}