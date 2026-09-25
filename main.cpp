#include <raylib.h>
#include <iostream>
#include <string>

struct Sawblade1 {
    float sb_radius = 40.0f;
    Vector2 sb_position = {80.0f, 130.0f};
};


struct Player {
    Vector2 position;
    float p_width = 40.0f; // people always use this as a constant but i am just gonna make it a variable because i might add a power up that changes the scale
    float p_height = 40.0f;
    float speed = 150.0f;

};

int main(int argc, char* argv[]) {
    int width = 600;
    int height = 400;
    std::string title = "Billy Ray V0.0.1"; //i was gonna include the version number in a variable but i am just gonna do it this way :p
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    Player player;
    player.position = {width/2.0f, height/2.0f}; 
    Sawblade1 sawblade1;
    bool dead = false;
    while(WindowShouldClose() == false && dead == false){
        Rectangle p_rect = {player.position.x, player.position.y, player.p_width, player.p_height};
        float dt = GetFrameTime();
        if (IsKeyDown(KEY_W)) player.position.y -= player.speed * dt;
        if (IsKeyDown(KEY_S)) player.position.y += player.speed * dt;
        if (IsKeyDown(KEY_A)) player.position.x -= player.speed * dt;
        if (IsKeyDown(KEY_D)) player.position.x += player.speed * dt;
        if (CheckCollisionCircleRec(sawblade1.sb_position, sawblade1.sb_radius, p_rect)){
            dead = true;
        }
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        DrawRectangleV( player.position, {player.p_width, player.p_height}, YELLOW);
        DrawCircleV(sawblade1.sb_position, sawblade1.sb_radius, RED);
        


        EndDrawing();
    }
    CloseWindow();
}   