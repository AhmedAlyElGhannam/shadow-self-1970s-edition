#ifndef CHIP8STACK_H_
#define CHIP8STACK_H_

#include "config.h"

// forward declaration for device data structure for prototype
struct chip8;

// device stack is represented as an array of 2 bytes with length 16 *kawaii!*
struct chip8_stack
{
    unsigned short stack[CHIP8_TOTAL_STACK_DEPTH];
};

void chip8_stack_push(struct chip8* chip8, unsigned short val);
unsigned short chip8_stack_pop(struct chip8* chip8);

#endif