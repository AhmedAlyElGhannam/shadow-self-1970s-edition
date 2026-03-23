#include "chip8memory.h"
#include <assert.h>

// assertion to check if index is within bounds
static void chip8_is_memory_in_bounds(unsigned int index)
{
    assert(index < CHIP8_MEMORY_SIZE);
}

// sets memory address at index with val
void chip8_memory_set(struct chip8_memory* memory, unsigned int index, unsigned char val)
{
    chip8_is_memory_in_bounds(index);
    memory->memory[index] = val;
}

// returns value stored at indexed memory address
unsigned char chip8_memory_get(struct chip8_memory* memory, unsigned int index)
{
    chip8_is_memory_in_bounds(index);
    return memory->memory[index];
}

// returns two consecutive bytes as a short (useful for opcode retrieval)
unsigned short chip8_memory_get_short(struct chip8_memory* memory, int index)
{
    unsigned char byte1 = chip8_memory_get(memory, index);
    unsigned char byte2 = chip8_memory_get(memory, index + 1);
    return ((byte1 << 8) | byte2);
}
