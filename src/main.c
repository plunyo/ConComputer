#include <raylib.h>
#include "assembler.h"
#include "computer.h"
#include "timer.h"

static void onInstructionTimerFinished(void* context) {
    Computer* computer = (Computer*)context;
    ExecuteNext(computer);
}

int main() {
    InitWindow(600, 600, "ConComputer");

    Computer computer = CreateComputer();
    LoadInstructionsFromFile(&computer, "main.casm");

    Timer instructionTimer = CreateTimer(0.01f, true, onInstructionTimerFinished, &computer);

    while (!WindowShouldClose() && GetCPUState(&computer) == CPU_RUNNING) {
        UpdateTimer(&instructionTimer, GetFrameTime());

        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}