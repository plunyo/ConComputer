#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_PROGRAM_SIZE 1024
#define MAX_MEMORY       512

typedef enum {
    CMD_SKIP,       // do nothing, just move to the next instruction
    CMD_MOVE,       // copy a value or the content of a register into another register
    CMD_ADD,        // add a value or another register to a destination register
    CMD_SUB,        // subtract a value or another register from a destination register
    CMD_LOAD,       // read a value from memory into a register
    CMD_STORE,      // write the value from a register into memory
    CMD_PIXEL,      // draw a pixel on the screen or framebuffer
    CMD_JUMP,       // unconditional jump to a specific instruction index
    CMD_JUMP_Z,     // jump to a specific instruction if the register is zero
    CMD_JUMP_NZ,    // jump to a specific instruction if the register is not zero
    CMD_PRINT,      // output a value from a register or memory to the console
} OpCode;

typedef enum Register {
    REG_NONE = -1,
    REG_A, REG_B,
    REG_C, REG_D,
    REG_COUNT
} Register;

typedef struct CPUCommand {
    OpCode opCode;

    // registers
    Register regDest;    // destination register
    Register regSrc;     // source register

    // either an immediate value or a memory address
    union {
        int immediateValue;
        int memoryAddress;
    };

    // only for PIX commands
    struct {
        int x, y;
    } pixelCoords;
} CPUCommand;

typedef struct CPU {
    uint32_t registers[REG_COUNT];
    uint32_t programCounter;
    uint32_t memory[MAX_MEMORY];

    CPUCommand program[MAX_PROGRAM_SIZE];
    size_t programSize;
} CPU;

CPU CreateCPU(CPUCommand* program, size_t programSize);
void StepCPU(CPU* cpu);
bool CPUFinished(CPU* cpu);