#include "Chip8.h"
#include <iostream>
#include <chrono>
#include <fstream>

// The Chip8’s memory from 0x000 to 0x1FF is reserved
// so the ROM instructions must start at 0x200.
const unsigned int START_ADDRESS = 0x200;

// There are 16 different (0-F) 5-byte fonts.
const unsigned int FONTSET_SIZE = 80;

// Each character sprite is 5 bytes, and each bit represents a pixel.
// Each bit represents a pixel, where 1 is on, and 0 is off.
// For example, the character F is 0xF0, 0x80, 0xF0, 0x80, 0x80, and
// when writing out the bits you can see the letter F:
//
// 11110000
// 10000000
// 11110000
// 10000000
// 10000000
uint8_t fontset[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    // the first instruction executed will be at 0x200
    pc = START_ADDRESS;
    opcode = 0;
    index = 0;
    sp = 0;

    // clear the display
    for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
        video[i] = 0;
    }

    // clear the stack, keypad, and registers
    for (int i = 0; i < 16; ++i) {
        stack[i] = 0;
        registers[i] = 0;
        keypad[i] = 0;
    }

    // clear memory
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    // load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; i++) {
        memory[i] = fontset[i];
    }

    // Chip8 has an instruction which places a random number into a register.
    // this will initialize the RNG for the instruction.
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

bool Chip8::LoadRom(const char *filename) {
    // Open the file as a stream of binary and move the file pointer to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Couldn't open file " << filename << std::endl;
        return false;
    }
    // Get size of file and allocate a buffer to hold the contents
    std::streampos size = file.tellg();
    char *buffer = new char[size];

    // Go back to the beginning of the file and fill the buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    // Load the ROM contents into the Chip8's memory, starting at 0x200
    for (long i = 0; i < size; ++i) {
        memory[START_ADDRESS + i] = buffer[i];
    }

    // Free the buffer
    delete[] buffer;
    return true;
}

// Fetch, decode, and execute
void Chip8::Cycle() {
    // fetch the operation
    opcode = memory[pc] << 8 | memory[pc + 1];

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                // 00E0 - Clear Screen
                case 0x0000:
                    op_00E0();
                    break;
                    // 00EE return from subroutine
                case 0x000E:
                    op_00EE();
                    break;
                default:
                    op_null();
            }
            break;
            // 1NNN - Jump to address NNN
        case 0x1000:
            op_1nnn();
            break;
            // 2NNN - Call subroutine at NNN
        case 0x2000:
            op_2nnn();
            break;
        case 0x3000:
            op_3xkk();
            break;
        case 0x4000:
            op_4xkk();
            break;
        case 0x5000:
            op_5xy0();
            break;
        case 0x6000:
            op_6xkk();
            break;
        case 0x7000:
            op_7xkk();
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    op_8xy0();
                    break;
                case 0x0001:
                    op_8xy1();
                    break;
                case 0x0002:
                    op_8xy2();
                    break;
                case 0x0003:
                    op_8xy3();
                    break;
                case 0x0004:
                    op_8xy4();
                    break;
                case 0x0005:
                    op_8xy5();
                    break;
                case 0x0006:
                    op_8xy6();
                    break;
                case 0x0007:
                    op_8xy7();
                    break;
                case 0x000E:
                    op_8xyE();
                    break;
                default:
                    op_null();
            }
            break;
        case 0x9000:
            op_9xy0();
            break;
        case 0xA000:
            op_Annn();
            break;
        case 0xB000:
            op_Bnnn();
            break;
        case 0xC000:
            op_Cxkk();
            break;
        case 0xD000:
            op_Dxyn();
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E:
                    op_Ex9E();
                    break;
                case 0x00A1:
                    op_ExA1();
                    break;
                default:
                    op_null();
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    op_Fx07();
                    break;
                case 0x000A:
                    op_Fx0A();
                    break;
                case 0x0015:
                    op_Fx15();
                    break;
                case 0x0018:
                    op_Fx18();
                    break;
                case 0x001E:
                    op_Fx1E();
                    break;
                case 0x0029:
                    op_Fx29();
                    break;
                case 0x0033:
                    op_Fx33();
                    break;
                case 0x0055:
                    op_Fx55();
                    break;
                case 0x0065:
                    op_Fx65();
                    break;
                default:
                    op_null();
            }
            break;
        default:
            op_null();
    }

    // decrement the delay timer if it's been set
    if (delayTimer > 0) {
        --delayTimer;
    }

    // decrement the sound timer if it's been set
    if (soundTimer > 0) {
        --soundTimer;
    }
}

//region Instructions

// Clear the display
void Chip8::op_00E0() {
    // we can simply set the entire video buffer to zeroes.
    memset(video, 0, sizeof(video));
    drawFlag = true;
    pc += 2;
}

// Return from subroutine
void Chip8::op_00EE() {
    // decrement the stack pointer and reassign the program counter
    --sp;
    pc = stack[sp];
    pc += 2;
}

// Jump to location nnn
// A jump doesn’t remember its origin, so no stack interaction required.
void Chip8::op_1nnn() {
    pc = opcode & 0xFFF;
}

// Call subroutine at nnn
void Chip8::op_2nnn() {
    uint16_t address = opcode & 0xFFF;

    // put the current PC onto the top of the stack
    stack[sp] = pc;
    ++sp;
    pc = address;
}

// Skip next instruction if Vx = kk
void Chip8::op_3xkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;

    if (registers[Vx] == byte) {
        pc += 4;
    } else {
        pc += 2;
    }
}

// Skip next instruction if Vx != kk
void Chip8::op_4xkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    if (registers[Vx] != byte) {
        pc += 4;
    } else {
        pc += 2;
    }
}

// Skip next instruction if Vx = Vy
void Chip8::op_5xy0() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    if (registers[Vx] == registers[Vy]) {
        pc += 4;
    } else {
        pc += 2;
    }
}

// Set Vx = kk
void Chip8::op_6xkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    registers[Vx] = byte;
    pc += 2;
}

// Set Vx = Vx + kk
void Chip8::op_7xkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    registers[Vx] += byte;
    pc += 2;
}

// Set Vx = Vy
void Chip8::op_8xy0() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] = registers[Vy];
    pc += 2;
}

// Set Vx = Vx OR Vy
void Chip8::op_8xy1() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] |= registers[Vy];
    pc += 2;
}

// Set Vx = Vx AND Vy
void Chip8::op_8xy2() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] &= registers[Vy];
    pc += 2;
}

// Set Vx = Vx XOR Vy
void Chip8::op_8xy3() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] ^= registers[Vy];
    pc += 2;
}

// Set Vx = Vx + Vy, set VF = carry
// The values of Vx and Vy are added together. If the result is greater than
// 8 bits (i.e., > 255,), VF is set to 1, otherwise 0. Only the lowest 8 bits
// of the result are kept, and stored in Vx.
void Chip8::op_8xy4() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] += registers[Vy];
    if (registers[Vy] > (0xFF - registers[Vx])) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 2;
    }
    pc += 2;
}

// Set Vx = Vx - Vy, set VF = NOT borrow
// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx,
// and the results stored in Vx.
void Chip8::op_8xy5() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    if (registers[Vy] > registers[Vx]) {
        registers[0xF] = 0; // there's a borrow
    } else {
        registers[0xF] = 1;
    }
    pc += 2;
}

// Set Vx = Vx SHR 1
// If the least-significant bit of Vx is 1, then VF is set to 1,
// otherwise 0. Then Vx is divided by 2.
void Chip8::op_8xy6() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    // Save least-significant bit (LSB) in VF
    registers[0xF] = (registers[Vx] & 0x1);

    // divide by 2
    registers[Vx] >>= 1;
    pc += 2;
}

// Set Vx = Vy - Vx, set VF = NOT borrow
// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is
// subtracted from Vy, and the results stored in Vx.
void Chip8::op_8xy7() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[0xF] = (registers[Vy] > registers[Vx] ? 1 : 0);
    registers[Vx] = registers[Vy] - registers[Vx];
    pc += 2;
}

// Set Vx = Vx SHL 1
// If the most-significant bit of Vx is 1, then VF is set to 1,
// otherwise to 0. Then Vx is multiplied by 2.
void Chip8::op_8xyE() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    // Save most-significant bit (MSB) in VF
    registers[0xF] = (registers[Vx] & 0x80) >> 7;

    // multiply by 2
    registers[Vx] <<= 1;
    pc += 2;
}

// Skip next instruction if Vx != Vy
void Chip8::op_9xy0() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    if (registers[Vx] != registers[Vy]) {
        pc += 4;
    } else {
        pc += 2;
    }
}

// Set I = nnn
void Chip8::op_Annn() {
    uint16_t address = opcode & 0x0FFF;
    index = address;
    pc += 2;
}

// Jump to location nnn + V0
void Chip8::op_Bnnn() {
    uint16_t address = opcode & 0x0FFF;
    pc = registers[0] + address;
}

// Set Vx = random byte AND kk
void Chip8::op_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    registers[Vx] = randByte(randGen) & byte;
    pc += 2;
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
void Chip8::op_Dxyn() {
    unsigned short x = registers[(opcode & 0x0F00) >> 8];
    unsigned short y = registers[(opcode & 0x00F0) >> 4];
    unsigned short height = opcode & 0x000F;
    unsigned short pixel;

    registers[0xF] = 0;
    for (int row = 0; row < height; row++) {
        pixel = memory[index + row];
        for (int col = 0; col < 8; col++) {
            if ((pixel & (0x80 >> col)) != 0) {
                if (video[(x + col + ((y + row) * 64))] == 1) {
                    registers[0xF] = 1;
                }
                video[x + col + ((y + row) * 64)] ^= 1;
            }
        }
    }

    drawFlag = true;
    pc += 2;
}

// Skip next instruction if key with the value of Vx is pressed
void Chip8::op_Ex9E() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key = registers[Vx];
    if (keypad[key]) {
        pc += 4;
    } else {
        pc += 2;
    }
}

// Skip next instruction if key with the value of Vx is not pressed
void Chip8::op_ExA1() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key = registers[Vx];
    if (!keypad[key]) {
        pc += 4;
    } else {
        pc += 2;
    }
}

// Set Vx = delay timer value
void Chip8::op_Fx07() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    registers[Vx] = delayTimer;
    pc += 2;
}

// Wait for a key press, store the value of the key in Vx
void Chip8::op_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    for (unsigned int i = 0; i < 16; i++) {
        if (keypad[i] != 0) {
            // key is pressed, store the value of the key in Vx
            registers[Vx] = i;
            pc += 2;
            return;
        }
    }
}

// Set delay timer = Vx
void Chip8::op_Fx15() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    delayTimer = registers[Vx];
    pc += 2;
}

// Set sound timer = Vx
void Chip8::op_Fx18() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    soundTimer = registers[Vx];
    pc += 2;
}

// Set I = I + Vx
void Chip8::op_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    if (index + registers[Vx] > 0xFFF) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }
    index += registers[Vx];
    pc += 2;
}

// Set I = location of sprite for digit Vx
void Chip8::op_Fx29() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t digit = registers[Vx];
    index = 5 * digit;
    pc += 2;
}

// Store BCD representation of Vx in memory locations I, I+1, and I+2
//
// The interpreter takes the decimal value of Vx, and places the hundreds
// digit in memory at location in I, the tens digit at location I+1,
// the ones digit at location I+2.
void Chip8::op_Fx33() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t value = registers[Vx];

    // Ones-place
    memory[index + 2] = value % 10;
    value /= 10;

    // Tens-place
    memory[index + 1] = value % 10;
    value /= 10;

    // Hundreds-place
    memory[index] = value % 10;

    pc += 2;
}

// Store registers V0 through Vx in memory starting at location I
void Chip8::op_Fx55() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= Vx; ++i) {
        memory[index + i] = registers[i];
    }
    index = Vx + 1;
    pc += 2;
}

// Read registers V0 through Vx from memory starting at location I
void Chip8::op_Fx65() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= Vx; i++) {
        registers[i] = memory[index + i];
    }
    index = Vx + 1;
    pc += 2;
}

void Chip8::op_null() {
    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
    exit(3);
}

//endregion