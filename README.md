# Chip-8 Emulator

An implementation of a Chip-8 Emulator in C++

![](https://i.imgur.com/GnFvkMD.png)

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
git clone --recurse-submodules https://github.com/caseyscarborough/chip8-cpp
cd chip8-cpp
mkdir build && cd build
cmake .. && make
```

## Running

```bash
# Usage
./chip8 <scale> <delay> <rom>

# Example
./chip8 20 3 ../roms/Tetris.ch8
```

## Playing Games

Chip-8 has a 16-key keypad. The following keys used for emulating the keypad:

|   |   |   |   |
|---|---|---|---|
|`1`|`2`|`3`|`4`|
|`Q`|`W`|`E`|`R`|
|`A`|`S`|`D`|`F`|
|`Z`|`X`|`C`|`V`|

Each game is different so try different keys to determine what each one does. Here are some I know off hand:

- Space Invaders
  - `Q` - move left
  - `W` - shoot
  - `E` - move right
- Tetris
  - `Q` - flip piece
  - `W` - move piece left
  - `E` - move piece right
- Pong
  - `1` - move left paddle up
  - `Q` - move left paddle down
  - `4` - move right paddle up
  - `R` - move right paddle down

You can press Escape to quit the emulator.

## ROMs

ROMs can be found in the `roms` directory or [here](https://github.com/kripod/chip8-roms).

## References

- [BUILDING A CHIP-8 EMULATOR [C++]](https://austinmorlan.com/posts/chip8_emulator/)
- [Mastering CHIP-8](https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908)
- [JamesGriffin/Chip-8-Emulator](https://github.com/JamesGriffin/CHIP-8-Emulator)
- [leemorgan/Chip8](https://github.com/leemorgan/Chip8)
