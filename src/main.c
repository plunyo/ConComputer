#include <raylib.h>
#include <stdbool.h>
#include "cpu/cpu.h"
#include "timer/timer.h"                                       

static void onCPUTimerFinished(void* context) {
    StepCPU((CPU*)context);
}

int main(int argc, char* argv[]) {
    InitWindow(800, 600, "My Game");
    SetTraceLogLevel(LOG_ERROR);

    CPUCommand program[] = {
    // setup: a=0, b=1, counter=10 (remaining loops)
    { CMD_MOVE, .regDest = REG_A, .immediateValue = 0 },
    { CMD_MOVE, .regDest = REG_B, .immediateValue = 1 },
    { CMD_MOVE, .regDest = REG_C, .immediateValue = 10 },

    // loop start at index 3
    { CMD_PRINT, .regSrc = REG_A },             // print a
    { CMD_ADD,   .regDest = REG_D, .regSrc = REG_A }, // D = a
    { CMD_ADD,   .regDest = REG_D, .regSrc = REG_B }, // D = a + b
    { CMD_MOVE,  .regDest = REG_A, .regSrc = REG_B }, // a = b
    { CMD_MOVE,  .regDest = REG_B, .regSrc = REG_D }, // b = nextFib
    { CMD_SUB,   .regDest = REG_C, .immediateValue = 1 }, // counter--
    { CMD_JUMP_NZ, .regDest = REG_C, .immediateValue = 3 }, // if counter != 0, jump to loop start
};


    CPU cpu = CreateCPU(program, sizeof(program) / sizeof(program[0]));
    Timer cpuTimer = CreateTimer(0.02f, true, onCPUTimerFinished, &cpu);

    while (!WindowShouldClose() && !CPUFinished(&cpu)) {
        UpdateTimer(&cpuTimer, GetFrameTime());
    
        BeginDrawing();
            ClearBackground(BLACK);
            DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}