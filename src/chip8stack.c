#include "shadowself.h"
#include <assert.h>

// assertion to prevent stack overflows
static void chip8_stack_in_bounds(struct chip8* chip8)
{
    assert(chip8->registers.SP < CHIP8_TOTAL_STACK_DEPTH);
}

// increments stack ptr then pushes value into stack
void chip8_stack_push(struct chip8* chip8, unsigned short val)
{
    chip8_stack_in_bounds(chip8);
    chip8->registers.SP += 1;
    chip8->stack.stack[chip8->registers.SP] = val;
    return;
}

// retrieves top of stack then decrements stack ptr
unsigned short chip8_stack_pop(struct chip8* chip8)
{
    chip8_stack_in_bounds(chip8);
    unsigned short res = chip8->stack.stack[chip8->registers.SP];
    chip8->registers.SP -= 1;
    return res;
}
