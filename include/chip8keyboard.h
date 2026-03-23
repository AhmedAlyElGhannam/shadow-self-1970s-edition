#ifndef CHIP8KEYBOARD_H_
#define CHIP8KEYBOARD_H_

#include <stdbool.h>
#include "config.h"

// data structure representing device virtual keyboard
struct chip8_keyboard
{
    bool keyboard[CHIP8_TOTAL_KEYS]; // state of keys (up/down)
    const char* keyboard_map; // maps physical to virtual keys
};

void chip8_keyboard_set_map(struct chip8_keyboard* keyboard, const char* map);
int chip8_keyboard_map(struct chip8_keyboard* keyboard, char key);
void chip8_keyboard_down(struct chip8_keyboard* keyboard, int key);
void chip8_keyboard_up(struct chip8_keyboard* keyboard, int key);
bool chip8_keyboard_is_down(struct chip8_keyboard* keyboard, int key);

#endif