#include "computer.h"
#include <stdbool.h>
#include <stdio.h>

Computer CreateComputer(void) {
    Computer computer = {0};
    
    computer.instructionCounter = 0;
    computer.instructionSize = 0;
    computer.labelCount = 0;
    computer.CPUState = CPU_RUNNING;
    
    computer.registers[REG_G] = MAX_MEMORY - 1; 
    
    return computer;
}

CPUState GetCPUState(Computer* computer) {
    return computer->CPUState;
}

static uint32_t GetSourceValue(Computer* computer, ComputerInstruction inst) {
    if (inst.regSrc != REG_NONE) {
        return computer->registers[inst.regSrc];
    }
    return (uint32_t)inst.immediate;
}

void ExecuteNext(Computer* computer) {
    if (computer->CPUState != CPU_RUNNING || computer->instructionCounter >= computer->instructionSize) {
        computer->CPUState = CPU_HALTED;
        return;
    }

    ComputerInstruction inst = computer->instructions[computer->instructionCounter];
    bool jumped = false;
    uint32_t targetAddr = 0;

    // 1. Resolve Jump Targets (Labels)
    bool isJumpCommand = (inst.type == CMD_JMP || inst.type == CMD_JZ || 
                          inst.type == CMD_JNZ || inst.type == CMD_JG || 
                          inst.type == CMD_JL || inst.type == CMD_CALL);

    if (isJumpCommand) {
        if (inst.immediate >= 0 && (size_t)inst.immediate < computer->labelCount) {
            targetAddr = computer->labels[inst.immediate].instructionIndex;
        } else {
            computer->CPUState = CPU_HALTED;
            return;
        }
    }

    // 2. Execute OpCodes
    switch (inst.type) {
        case CMD_NOP:
            break;

        case CMD_MOVE:
            if (inst.regDest != REG_NONE) {
                computer->registers[inst.regDest] = GetSourceValue(computer, inst);
            }
            break;

        case CMD_LOAD:
            if (inst.regDest != REG_NONE) {
                uint32_t addr = GetSourceValue(computer, inst);
                if (addr < MAX_MEMORY) computer->registers[inst.regDest] = computer->memory[addr];
            }
            break;

        case CMD_STORE:
            if (inst.regDest != REG_NONE) {
                // regDest is the register to save, regSrc/immediate is the address
                uint32_t addr = GetSourceValue(computer, inst);
                if (addr < MAX_MEMORY) computer->memory[addr] = computer->registers[inst.regDest];
            }
            break;

        case CMD_ADD:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] += GetSourceValue(computer, inst);
            break;

        case CMD_SUB:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] -= GetSourceValue(computer, inst);
            break;

        case CMD_MUL:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] *= GetSourceValue(computer, inst);
            break;

        case CMD_DIV:
            if (inst.regDest != REG_NONE) {
                uint32_t val = GetSourceValue(computer, inst);
                if (val != 0) computer->registers[inst.regDest] /= val;
            }
            break;

        case CMD_MOD:
            if (inst.regDest != REG_NONE) {
                uint32_t val = GetSourceValue(computer, inst);
                if (val != 0) computer->registers[inst.regDest] %= val;
            }
            break;

        case CMD_AND:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] &= GetSourceValue(computer, inst);
            break;

        case CMD_OR:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] |= GetSourceValue(computer, inst);
            break;

        case CMD_XOR:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] ^= GetSourceValue(computer, inst);
            break;

        case CMD_NOT:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] = ~computer->registers[inst.regDest];
            break;

        case CMD_SHL:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] <<= GetSourceValue(computer, inst);
            break;

        case CMD_SHR:
            if (inst.regDest != REG_NONE) computer->registers[inst.regDest] >>= GetSourceValue(computer, inst);
            break;

        case CMD_CMP: {
            int32_t val1 = computer->registers[inst.regDest];
            int32_t val2 = (inst.regSrc != REG_NONE) ? 
                            computer->registers[inst.regSrc] : inst.immediate;
            
            int32_t result = val1 - val2;

            // Reset flags first, then set them based on result
            computer->registers[REG_FLAGS] = 0; 
            if (result == 0) {
                computer->registers[REG_FLAGS] |= FLAG_ZERO; // Define this as 1
            }
            if (result > 0) {
                computer->registers[REG_FLAGS] |= FLAG_GREATER; // Define this as 2
            }
            break;
        }

        case CMD_JMP:
            computer->instructionCounter = targetAddr;
            jumped = true;
            break;

        case CMD_JZ:
            if (computer->registers[REG_FLAGS] == 0) {
                computer->instructionCounter = targetAddr;
                jumped = true;
            }
            break;

        case CMD_JNZ:
            if (computer->registers[REG_FLAGS] != 0) {
                computer->instructionCounter = targetAddr;
                jumped = true;
            }
            break;

        case CMD_JG:
            if (computer->registers[REG_FLAGS] == 1) {
                computer->instructionCounter = targetAddr;
                jumped = true;
            }
            break;

        case CMD_JL:
            if (computer->registers[REG_FLAGS] == 2) {
                computer->instructionCounter = targetAddr;
                jumped = true;
            }
            break;

        case CMD_PUSH:
            if (computer->registers[REG_G] > 0) {
                // We use the corrected logic to see if we're pushing a reg or immediate
                uint32_t val = (inst.regDest != REG_NONE) ? computer->registers[inst.regDest] : (uint32_t)inst.immediate;
                computer->memory[computer->registers[REG_G]--] = val;
            }
            break;

        case CMD_POP:
            if (inst.regDest != REG_NONE && computer->registers[REG_G] < MAX_MEMORY - 1) {
                computer->registers[inst.regDest] = computer->memory[++computer->registers[REG_G]];
            }
            break;

        case CMD_CALL:
            if (computer->registers[REG_G] > 0) {
                computer->memory[computer->registers[REG_G]--] = computer->instructionCounter + 1;
                computer->instructionCounter = targetAddr;
                jumped = true;
            }
            break;

        case CMD_RETURN:
            if (computer->registers[REG_G] < MAX_MEMORY - 1) {
                computer->instructionCounter = computer->memory[++computer->registers[REG_G]];
                jumped = true;
            }
            break;
        
        case CMD_PRINT: {
            // Check regDest first (for PRINT REG_A), then regSrc, then immediate
            uint32_t pVal = 0;
            if (inst.regDest != REG_NONE) pVal = computer->registers[inst.regDest];
            else if (inst.regSrc != REG_NONE) pVal = computer->registers[inst.regSrc];
            else pVal = (uint32_t)inst.immediate;
            printf("%u\n", pVal);
        } break;

        case CMD_HALT:
            computer->CPUState = CPU_HALTED;
            break;

        default:
            break;
    }

    if (!jumped) {
        computer->instructionCounter++;
    }
}