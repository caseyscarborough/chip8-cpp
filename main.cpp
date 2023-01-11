#include <iostream>
#include "Chip8.h"
#include "Platform.h"
#include <thread>

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

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const *romFilename = argv[3];

    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not be initialized!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow(
            "Chip 8 Emulator",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale,
            SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(2);
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale);

    // Create texture that stores frame buffer
    SDL_Texture *sdlTexture = SDL_CreateTexture(renderer,
                                                SDL_PIXELFORMAT_ARGB8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                64, 32);

    Chip8 chip8;

    // Temporary pixel buffer
    uint32_t pixels[2048];

    load:
    bool loaded = chip8.LoadRom(romFilename);
    if (!loaded) {
        return 2;
    }

    // Emulation loop
    while (true) {
        chip8.Cycle();

        // Process SDL events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) exit(0);

            // Process keydown events
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    exit(0);

                if (e.key.keysym.sym == SDLK_F1)
                    goto load;      // *gasp*, a goto statement!
                // Used to reset/reload ROM

                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.keypad[i] = 1;
                    }
                }
            }
            // Process keyup events
            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.keypad[i] = 0;
                    }
                }
            }
        }

        // If draw occurred, redraw SDL screen
        if (chip8.drawFlag) {
            chip8.drawFlag = false;

            // Store pixels in temporary buffer
            for (int i = 0; i < 2048; ++i) {
                uint8_t pixel = chip8.video[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }
            SDL_UpdateTexture(sdlTexture, nullptr, pixels, 64 * sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }

        // Sleep to slow down emulation speed
        std::this_thread::sleep_for(std::chrono::microseconds(cycleDelay * 1200));

    }
}
