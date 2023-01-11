#include "Chip8.h"
#include "Platform.h"
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int scale = std::stoi(argv[1]);
    int delay = std::stoi(argv[2]);
    char const *rom = argv[3];

    Platform platform("Chip 8 Emulator", VIDEO_WIDTH * scale, VIDEO_HEIGHT * scale);
    Chip8 chip8;

    bool loaded = chip8.load_rom(rom);
    if (!loaded) {
        std::cerr << "ROM not loaded!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Emulation loop
    bool quit = false;
    while (!quit) {
        chip8.cycle();
        quit = platform.process_input(chip8.keypad);
        // If draw occurred, redraw SDL screen
        if (chip8.draw_flag) {
            chip8.draw_flag = false;
            platform.update(chip8.video);
        }

        // Sleep to slow down emulation speed
        std::this_thread::sleep_for(std::chrono::microseconds(delay * 1200));
    }
}
