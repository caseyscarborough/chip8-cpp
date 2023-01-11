#include "chip8.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstring>

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

const uint8_t VF = 0xF;

Chip8::Chip8() : rand_gen(std::chrono::system_clock::now().time_since_epoch().count()) {
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
    for (unsigned int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    // load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; i++) {
        memory[i] = fontset[i];
    }

    // Chip8 has an instruction which places a random number into a register.
    // this will initialize the RNG for the instruction.
    rand_byte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

bool Chip8::load_rom(const char *filename) {
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

uint8_t Chip8::Vx() {
    return (opcode & 0x0F00) >> 8;
}

uint8_t Chip8::Vy() {
    return (opcode & 0x00F0) >> 4;
}

uint8_t Chip8::kk() {
    return opcode & 0x00FF;
}

uint16_t Chip8::nnn() {
    return opcode & 0xFFF;
}

// Fetch, decode, and execute
void Chip8::cycle() {
    // fetch the operation
    opcode = memory[pc] << 8 | memory[pc + 1];

    // increment the program counter
    pc += 2;

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
    if (delay_timer > 0) {
        --delay_timer;
    }

    // decrement the sound timer if it's been set
    if (sound_timer > 0) {
        --sound_timer;
    }
}

//region Instructions

// Clear the display
void Chip8::op_00E0() {
    // we can simply set the entire video buffer to zeroes.
    memset(video, 0, sizeof(video));
    draw_flag = true;
}

// Return from subroutine
void Chip8::op_00EE() {
    // decrement the stack pointer and reassign the program counter
    --sp;
    pc = stack[sp];
}

// Jump to location nnn
// A jump doesn’t remember its origin, so no stack interaction required.
void Chip8::op_1nnn() {
    pc = nnn();
}

// Call subroutine at nnn
void Chip8::op_2nnn() {
    // put the current PC onto the top of the stack
    stack[sp] = pc;
    ++sp;
    pc = nnn();
}

// Skip next instruction if Vx = kk
void Chip8::op_3xkk() {
    if (registers[Vx()] == kk()) {
        pc += 2;
    }
}

// Skip next instruction if Vx != kk
void Chip8::op_4xkk() {
    if (registers[Vx()] != kk()) {
        pc += 2;
    }
}

// Skip next instruction if Vx = Vy
void Chip8::op_5xy0() {
    if (registers[Vx()] == registers[Vy()]) {
        pc += 2;
    }
}

// Set Vx = kk
void Chip8::op_6xkk() {
    registers[Vx()] = kk();
}

// Set Vx = Vx + kk
void Chip8::op_7xkk() {
    registers[Vx()] += kk();
}

// Set Vx = Vy
void Chip8::op_8xy0() {
    registers[Vx()] = registers[Vy()];
}

// Set Vx = Vx OR Vy
void Chip8::op_8xy1() {
    registers[Vx()] |= registers[Vy()];
}

// Set Vx = Vx AND Vy
void Chip8::op_8xy2() {
    registers[Vx()] &= registers[Vy()];
}

// Set Vx = Vx XOR Vy
void Chip8::op_8xy3() {
    registers[Vx()] ^= registers[Vy()];
}

// Set Vx = Vx + Vy, set VF = carry
// The values of Vx and Vy are added together. If the result is greater than
// 8 bits (i.e., > 255,), VF is set to 1, otherwise 0. Only the lowest 8 bits
// of the result are kept, and stored in Vx.
void Chip8::op_8xy4() {
    uint8_t Vx = this->Vx();
    uint8_t Vy = this->Vy();

    uint16_t sum = registers[Vx] + registers[Vy];
    registers[VF] = sum > 255 ? 1 : 0;
    registers[Vx] = sum & 0xFFu;
}

// Set Vx = Vx - Vy, set VF = NOT borrow
// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx,
// and the results stored in Vx.
void Chip8::op_8xy5() {
    uint8_t Vx = this->Vx();
    uint8_t Vy = this->Vy();
    registers[VF] = registers[Vy] > registers[Vx] ? 0 : 1;
    registers[Vx] -= registers[Vy];
}

// Set Vx = Vx SHR 1
// If the least-significant bit of Vx is 1, then VF is set to 1,
// otherwise 0. Then Vx is divided by 2.
void Chip8::op_8xy6() {
    uint8_t Vx = this->Vx();

    // Save least-significant bit (LSB) in VF
    registers[VF] = (registers[Vx] & 0x1);

    // divide by 2
    registers[Vx] >>= 1;
}

// Set Vx = Vy - Vx, set VF = NOT borrow
// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is
// subtracted from Vy, and the results stored in Vx.
void Chip8::op_8xy7() {
    uint8_t Vx = this->Vx();
    uint8_t Vy = this->Vy();

    registers[VF] = (registers[Vy] > registers[Vx] ? 1 : 0);
    registers[Vx] = registers[Vy] - registers[Vx];
}

// Set Vx = Vx SHL 1
// If the most-significant bit of Vx is 1, then VF is set to 1,
// otherwise to 0. Then Vx is multiplied by 2.
void Chip8::op_8xyE() {
    uint8_t Vx = this->Vx();

    // Save most-significant bit (MSB) in VF
    registers[VF] = (registers[Vx] & 0x80) >> 7;

    // multiply by 2
    registers[Vx] <<= 1;
}

// Skip next instruction if Vx != Vy
void Chip8::op_9xy0() {
    if (registers[Vx()] != registers[Vy()]) {
        pc += 2;
    }
}

// Set I = nnn
void Chip8::op_Annn() {
    index = nnn();
}

// Jump to location nnn + V0
void Chip8::op_Bnnn() {
    pc = registers[0] + nnn();
}

// Set Vx = random byte AND kk
void Chip8::op_Cxkk() {
    registers[Vx()] = rand_byte(rand_gen) & kk();
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
void Chip8::op_Dxyn() {
    uint8_t x = registers[Vx()] % VIDEO_WIDTH;
    uint8_t y = registers[Vy()] % VIDEO_HEIGHT;
    uint8_t height = opcode & 0x000F;

    registers[VF] = 0;
    for (int row = 0; row < height; row++) {
        uint8_t byte = memory[index + row];
        for (int col = 0; col < 8; col++) {
            uint8_t sprite_pixel = byte & (0x80 >> col);
            uint32_t *screen_pixel = &video[(y + row) * VIDEO_WIDTH + (x + col)];
            if (sprite_pixel != 0) {
                // sprite pixel is on
                if (*screen_pixel == 1) {
                    // screen pixel is also on, we have a collision
                    registers[VF] = 1;
                }

                *screen_pixel ^= 1;
            }
        }
    }

    draw_flag = true;
}

// Skip next instruction if key with the value of Vx is pressed
void Chip8::op_Ex9E() {
    if (keypad[registers[Vx()]]) {
        pc += 2;
    }
}

// Skip next instruction if key with the value of Vx is not pressed
void Chip8::op_ExA1() {
    if (!keypad[registers[Vx()]]) {
        pc += 2;
    }
}

// Set Vx = delay timer value
void Chip8::op_Fx07() {
    registers[Vx()] = delay_timer;
}

// Wait for a key press, store the value of the key in Vx
void Chip8::op_Fx0A() {
    uint8_t Vx = this->Vx();
    for (unsigned int i = 0; i < 16; i++) {
        if (keypad[i] != 0) {
            // key is pressed, store the value of the key in Vx
            registers[Vx] = i;
            return;
        }
    }

    // decrement the program counter to re-execute this instruction
    // until a key is pressed
    pc -= 2;
}

// Set delay timer = Vx
void Chip8::op_Fx15() {
    delay_timer = registers[Vx()];
}

// Set sound timer = Vx
void Chip8::op_Fx18() {
    sound_timer = registers[Vx()];
}

// Set I = I + Vx
void Chip8::op_Fx1E() {
    uint8_t Vx = this->Vx();
    registers[0xF] = index + registers[Vx] > 0xFFF ? 1 : 0;
    index += registers[Vx];
}

// Set I = location of sprite for digit Vx
void Chip8::op_Fx29() {
    index = 5 * registers[Vx()];
}

// Store BCD representation of Vx in memory locations I, I+1, and I+2
//
// The interpreter takes the decimal value of Vx, and places the hundreds
// digit in memory at location in I, the tens digit at location I+1,
// the ones digit at location I+2.
void Chip8::op_Fx33() {
    uint8_t value = registers[Vx()];
    memory[index + 2] = value % 10; // Ones-place
    value /= 10;
    memory[index + 1] = value % 10; // Tens-place
    value /= 10;
    memory[index] = value % 10; // Hundreds-place
}

// Store registers V0 through Vx in memory starting at location I
void Chip8::op_Fx55() {
    uint8_t Vx = this->Vx();
    for (uint8_t i = 0; i <= Vx; ++i) {
        memory[index + i] = registers[i];
    }
    index = Vx + 1;
}

// Read registers V0 through Vx from memory starting at location I
void Chip8::op_Fx65() {
    uint8_t Vx = this->Vx();
    for (uint8_t i = 0; i <= Vx; i++) {
        registers[i] = memory[index + i];
    }
    index = Vx + 1;
}

void Chip8::op_null() {
    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
    std::exit(EXIT_FAILURE);
}

//endregion
