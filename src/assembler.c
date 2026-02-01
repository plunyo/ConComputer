#include "assembler.h"
#include "computer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static OpCode StringToOpCode(const char* str) {
    if (strcmp(str, "NOP") == 0)           return CMD_NOP;
    if (strcmp(str, "MOVE") == 0)          return CMD_MOVE;
    if (strcmp(str, "LOAD") == 0)          return CMD_LOAD;
    if (strcmp(str, "STORE") == 0)         return CMD_STORE;
    if (strcmp(str, "ADD") == 0)           return CMD_ADD;
    if (strcmp(str, "SUB") == 0)           return CMD_SUB;
    if (strcmp(str, "MUL") == 0)           return CMD_MUL;
    if (strcmp(str, "DIV") == 0)           return CMD_DIV;
    if (strcmp(str, "MOD") == 0)           return CMD_MOD;
    if (strcmp(str, "AND") == 0)           return CMD_AND;
    if (strcmp(str, "OR") == 0)            return CMD_OR;
    if (strcmp(str, "XOR") == 0)           return CMD_XOR;
    if (strcmp(str, "NOT") == 0)           return CMD_NOT;
    if (strcmp(str, "SHL") == 0)           return CMD_SHL;
    if (strcmp(str, "SHR") == 0)           return CMD_SHR;
    if (strcmp(str, "CMP") == 0)           return CMD_CMP;
    if (strcmp(str, "JUMP") == 0)          return CMD_JMP;
    if (strcmp(str, "JUMP_ZERO") == 0)     return CMD_JZ;
    if (strcmp(str, "JUMP_NOT_ZERO") == 0) return CMD_JNZ;
    if (strcmp(str, "JUMP_GREATER") == 0)  return CMD_JG;
    if (strcmp(str, "JUMP_LESS") == 0)     return CMD_JL;
    if (strcmp(str, "PUSH") == 0)          return CMD_PUSH;
    if (strcmp(str, "POP") == 0)           return CMD_POP;
    if (strcmp(str, "CALL") == 0)          return CMD_CALL;
    if (strcmp(str, "RETURN") == 0)        return CMD_RETURN;
    if (strcmp(str, "PRINT") == 0)         return CMD_PRINT;
    if (strcmp(str, "HALT") == 0)          return CMD_HALT;

    return CMD_NOP;
}

static Register StringToRegister(const char* str) {
    if (!str) return REG_NONE;
    if (strcmp(str, "REG_A") == 0) return REG_A;
    if (strcmp(str, "REG_B") == 0) return REG_B;
    if (strcmp(str, "REG_C") == 0) return REG_C;
    if (strcmp(str, "REG_D") == 0) return REG_D;
    if (strcmp(str, "REG_E") == 0) return REG_E;
    if (strcmp(str, "REG_F") == 0) return REG_F;
    if (strcmp(str, "REG_G") == 0) return REG_G;
    if (strcmp(str, "REG_H") == 0) return REG_H;
    if (strcmp(str, "REG_FLAGS") == 0) return REG_FLAGS;

    return REG_NONE;
}

static int GetLabelIndex(Computer* computer, const char* name) {
    if (!name) return -1;
    for (size_t i = 0; i < computer->labelCount; i++) {
        if (strcmp(computer->labels[i].name, name) == 0) return (int)i;
    }
    return -1;
}

bool LoadInstructionsFromFile(Computer* computer, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return false;

    char line[256];
    uint32_t instCount = 0;

    // --- Pass 1: Collect Labels ---
    while (fgets(line, sizeof(line), file)) {
        char tempLine[256];
        strcpy(tempLine, line);
        char* token = strtok(tempLine, " ,;\"\t\n\r");
        if (!token || token[0] == '#') continue; // Skip empty lines and comments

        if (strcmp(token, "LABEL") == 0) {
            char* name = strtok(NULL, " ,;\"\t\n\r");
            if (name && computer->labelCount < MAX_LABELS) {
                computer->labels[computer->labelCount].name = strdup(name);
                computer->labels[computer->labelCount].instructionIndex = instCount;
                computer->labelCount++;
            }
        } else {
            instCount++;
        }
    }

    // --- Pass 2: Parse Instructions ---
    fseek(file, 0, SEEK_SET);
    instCount = 0;

    while (fgets(line, sizeof(line), file)) {
        char tempLine[256];
        strcpy(tempLine, line);
        char* token = strtok(tempLine, " ,;\"\t\n\r");
        
        // Skip null tokens, comments, and label declarations
        if (!token || token[0] == '#' || strcmp(token, "LABEL") == 0) continue;

        ComputerInstruction inst = {0};
        inst.type = StringToOpCode(token);
        inst.regDest = REG_NONE;
        inst.regSrc = REG_NONE;

        char* arg1 = strtok(NULL, " ,;\"\t\n\r");
        char* arg2 = strtok(NULL, " ,;\"\t\n\r");

        // Handle Jumps/Calls (Arg1 is a Label)
        if (inst.type == CMD_JMP || inst.type == CMD_JZ || inst.type == CMD_JNZ || 
            inst.type == CMD_JG || inst.type == CMD_JL || inst.type == CMD_CALL) {
            inst.immediate = GetLabelIndex(computer, arg1);
        } 
        else {
            // ARG 1
            if (arg1) {
                Register r = StringToRegister(arg1);
                if (r != REG_NONE) {
                    inst.regDest = r;
                } else {
                    // Use strtol(..., 0) to auto-detect hex (0x), octal (0), and decimal
                    inst.immediate = (int32_t)strtol(arg1, NULL, 0);
                }
            }
            // ARG 2
            if (arg2) {
                Register r = StringToRegister(arg2);
                if (r != REG_NONE) {
                    inst.regSrc = r;
                } else {
                    inst.immediate = (int32_t)strtol(arg2, NULL, 0);
                }
            }
        }

        if (instCount < MAX_INSTRUCTIONS) {
            computer->instructions[instCount++] = inst;
        }
    }

    computer->instructionSize = instCount;
    fclose(file);
    return true;
}