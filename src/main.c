#include <stdio.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdlib.h>

#include "shadowself.h"

// decided to use audio via SDL since beep was blocking the emulator on Linux
SDL_AudioDeviceID audio_device;

// beep callback prototype
void audio_callback(void* userdata, Uint8* stream, int len);

// a data structure used to map physical keyboard keys into 0-15 keys from emulator (using SDL)
const char keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_KP_0, SDLK_KP_1, SDLK_KP_2, SDLK_KP_3, SDLK_KP_4, SDLK_KP_5, SDLK_KP_6, SDLK_KP_7, SDLK_KP_8, SDLK_KP_9, SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Please provide a file to load\n");
        return -1;
    }

    const char* filename = argv[1];
    printf("file name is %s\n", filename);

    FILE* f = fopen(filename, "rb");
    if (!f)
    {
        printf("Failed to open file named %s", filename);
        return -1;
    }

    // seeking till the end then using ftell to get the binary size (neat trick)
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char buf[size];
    int res = fread(buf, size, 1, f);
    if (res != 1)
    {
        printf("Failed to read from file\n");
        return -1;
    }

    // defining device object
    struct chip8 chip8;

    // initializing memory and registers to 0 + copying default character set to designated memory address
    chip8_init(&chip8);

    // loading binary into device memory + initializing PC
    chip8_load(&chip8, buf, size);

    // setting keyboard map to map defined above
    chip8_keyboard_set_map(&chip8.keyboard, keyboard_map);
    
    // initializing SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    // SDL audio shenanigans
    SDL_AudioSpec desired, obtained;
    SDL_memset(&desired, 0, sizeof(desired));
    desired.freq     = BEEP_SAMPLE_RATE;
    desired.format   = AUDIO_S16SYS;
    desired.channels = 1;
    desired.samples  = 512;
    desired.callback = audio_callback;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);

    // SDL window shenanigans
    SDL_Window *window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER, 
        CHIP8_HEIGHT * CHIP8_WINDOW_MULTIPLIER, 
        SDL_WINDOW_SHOWN);

    // SDL rendering shenanigans on window defined above
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_TEXTUREACCESS_TARGET
    );
     
    while(true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // execute action corresponding to event
            switch (event.type)
            {
                // allows exiting superloop via Ctrl + C
                case SDL_QUIT:
                {
                    goto out;
                }
                break;

                // register key press at device virtual keyboard
                case SDL_KEYDOWN:
                {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(&chip8.keyboard, key);
                    if (vkey != -1)
                    {
                        chip8_keyboard_down(&chip8.keyboard, vkey);
                    }
                }
                break;

                // register key release at device virtual keyboard
                case SDL_KEYUP:
                {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(&chip8.keyboard, key);
                    if (vkey != -1)
                    {
                        chip8_keyboard_up(&chip8.keyboard, vkey);
                    }
                }
                break;
            }
        }

        // clear screen + set drawing color to white
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

        // draw pixels
        for (int x = 0; x < CHIP8_WIDTH; x++)
        {
            for (int y = 0; y < CHIP8_HEIGHT; y++)
            {
                if (chip8_screen_is_set(&chip8.screen, x, y))
                {
                    SDL_Rect r;
                    r.x = x * CHIP8_WINDOW_MULTIPLIER;
                    r.y = y * CHIP8_WINDOW_MULTIPLIER;
                    r.w = CHIP8_WINDOW_MULTIPLIER;
                    r.h = CHIP8_WINDOW_MULTIPLIER;
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }
        SDL_RenderPresent(renderer);

        // delay timer handler
        if (chip8.registers.delay_timer > 0)
        {
            usleep(5000); // 5 ms
            chip8.registers.delay_timer -= 1;
        }

        // sound timer/beep handler
        if (chip8.registers.sound_timer > 0)
        {
            SDL_PauseAudioDevice(audio_device, 0);
            chip8.registers.sound_timer--;
        }
        else
        {
            SDL_PauseAudioDevice(audio_device, 1);
        }

        // read instruction opcode + increment PC + execute instruction
        unsigned short opcode = chip8_memory_get_short(&chip8.memory, chip8.registers.PC);
        chip8.registers.PC += 2;
        chip8_exec(&chip8, opcode);
    }

out:
    // close beeping audio device
    SDL_CloseAudioDevice(audio_device);
    // destroy created window
    SDL_DestroyWindow(window);
    return 0;
}

// this function was added to allow beeping on Linux via SDL
void audio_callback(void* userdata, Uint8* stream, int len)
{
    static int sample_pos = 0;
    Sint16* buf = (Sint16*)stream;
    int samples = len / 2;
    int period = BEEP_SAMPLE_RATE / BEEP_FREQUENCY;

    for (int i = 0; i < samples; i++)
    {
        buf[i] = (sample_pos % period < period / 2) ? BEEP_AMPLITUDE : -BEEP_AMPLITUDE;
        sample_pos++;
    }
}