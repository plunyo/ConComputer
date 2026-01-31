#include "cpu.h"
#include <stdio.h>
#include <string.h>

CPU CreateCPU(CPUCommand* program, size_t programSize) {
    CPU cpu;
    memset(&cpu, 0, sizeof(CPU)); // zero out registers and memory
    if (programSize > MAX_PROGRAM_SIZE) {
        programSize = MAX_PROGRAM_SIZE;
    }
    memcpy(cpu.program, program, programSize * sizeof(CPUCommand));
    cpu.programSize = programSize;
    cpu.programCounter = 0;
    return cpu;
}

bool CPUFinished(CPU* cpu) {
    return cpu->programCounter >= cpu->programSize;
}

static int GetValue(CPU* cpu, CPUCommand* cmd) {
    if (cmd->regSrc != REG_NONE) {
        return cpu->registers[cmd->regSrc];
    } else {
        return cmd->immediateValue;
    }
}

// execute one instruction
void StepCPU(CPU* cpu) {
    if (CPUFinished(cpu)) return;

    CPUCommand* cmd = &cpu->program[cpu->programCounter];
    int value;

    switch (cmd->opCode) {
        case CMD_SKIP:
            // do nothing
            break;

        case CMD_MOVE:
            value = GetValue(cpu, cmd);
            if (cmd->regDest != REG_NONE)
                cpu->registers[cmd->regDest] = value;
            break;

        case CMD_ADD:
            value = GetValue(cpu, cmd);
            if (cmd->regDest != REG_NONE)
                cpu->registers[cmd->regDest] += value;
            break;

        case CMD_SUB:
            value = GetValue(cpu, cmd);
            if (cmd->regDest != REG_NONE)
                cpu->registers[cmd->regDest] -= value;
            break;

        case CMD_LOAD:
            if (cmd->regDest != REG_NONE && cmd->memoryAddress < MAX_MEMORY)
                cpu->registers[cmd->regDest] = cpu->memory[cmd->memoryAddress];
            break;

        case CMD_STORE:
            if (cmd->regSrc != REG_NONE && cmd->memoryAddress < MAX_MEMORY)
                cpu->memory[cmd->memoryAddress] = cpu->registers[cmd->regSrc];
            break;

        case CMD_PIXEL:
            // placeholder: just print pixel coords and reg value
            if (cmd->regSrc != REG_NONE)
                printf("PIXEL at (%d,%d) value=%d\n", cmd->pixelCoords.x, cmd->pixelCoords.y, cpu->registers[cmd->regSrc]);
            break;

        case CMD_JUMP:
            if (cmd->immediateValue < cpu->programSize)
                cpu->programCounter = cmd->immediateValue - 1; // -1 because we'll increment after switch
            break;

        case CMD_JUMP_Z:
            if (cmd->regDest != REG_NONE && cpu->registers[cmd->regDest] == 0 && cmd->immediateValue < cpu->programSize)
                cpu->programCounter = cmd->immediateValue - 1;
            break;

        case CMD_JUMP_NZ:
            if (cmd->regDest != REG_NONE && cpu->registers[cmd->regDest] != 0 && cmd->immediateValue < cpu->programSize)
                cpu->programCounter = cmd->immediateValue - 1;
            break;

        case CMD_PRINT:
            if (cmd->regSrc != REG_NONE)
                printf("%d\n", cpu->registers[cmd->regSrc]);
            else
                printf("%d\n", cmd->immediateValue);
            break;

        default:
            printf("Unknown opcode %d\n", cmd->opCode);
            break;
    }

    cpu->programCounter++;
}
