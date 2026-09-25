#include <raylib.h>
#include <iostream>
#include <string>
#include <vector>
#include <raymath.h>
struct MovingSawblade {

    Vector2 sb_pos_a;
    Vector2 sb_pos_b;
    Vector2 sb_pos_current;
    bool sb_direction;
    float sb_speed;
    float sb_radius = 10.0f;
};


struct SawBlade {
    Vector2 sb_position;
    float radius;
    float rotation_speed; //making this will probably be complicated but not now :/
};


struct Player {
    Vector2 position;
    float p_width = 40.0f; // people always use this as a constant but i am just gonna make it a variable because i might add a power up that changes the scale
    float p_height = 40.0f;
    float speed = 150.0f;

};

int main(int argc, char* argv[]) {


    std::vector<MovingSawblade> movingsawblades {
        { {50.0f, 50.0f}, {100.0f, 00.0f}, {50.0f, 50.0f}, true, 180.0f, 20.0f},
        { {50.0f, 50.0f}, {250.0f, 100.0f}, {50.0f, 50.0f}, true, 180.0f, 20.0f},
    };



    int width = 600;
    int height = 400;
    std::string title = "Billy Ray V0.0.1"; //i was gonna include the version number in a variable but i am just gonna do it this way :p
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    Player player;
    player.position = {width/2.0f, height/2.0f}; 
    MovingSawblade movingsawblade;
    bool dead = false;
    while(WindowShouldClose() == false && dead == false){
        Rectangle p_rect = {player.position.x, player.position.y, player.p_width, player.p_height};
        float dt = GetFrameTime();
        for(auto& saw : movingsawblades){
            Vector2 target;
            if(saw.sb_direction){
                target = saw.sb_pos_b;
            }
            else{
                target = saw.sb_pos_a;
            }

            saw.sb_pos_current = Vector2MoveTowards(saw.sb_pos_current, target, saw.sb_speed * dt);
            if(Vector2Distance(saw.sb_pos_current, target) < 1.0f){ // needed some help from our friend gemini here, it gave me the idea of using vector2move and vector2distance
                saw.sb_direction = !saw.sb_direction;
            }

            if(CheckCollisionCircleRec(saw.sb_pos_current, saw.sb_radius, p_rect)){
                dead = true;
            }



        }
        
        
        
        if (IsKeyDown(KEY_W)) player.position.y -= player.speed * dt;
        if (IsKeyDown(KEY_S)) player.position.y += player.speed * dt;
        if (IsKeyDown(KEY_A)) player.position.x -= player.speed * dt;
        if (IsKeyDown(KEY_D)) player.position.x += player.speed * dt;
        
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        DrawRectangleV( player.position, {player.p_width, player.p_height}, YELLOW);
        for(const auto& saw : movingsawblades){
            DrawCircleV(saw.sb_pos_current, saw.sb_radius, RED);
        }
        


        EndDrawing();
    }
    CloseWindow();
}   