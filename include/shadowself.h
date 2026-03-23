#ifndef SHADOW_H_
#define SHADOW_H_

#include <stddef.h>

#include "config.h"
#include "chip8memory.h"
#include "chip8registers.h"
#include "chip8stack.h"
#include "chip8keyboard.h"
#include "chip8screen.h"

// device is represented as a chonky data structure containing all of the other structures necessary for successful emulation
struct chip8
{
    struct chip8_memory memory;
    struct chip8_stack stack;
    struct chip8_registers registers;
    struct chip8_keyboard keyboard;
    struct chip8_screen screen;
};

void chip8_init(struct chip8* chip8);
void chip8_exec(struct chip8* chip8, unsigned short opcode);
void chip8_load(struct chip8* chip8, const char* buf, size_t size);

#endif