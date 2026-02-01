#pragma once

#include <stddef.h>
#include <stdint.h>

#define MAX_INSTRUCTIONS 1024
#define MAX_MEMORY       1024
#define MAX_LABELS       64

#define SCREEN_HEIGHT 128
#define SCREEN_WIDTH  128

#define FLAG_ZERO     (1 << 0)  // 0001 in binary
#define FLAG_GREATER  (1 << 1)  // 0010 in binary
#define FLAG_LESS     (1 << 2)  // 0100 in binary

typedef enum {
    // no-op
    CMD_NOP,        // does nothing

    // data movement
    CMD_MOVE,       // reg <- reg / immediate
    CMD_LOAD,       // reg <- memory[address]
    CMD_STORE,      // memory[address] <- reg

    // arithmetic
    CMD_ADD,        // reg += value
    CMD_SUB,        // reg -= value
    CMD_MUL,        // reg *= value
    CMD_DIV,        // reg /= value
    CMD_MOD,        // reg %= value

    // bitwise
    CMD_AND,        // reg &= value
    CMD_OR,         // reg |= value
    CMD_XOR,        // reg ^= value
    CMD_NOT,        // reg = ~reg
    CMD_SHL,        // reg <<= value
    CMD_SHR,        // reg >>= value

    // comparison / flags
    CMD_CMP,        // compare two values, set flags

    // control flow
    CMD_JMP,        // unconditional jump
    CMD_JZ,         // jump if zero flag set
    CMD_JNZ,        // jump if zero flag not set
    CMD_JG,         // jump if greater
    CMD_JL,         // jump if less

    // stack (optional but powerful)
    CMD_PUSH,       // push reg to stack
    CMD_POP,        // pop stack to reg
    CMD_CALL,       // call subroutine
    CMD_RETURN,        // return from subroutine

    // io
    CMD_PRINT,

    // system
    CMD_HALT        // stop execution
} OpCode;

typedef enum Register {
    REG_NONE = -1,
    REG_A, REG_B, REG_C, REG_D,
    REG_E, REG_F, REG_G, REG_H,
    REG_FLAGS,
    REG_COUNT
} Register;

typedef struct ComputerInstruction {
    OpCode type;

    Register regDest;
    Register regSrc;

    int32_t immediate;
} ComputerInstruction;

typedef enum CPUState {
    CPU_RUNNING,
    CPU_HALTED,
    CPU_WAITING
} CPUState;

typedef struct Label {
    const char* name;
    uint32_t    instructionIndex;
} Label;


typedef struct Computer {
    ComputerInstruction  instructions[MAX_INSTRUCTIONS];
    uint32_t             instructionCounter;
    size_t               instructionSize;

    CPUState             CPUState;
    uint32_t             registers[REG_COUNT];
    uint32_t             memory[MAX_MEMORY];

    Label                labels[MAX_LABELS];
    size_t               labelCount;

    uint8_t              vRam[SCREEN_WIDTH][SCREEN_HEIGHT];
} Computer;

Computer CreateComputer();
void     ExecuteNext(Computer* computer);
CPUState GetCPUState(Computer* computer);