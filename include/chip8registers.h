#ifndef CHIP8REGISTERS_H_
#define CHIP8REGISTERS_H_

#include "config.h"

// device registers
struct chip8_registers
{
    unsigned char V[CHIP8_TOTAL_DATA_REGISTERS]; // 1B-wide registers used in operations
    unsigned short I; // 2B-wide register used for address shenanigans
    unsigned char delay_timer; // 1B-wide for delay timer
    unsigned char sound_timer; // 1B-wide for sound timer
    unsigned short PC; // 2B-wide program counter (points to next instruction to be executed)
    unsigned char SP; // 1B-wide stack pointer
};

#endif
