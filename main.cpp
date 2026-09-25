#include <raylib.h>
#include <iostream>
#include <string>

struct Player {
    Vector2 position;
    float p_width = 40.0f; // people always use this as a constant but i am just gonna make it a variable because i might add a power up that changes the scale
    float p_height = 40.0f;
    Rectangle p_rect = {position.x, position.y, p_width, p_height};

};

int main(int argc, char* argv[]) {
    int width = 600;
    int height = 400;
    std::string title = "Billy Ray V0.0.1"; //i was gonna include the version number in a variable but i am just gonna do it this way :p
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    Player player;
    player.position = {width/2.0f, height/2.0f};
        
    while(WindowShouldClose() == false){
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        DrawRectangleRounded(player.p_rect, 2.0f, 3, YELLOW);
        


        EndDrawing();
    }
    CloseWindow();
}   