#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include <string>
#include <switch.h>

#include "chip8.h"

chip8 chip8;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
SDL_Event e;
SDL_Window* window = NULL;
SDL_Renderer* renderer;

const int AMPLITUDE = 28000;
const int SAMPLE_RATE = 44100;

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2;
    int &sample_nr(*(int*)user_data);

    for(int i = 0; i < length; i++, sample_nr++)
    {
        double time = (double)sample_nr / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
}

void playBeep() {
    int sample_nr = 0;

    SDL_AudioSpec want;
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = audio_callback;
    want.userdata = &sample_nr;

    SDL_AudioSpec have;
    SDL_OpenAudio(&want, &have);

    SDL_PauseAudio(0);
    SDL_Delay(50);
    SDL_PauseAudio(1);

    chip8.playBeep = false;
}

void setupGraphics() {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        std::cout<<"SDL could not initialize! SDL_Error: %s\n"<<SDL_GetError();
    } else {
        window = SDL_CreateWindow("switch-chip", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (window == NULL) {
            std::cout<<"Window could not be created! SDL_Error: %s\n"<<SDL_GetError();
        } else {
            SDL_UpdateWindowSurface( window );
        }
    }
}

void drawGraphics() {
    SDL_Rect fillRect;
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(renderer);

    for (int i = 0; i < chip8.SCREEN_WIDTH; i++) {
        for (int j = 0; j < chip8.SCREEN_HEIGHT; j++) {
            if (chip8.screen[i][j] == 1) {
                fillRect.x = i*(SCREEN_WIDTH/chip8.SCREEN_WIDTH);
                fillRect.y = j*(SCREEN_HEIGHT/chip8.SCREEN_HEIGHT);
                fillRect.w = SCREEN_WIDTH/chip8.SCREEN_WIDTH;
                fillRect.h = SCREEN_HEIGHT/chip8.SCREEN_HEIGHT;

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(renderer, &fillRect);
            }
        }
    }

    SDL_RenderPresent(renderer);
    chip8.drawFlag = false;
}

void checkInput() {
    while (SDL_PollEvent( &e ) != 0) {
        if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP) {
            bool keyUp = true;
            if( e.type == SDL_JOYBUTTONUP ) {
                keyUp = false;
            }

            // seek for joystick #0
            if (e.jbutton.which == 0) {
                switch (e.jbutton.button) {
                    case 10:
                        // (+) button
                        chip8.quit = true;
                        break;

//                    case SDLK_b:
//                        chip8.keypad[0x0] = keyUp;
//                        break;
//
//                    case SDLK_z:
//                        chip8.keypad[0x01] = keyUp;
//                        break;

                    case 15:
                        // Dpad down
                        chip8.keypad[0x02] = keyUp;
                        break;

//                    case SDLK_c:
//                        chip8.keypad[0x03] = keyUp;
//                        break;

                    case 12:
                        // Dpad left
                        chip8.keypad[0x04] = keyUp;
                        break;

//                    case SDLK_s:
//                        chip8.keypad[0x05] = keyUp;
//                        break;

                    case 14:
                        // Dpad right
                        chip8.keypad[0x06] = keyUp;
                        break;

//                    case SDLK_q:
//                        chip8.keypad[0x07] = keyUp;
//                        break;

                    case 13:
                        // Dpad up
                        chip8.keypad[0x08] = keyUp;
                        break;

//                    case SDLK_e:
//                        chip8.keypad[0x09] = keyUp;
//                        break;
//
//                    case SDLK_r:
//                        chip8.keypad[0x0A] = keyUp;
//                        break;
//
//                    case SDLK_t:
//                        chip8.keypad[0x0B] = keyUp;
//                        break;
//
//                    case SDLK_y:
//                        chip8.keypad[0x0C] = keyUp;
//                        break;
//
//                    case SDLK_f:
//                        chip8.keypad[0x0D] = keyUp;
//                        break;
//
//                    case SDLK_g:
//                        chip8.keypad[0x0E] = keyUp;
//                        break;
//
//                    case SDLK_h:
//                        chip8.keypad[0x0F] = keyUp;
//                        break;
                }
            }
        }
    }
}

int main(int argCount, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    romfsInit();

    std::string fileName;

    fileName = "romfs:/c8games/custom-rom";

    setupGraphics();

    chip8.initialize();
    chip8.loadGame(fileName);

    // Emulation loop
    while( !chip8.quit ) {
        if (chip8.playBeep) {
            playBeep();
        }

        checkInput();

        chip8.emulateCycle();

        if(chip8.drawFlag) {
            drawGraphics();
        }

        // Set to run at 400hz
        SDL_Delay(10/4);

        consoleUpdate(NULL);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow( window );
    SDL_CloseAudio();

    SDL_Quit();

    romfsExit();

    return 0;
}
