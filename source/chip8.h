//
// Created by william on 2020-10-24.
//

#ifndef SEA_CHIP_CHIP8_H
#define SEA_CHIP_CHIP8_H

#include <string>

class chip8 {
public:
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];

    unsigned short indexRegister;
    unsigned short programCounter;

    static const int SCREEN_WIDTH = 64;
    static const int SCREEN_HEIGHT = 32;
    unsigned char screen[SCREEN_WIDTH][SCREEN_HEIGHT];

    unsigned char delayTimer;
    unsigned char soundTimer;

    unsigned short stack[16];
    unsigned short stackPointer;

    bool keypad[16];

    unsigned char fontSet[80] =
            {
                    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                    0x20, 0x60, 0x20, 0x20, 0x70, // 1
                    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
            };

    bool drawFlag;
    bool quit = false;
    bool playBeep = true;

    void loadGame(std::string rom);

    void emulateCycle();

    void initialize();

    void decodeAndExecuteOpcode();

    void clearDisplay();

    void handleOpcodeError(char instruction[8]);
};


#endif //SEA_CHIP_CHIP8_H
