#include <raylib.h>
#include <iostream>
#include <string>


int main(int argc, char* argv[]) {
    int width = 600;
    int height = 400;
    std::string title = "Billy Ray V0.0.1"; //i was gonna include the version number in a variable but i am just gonna do it this way :p
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);
    while(WindowShouldClose() == false){
        BeginDrawing();
        ClearBackground(DARKGRAY);
    
        EndDrawing();
    }
    CloseWindow();
}   