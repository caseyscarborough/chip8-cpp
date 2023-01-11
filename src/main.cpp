#include "Chip8.h"
#include "Platform.h"
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const *romFilename = argv[3];

    Platform platform("Chip 8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale);
    Chip8 chip8;

    bool loaded = chip8.LoadRom(romFilename);
    if (!loaded) {
        std::exit(EXIT_FAILURE);
    }

    // Emulation loop
    bool quit = false;
    while (!quit) {
        chip8.Cycle();
        quit = platform.ProcessInput(chip8.keypad);
        // If draw occurred, redraw SDL screen
        if (chip8.drawFlag) {
            chip8.drawFlag = false;
            platform.Update(chip8.video);
        }

        // Sleep to slow down emulation speed
        std::this_thread::sleep_for(std::chrono::microseconds(cycleDelay * 1200));
    }
}
