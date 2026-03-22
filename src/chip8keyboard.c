#include "shadowself.h"
#include <assert.h>

static void chip8_keyboard_ensure_in_bounds(int key)
{
    assert(key >= 0 && key < CHIP8_TOTAL_KEYS);
}

void chip8_keyboard_set_map(struct chip8_keyboard* keyboard, const char* map)
{
    keyboard->keyboard_map = map;
}

int chip8_keyboard_map(struct chip8_keyboard* keyboard, char key)
{
    int mapped_key = -1;

    for (int i = 0; i < CHIP8_TOTAL_KEYS; i++)
    {
        if (keyboard->keyboard_map[i] == key)
        {
            mapped_key = i;
            goto out;
        }
    }

out:
    return mapped_key;
}

void chip8_keyboard_down(struct chip8_keyboard* keyboard, int key)
{
    chip8_keyboard_ensure_in_bounds(key);
    keyboard->keyboard[key] = true;
}

void chip8_keyboard_up(struct chip8_keyboard* keyboard, int key)
{
    chip8_keyboard_ensure_in_bounds(key);
    keyboard->keyboard[key] = false;
}

bool chip8_keyboard_is_down(struct chip8_keyboard* keyboard, int key)
{
    chip8_keyboard_ensure_in_bounds(key);
    return keyboard->keyboard[key];
}