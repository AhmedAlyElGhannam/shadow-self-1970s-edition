#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "shadowself.h"

// array of sprites for hexadecimal digits (preloaded into device memory at initialization)
const char chip8_default_character_set[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
};

// initialize device data structure to 0 then copy sprites into memory
void chip8_init(struct chip8* chip8)
{
    memset(chip8, 0, sizeof(struct chip8));
    memcpy(&chip8->memory.memory, chip8_default_character_set, sizeof(chip8_default_character_set)); 
}

// copy binary into device memory then set program counter to load address (in preparation for binary execution)
void chip8_load(struct chip8* chip8, const char* buf, size_t size)
{
    assert((size + CHIP8_PROGRAM_LOAD_ADDRESS) < CHIP8_MEMORY_SIZE);
    memcpy(&chip8->memory.memory[CHIP8_PROGRAM_LOAD_ADDRESS], buf, size);
    chip8->registers.PC = CHIP8_PROGRAM_LOAD_ADDRESS;
}

// auxiliary function used to wait for keypress (see LD Vx, K below)
static char chip8_wait_for_key_press(struct chip8* chip8)
{
    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        if (event.type != SDL_KEYDOWN)
            continue;
        
        char c = event.key.keysym.sym;
        char chip8_key = chip8_keyboard_map(&chip8->keyboard, c);
        if (chip8_key != -1)
            return chip8_key;
    }

    // unreachable
    return -1;
}

// map chip8 instruction set (opcode) into operations on device and its components
void chip8_exec(struct chip8* chip8, unsigned short opcode)
{
    // auxiliary variables to extract parts of opcode used in some operations
    unsigned short nnn = (opcode & 0x0FFF);
    unsigned char x = ((opcode & 0x0F00) >> 8);
    unsigned char kk = (opcode & 0x00FF);
    unsigned char y = (opcode & 0x00F0) >> 4;
    unsigned char n = (opcode & 0x000F);

    switch (opcode)
    {
        // CLS -> clears display
        case 0x00E0:
        {
            chip8_screen_clear(&chip8->screen);
        }
        break;

        // RET -> returns from a subroutine (sets PC to value at top of stack)
        case 0x00EE:
        {
            chip8->registers.PC = chip8_stack_pop(chip8);
        }
        break;

        default:
        {
            switch (opcode & 0xF000)
            {
                // JP nnn -> set program counter to nnn
                case 0x1000:
                {
                    chip8->registers.PC = nnn;
                }
                break;
            
                // CALL nnn -> call subroutine at nnn
                // push current PC to top of stack then set PC to nnn
                case 0x2000:
                {
                    chip8_stack_push(chip8, chip8->registers.PC);
                    chip8->registers.PC = nnn;
                }
                break;
            
                // SE Vx, kk -> skip next instruction if Vx == kk
                case 0x3000:
                {
                    if (chip8->registers.V[x] == kk)
                    {
                        chip8->registers.PC += 2;
                    }
                }
                break;
            
                // SNE Vx, kk -> skip next instruction if Vx != kk
                case 0x4000:
                {
                    if (chip8->registers.V[x] != kk)
                    {
                        chip8->registers.PC += 2;
                    }
                }
                break;
            
                // SE Vx, Vy -> skip next instruction if Vx == Vy
                case 0x5000:
                {
                    if (chip8->registers.V[x] == chip8->registers.V[y])
                    {
                        chip8->registers.PC += 2;
                    }
                }
                break;
            
                // LD Vx, kk -> loads value kk into Vx register
                case 0x6000:
                {
                    chip8->registers.V[x] = kk;
                }
                break;
            
                // ADD Vx, kk -> set Vx = Vx + kk
                case 0x7000:
                {
                    chip8->registers.V[x] += kk;
                }
                break;
            
                // can be on of the following
                case 0x8000:
                {
                    switch (n)
                    {
                        // LD Vx, Vy -> loads Vy into Vx
                        case 0x00:
                        {
                            chip8->registers.V[x] = chip8->registers.V[y];
                        }
                        break;
                    
                        // OR Vx, Vy -> bitwise operation between Vx and Vy and stores the result in Vx
                        case 0x01:
                        {
                            chip8->registers.V[x] |= chip8->registers.V[y];
                        }
                        break;
                    
                        // AND Vx, Vy -> bitwise operation between Vx and Vy and stores the result in Vx
                        case 0x02:
                        {
                            chip8->registers.V[x] &= chip8->registers.V[y];
                        }
                        break;
                    
                        // XOR Vx, Vy -> bitwise operation between Vx and Vy and stores the result in Vx
                        case 0x03:
                        {
                            chip8->registers.V[x] ^= chip8->registers.V[y];
                        }
                        break;
                    
                        // ADD Vx, Vy -> add Vx and Vy and store the result in Vx. Vf = carry
                        case 0x04:
                        {
                            chip8->registers.V[0x0F] = ((chip8->registers.V[x] + chip8->registers.V[y]) > 255);
                            chip8->registers.V[x] += chip8->registers.V[y];
                        }
                        break;
                    
                        // SUB Vx, Vy -> sub Vy from Vx and store the result in Vx. Vf = !borrow
                        case 0x05:
                        {
                            chip8->registers.V[0x0F] = (chip8->registers.V[x] > chip8->registers.V[y]);
                            chip8->registers.V[x] -= chip8->registers.V[y];
                        }
                        break;
                    
                        // SHR Vx, {, Vy} -> shift Vx right by 1 (divide by 2) and set Vf based on the value of least significant bit of Vx
                        case 0x06:
                        {
                            chip8->registers.V[0x0F] = (chip8->registers.V[x] & 0x01);
                            chip8->registers.V[x] >>= 1; 
                        }
                        break;
                    
                        // SUBN Vx, Vy -> Vx = Vy - Vx and set Vf accordingly
                        case 0x07:
                        {
                            chip8->registers.V[0x0F] = (chip8->registers.V[y] > chip8->registers.V[x]);
                            chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
                        }
                        break;
                    
                        // SHL Vx {, Vy} -> shift Vx left by 1 (multiply by 2) and set Vf based n the value of most significant bit of Vx
                        case 0x0E:
                        {
                            chip8->registers.V[0x0F] = (chip8->registers.V[x] & 0x80);
                            chip8->registers.V[x] <<= 1; 
                        }
                        break;
                    }
                }
                break;
            
                // SNE Vx, Vy -> skip next instruction if Vx != Vy
                case 0x9000:
                {
                    if (chip8->registers.V[x] != chip8->registers.V[y])
                    {
                        chip8->registers.PC += 2;
                    }
                }
                break;
            
                // LD I, nnn -> sets I register to nnn
                case 0xA000:
                {
                    chip8->registers.I = nnn;
                }
                break;
            
                // JP V0, nnn -> jumps to address nnn + V0
                case 0xB000:
                {
                    chip8->registers.PC = nnn + chip8->registers.V[0x00];
                }
                break;
            
                // RND Vx, kk -> performs bitwise and with kk and a random number (0-255) then stores the result in Vx
                case 0xC000:
                {
                    srand(clock());
                    chip8->registers.V[x] = (rand() % 255) & kk;
                }
                break;
            
                // DRW Vx, Vy, n -> draws n-byte sprite starting at memory location I at (Vx, Vy) and sets Vf in case of collision
                case 0xD000:
                {
                    const char* sprite = (const char*) &chip8->memory.memory[chip8->registers.I];
                    chip8->registers.V[0x0F] = chip8_screen_draw_sprite(&chip8->screen, chip8->registers.V[x], chip8->registers.V[y], sprite, n);
                }
                break;
            
                // can be any of the following
                case 0xE000:
                {
                    switch (kk)
                    {
                        // SKP Vx -> skip next instruction if key with value of Vx is pressed
                        case 0x9E:
                        {
                            if (chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x]))
                            {
                                chip8->registers.PC += 2;
                            }
                        }
                        break;
                    
                        // SKNP Vx -> skip next instruction if key with value of Vx is NOT pressed
                        case 0xA1:
                        {
                            if (!chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x]))
                            {
                                chip8->registers.PC += 2;
                            }
                        }
                        break;
                    }

                }
                break;
            
                // can be any of the following
                case 0xF000:
                {
                    switch (kk)
                    {
                        // LD Vx, DT -> set Vx to delay timer value
                        case 0x07:
                        {
                            chip8->registers.V[x] = chip8->registers.delay_timer;
                        }
                        break;
                    
                        // LD Vx, K -> wait for a key press, store the value of the key in Vx (all execution stops until a key is pressed)
                        case 0x0A:
                        {
                            chip8->registers.V[x] = chip8_wait_for_key_press(chip8);
                        }
                        break;
                    
                        // LD DT, Vx -> set delay timer to Vx value
                        case 0x15:
                        {
                            chip8->registers.delay_timer = chip8->registers.V[x];
                        }
                        break;
                    
                        // LD ST, Vx -> set sound timer to Vx value
                        case 0x18:
                        {
                            chip8->registers.sound_timer = chip8->registers.V[x];
                        }
                        break;
                    
                        // ADD I, Vx -> add values of I and Vx and store the result in I
                        case 0x1E:
                        {
                            chip8->registers.I += chip8->registers.V[x];
                        }
                        break;
                    
                        // LD F, Vx -> set I to the location of sprite for value of Vx (?!!)
                        case 0x29:
                        {
                            chip8->registers.I = chip8->registers.V[x] * CHIP8_DEFAULT_SPRITE_HEIGHT;
                        }
                        break;
                    
                        // LD B, Vx -> store BCD representation of Vx in memory locations I, I+1, and I+2
                        case 0x33:
                        {
                            unsigned char hundreds = chip8->registers.V[x] / 100;
                            unsigned char tens = chip8->registers.V[x] / 10 % 10;
                            unsigned char units = chip8->registers.V[x] % 10;
                        
                            chip8_memory_set(&chip8->memory, chip8->registers.I, hundreds);
                            chip8_memory_set(&chip8->memory, chip8->registers.I + 1, tens);
                            chip8_memory_set(&chip8->memory, chip8->registers.I + 2, units);
                        }
                        break;
                    
                        // LD [I], Vx -> store registers V0 until Vx in memory starting at location I
                        case 0x55:
                        {
                            for (int i = 0; i <= x; i++)
                                chip8_memory_set(&chip8->memory, chip8->registers.I + i, chip8->registers.V[i]);
                        }
                        break;
                    
                        // LD Vx, [I] -> load values from memory into V0-Vx starting from address I
                        case 0x65:
                        {
                            for (int i = 0; i <= x; i++)
                                chip8->registers.V[i] = chip8_memory_get(&chip8->memory, chip8->registers.I + i);
                        }
                        break;
                    }
                }
                break;
            }
        }
        break;
    }
}
