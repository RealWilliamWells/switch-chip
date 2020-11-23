//
// Created by william on 2020-10-24.
//

#include <iostream>
#include <cmath>

#include "chip8.h"

void chip8::clearDisplay() {
    for (int i = 0; i<SCREEN_WIDTH; i++) {
        for (int j = 0; j<SCREEN_HEIGHT; j++) {
            screen[i][j] = 0;
        }
        drawFlag = true;
    }
}

void chip8::decodeAndExecuteOpcode() {
    char instruction[8];
    int x = (opcode >> 8) & 0x0F;
    int y = (opcode >> 4) & 0x0F;

    unsigned short nn = opcode & 0xFF;
    unsigned short nnn = opcode & 0x0FFF;

    char number = V[x];
    int bitPosition = 0;

    sprintf(instruction, "%x", opcode);

    switch(instruction[0]) {
        case '1':
            programCounter = nnn;
            break;

        case '2':
            stack[stackPointer] = programCounter;
            stackPointer++;
            programCounter = nnn;
            break;

        case '3':
            if (V[x] == nn) {
                programCounter += 4;
            } else {
                programCounter += 2;
            }
            break;

        case '4':
            if (V[x] != nn) {
                programCounter += 4;
            } else {
                programCounter += 2;
            }
            break;

        case '5':
            if (V[x] == V[y]) {
                programCounter += 4;
            } else {
                programCounter += 2;
            }
            break;

        case '6':
            V[x] = nn;
            programCounter += 2;
            break;

        case '7':
            V[x] += nn;
            programCounter += 2;
            break;

        case '8':
            switch(instruction[3]) {
                case '0':
                    V[x] = V[y];
                    programCounter += 2;
                    break;

                case '1':
                    V[x] = V[x]|V[y];
                    programCounter += 2;
                    break;

                case '2':
                    V[x] = V[x]&V[y];
                    programCounter += 2;
                    break;

                case '3':
                    V[x] = V[x]^V[y];
                    programCounter += 2;
                    break;

                case '4':
                    if(V[x] > (0xFF - V[y])) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[x] += V[y];
                    programCounter += 2;
                    break;

                case '5':
                    if(V[x] < V[y]) {
                        V[0xF] = 0;
                    } else {
                        V[0xF] = 1;
                    }
                    V[x] -= V[y];
                    programCounter += 2;
                    break;

                case '6':
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    programCounter += 2;
                    break;

                case '7':
                    if(V[x] > V[y]) {
                        V[0xF] = 0;
                    } else {
                        V[0xF] = 1;
                    }
                    V[x] = V[y]-V[x];
                    programCounter += 2;
                    break;

                case 'e':
                    V[0xF] = ((V[x] & 0x80) != 0);
                    V[x]<<=1;
                    programCounter += 2;
                    break;

                default:
                    handleOpcodeError(instruction);
                    break;
            }
            break;

        case '9':
            if (V[x] != V[y]) {
                programCounter += 4;
            } else {
                programCounter += 2;
            }
            break;

        case 'a':
            indexRegister = nnn;
            programCounter += 2;
            break;

        case 'b':
            programCounter = V[0] + nnn;
            break;

        case 'c':
            V[x]=(rand() % 256) & nn;
            programCounter += 2;
            break;

        case 'd': {
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int yLine = 0; yLine < height; yLine++) {
                pixel = memory[indexRegister + yLine];
                for (int xLine = 0; xLine < 8; xLine++) {
                    if ((pixel & (0x80 >> xLine)) != 0) {
                        if (screen[((V[x] + xLine) % SCREEN_WIDTH)][((V[y] + yLine) % SCREEN_HEIGHT)] == 1) {
                            V[0xF] = 1;
                        }
                        screen[((V[x] + xLine) % SCREEN_WIDTH)][((V[y] + yLine) % SCREEN_HEIGHT)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            programCounter += 2;
        }
        break;

        case 'e':
            if (opcode == 0x00E0) {
                clearDisplay();
                programCounter += 2;
            } else if (opcode == 0x00EE) {
                stackPointer--;
                programCounter = stack[stackPointer];
                programCounter += 2;
            } else if (instruction[3] == 'e') {
                if (keypad[V[x]]) {
                    programCounter += 2;
                }
                programCounter += 2;
            } else if (instruction[3] == '1') {
                if (!keypad[V[x]]) {
                    programCounter += 2;
                }
                programCounter += 2;
            } else {
                handleOpcodeError(instruction);
            }
            break;

        case 'f':
            switch (instruction[3]) {
                case '7':
                    V[x] = delayTimer;
                    programCounter += 2;
                    break;

                case 'a':
                    if (keypad[V[x]]) {
                        programCounter += 2;
                    }
                    break;

                case '5':
                    switch (instruction[2]) {
                        case '1':
                            delayTimer = V[x];
                            programCounter += 2;
                            break;

                        case '5':
                            if (indexRegister + x > 4096) {
                                x = 4096 - indexRegister;
                            }

                            for (int i = 0; i<=x; i++) {
                                memory[indexRegister + i] = V[i];
                            }
                            programCounter += 2;
                            break;

                        case '6':
                            if (indexRegister + x > 4096) {
                                x = 4096 - indexRegister;
                            }

                            for (int i = 0; i<=x; i++) {
                                V[i] = memory[indexRegister + i];
                            }
                            programCounter += 2;
                            break;

                        default:
                            handleOpcodeError(instruction);
                            break;
                    }
                    break;

                case '8':
                    soundTimer = V[x];
                    programCounter += 2;
                    break;

                case 'e':
                    if (indexRegister + V[x] > 255) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    indexRegister += V[x];
                    programCounter += 2;
                    break;

                case '9':
                    indexRegister = V[x] * 5;
                    programCounter += 2;
                    break;

                case '3':
                    memory[indexRegister] = V[x] / 100;
                    memory[indexRegister + 1] = (V[x] / 10) % 10;
                    memory[indexRegister + 2] = (V[x] % 100) % 10;
                    programCounter += 2;
                    break;
            }
            break;

        default:
            handleOpcodeError(instruction);
            break;
    }
}

void chip8::emulateCycle() {
    opcode = memory[programCounter] << 8 | memory[programCounter + 1];

    // DEBUG
//    freopen( "OpcodeDebug.txt", "w", stderr );
//    char instruction[8];
//    sprintf(instruction, "%x", opcode);
//    std::cout<<"Debug: "<<instruction<<"\n";

    decodeAndExecuteOpcode();

    if (delayTimer > 0) {
        delayTimer--;
    }

    if (soundTimer > 0) {
        if (soundTimer == 1) {
            playBeep = true;
        }
        soundTimer--;
    }
}

void chip8::initialize() {
    programCounter = 0x200;
    opcode = 0;
    indexRegister = 0;
    stackPointer = 0;

    clearDisplay();

    for (int i = 0; i < 16; i++) {
        stack[i] = 0;
    }

    for (int i = 0; i < 16; i++) {
        V[i] = 0;
        keypad[i] = false;
    }

    for (int i = 0; i < 4096; i++) {
        memory[i] = 0;
    }

    for (int i = 0; i < 80; i++) {
        memory[i] = fontSet[i];
    }
}

void chip8::loadGame(std::string rom) {
    FILE *file;
    file = fopen (rom.c_str(),"rb");

    // Get rom size
    fseek (file , 0 , SEEK_END);
    long int bufferSize = ftell (file);
    rewind(file);

    // Read bytes of rom into unsigned character array
    unsigned char buffer[bufferSize];
    fread(buffer, sizeof(unsigned char), bufferSize, file);
    fclose (file);

    if (bufferSize>0) {
        for (int i = 0; i < bufferSize; ++i) {
            memory[i + 512] = buffer[i];
        }
    } else {
        freopen( "RomLoadErrorLog.txt", "w", stderr );
        std::cerr<<"Error: failed to load rom!";
    }
}

void chip8::handleOpcodeError(char instruction[8]) {
    freopen( "OpcodeError.txt", "w", stderr );
    std::cerr<<"Error: Invalid opcode: "<<instruction<<"!";
    quit = true;
}