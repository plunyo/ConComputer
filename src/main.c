#include <raylib.h>

int main() {
    InitWindow(600, 600, "ConComputer");

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}