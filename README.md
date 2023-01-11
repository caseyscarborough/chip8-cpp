# Chip-8 Emulator

An implementation of a Chip-8 Emulator in C++

## Requirements

- CMake
- Make
- SDL2

```bash
# Install with homebrew
brew install cmake make sdl2
```

## Building

```bash
mkdir build
cd build
cmake ..
make .
```

## Running

```bash
# Usage
./chip8 <scale> <delay> <rom>

# Example
./chip8 20 1 Tetris.ch8
```

## ROMs

ROMs can be found [here](https://github.com/kripod/chip8-roms).

## References

- [BUILDING A CHIP-8 EMULATOR [C++]](https://austinmorlan.com/posts/chip8_emulator/)
- [Mastering CHIP-8](https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908)
- [JamesGriffin/Chip-8-Emulator](https://github.com/JamesGriffin/CHIP-8-Emulator)
- [leemorgan/Chip8](https://github.com/leemorgan/Chip8)