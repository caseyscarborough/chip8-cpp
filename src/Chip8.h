#pragma once

#include <cstdint>
#include <random>

const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const int VIDEO_HEIGHT = 32;
const int VIDEO_WIDTH = 64;

class Chip8 {
public:
    Chip8();
    bool LoadRom(char const* filename);
    void Cycle();

    bool drawFlag;
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};
    uint8_t keypad[KEY_COUNT]{};
private:
    uint8_t registers[REGISTER_COUNT]{}; // 16 8-bit registers
    uint8_t memory[MEMORY_SIZE]{};       // 4K bytes of memory
    uint16_t index{};                    // 16-bit index register
    uint16_t pc{};                       // 16-bit program counter
    uint16_t stack[STACK_LEVELS]{};      // 16-level stack (can hold 16 program counters)
    uint8_t sp{};                        // 8-bit stack pointer
    uint8_t delayTimer{};                // 8-bit delay timer
    uint8_t soundTimer{};                // 8-bit sound timer
    // 16 input keys 0-F
    // 64x32 monochrome display memory
    uint16_t opcode;                     // 16-bit current instruction

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    //region Instructions

    void op_00E0(); // CLS - clears the display
    void op_00EE(); // RET - return from a subroutine
    void op_1nnn(); // JP addr - jump to location nnn
    void op_2nnn(); // CALL addr - call subroutine at nnn
    void op_3xkk(); // SE Vx, byte - skip next instruction if Vx = kk
    void op_4xkk(); // SNE Vx, byte - skip next instruction if Vx != kk
    void op_5xy0(); // SE Vx, Vy - skip next instruction if Vx = Vy
    void op_6xkk(); // LB Vx, byte - set Vx = kk
    void op_7xkk(); // ADD Vx, byte - set Vx = Vx + kk
    void op_8xy0(); // LD Vx, Vy - set Vx = Vy
    void op_8xy1(); // OR Vx, Vy - set Vx = Vx OR Vy
    void op_8xy2(); // AND Vx, Vy - set Vx = Vx AND Vy
    void op_8xy3(); // XOR Vx, Vy - set Vx = Vx XOR Vy
    void op_8xy4(); // ADD Vx, Vy - set Vx = Vx + Vy, set VF = carry
    void op_8xy5(); // SUB Vx, Vy - set Vx = Vx - Vy, set VF = NOT borrow
    void op_8xy6(); // SHR Vx - set Vx = Vx SHR 1
    void op_8xy7(); // SUBN Vx, Vy - set Vx = Vy - Vx, set VF = NOT borrow
    void op_8xyE(); // SHL Vx {, Vy} - set Vx = Vx SHL 1
    void op_9xy0(); // SNE Vx, Vy - skip next instruction if Vx != Vy
    void op_Annn(); // LD I, addr - set I = nnn
    void op_Bnnn(); // JP V0, addr - jump to location nnn + V0
    void op_Cxkk(); // RND Vx, byte - set Vx = random byte AND kk
    void op_Dxyn(); // DRW Vx, Vy, nibble - display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    void op_Ex9E(); // SKP Vx - skip next instruction if key with the value of Vx is pressed
    void op_ExA1(); // SKNP Vx - skip next instruction if key with the value of Vx is not pressed
    void op_Fx07(); // LD Vx, DT - set Vx = delay timer value
    void op_Fx0A(); // LD Vx, k - wait for a key press, store the value of the key in Vx
    void op_Fx15(); // LD DT, Vx - set delay timer = Vx
    void op_Fx18(); // LD ST, Vx - set sound timer = Vx
    void op_Fx1E(); // ADD I, Vx - set I = I + Vx
    void op_Fx29(); // LD F, Vx - set I = location of sprite for digit Vx
    void op_Fx33(); // LD B, Vx - store BCD representation of Vx in memory locations I, I+1, and I+2
    void op_Fx55(); // LD [I], Vx - store registers V0 through Vx in memory starting at location I
    void op_Fx65(); // LD Vx, [I] - read registers V0 through Vx from memory starting at location I
    void op_null(); // does nothing

    //endregion
};