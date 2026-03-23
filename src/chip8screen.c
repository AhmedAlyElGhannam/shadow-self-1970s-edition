#include "chip8screen.h"
#include <assert.h>
#include <memory.h>

// assertion to check if x and y are within screen dimensions
static void chip8_screen_check_bounds(int x, int y)
{
    assert(x >= 0 && x < CHIP8_WIDTH);
    assert(y >= 0 && y < CHIP8_HEIGHT);
}

// clears screen aka sets all pixels to off
void chip8_screen_clear(struct chip8_screen* screen)
{
    memset(screen->pixels, 0, sizeof(screen->pixels));
}

// turns on pixel at designated coordinates x,y
void chip8_screen_set(struct chip8_screen* screen, int x, int y)
{
    chip8_screen_check_bounds(x, y);
    screen->pixels[y][x] = true;
}

// checks if a pixel at designated coordinates x,y is on or off
bool chip8_screen_is_set(struct chip8_screen* screen, int x, int y)
{
    chip8_screen_check_bounds(x, y);
    return screen->pixels[y][x];
}

// draws a sprite (group of bytes that give a binary representation of a picture) and returns true if a pixel collision happens
bool chip8_screen_draw_sprite(struct chip8_screen* screen, int x, int y, const char* sprite, int num)
{
    bool pixel_collision = false;

    for (int ly = 0 ;ly < num; ly++)
    {
        char c = sprite[ly];
        for (int lx = 0; lx < 8; lx++)
        {
            if ((c & (0b10000000 >> lx)) == 0)
                continue;

            if (screen->pixels[(ly + y) % CHIP8_HEIGHT][(lx + x) % CHIP8_WIDTH])
                pixel_collision = true;

            screen->pixels[(ly + y) % CHIP8_HEIGHT][(lx + x) % CHIP8_WIDTH] ^= true;
        }
    }

    return pixel_collision;
}
