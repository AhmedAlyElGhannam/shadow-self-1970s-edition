#ifndef CHIP8MEMORY_H_
#define CHIP8MEMORY_H_

#include "config.h"

// device memory is 4KB in total (represented as an array)
struct chip8_memory
{
    unsigned char memory[CHIP8_MEMORY_SIZE];
};

void chip8_memory_set(struct chip8_memory* memory, unsigned int index, unsigned char val);
unsigned char chip8_memory_get(struct chip8_memory* memory, unsigned int index);
unsigned short chip8_memory_get_short(struct chip8_memory* memory, int index);

#endif
