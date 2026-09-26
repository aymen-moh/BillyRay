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
    float sb_radius;
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
struct Block {

};

struct SFX {
    Sound failsound;
};
int main(int argc, char* argv[]) {
    SFX sfx;
    
    std::vector<MovingSawblade> movingsawblades {
        { {112.0, 272.0}, {496.0, 272.0}, {112.0, 272.0}, true, 280.0f, 16.0f},
        { {496.0, 368.0}, {112.0, 368.0}, {496.0, 368.0}, true, 280.0f, 16.0f},
        { {144.0, 272.0}, {144.0, 368.0}, {144.0, 272.0}, true, 40.0f, 16.0f},
        { {560.0, 80.0}, {560.0, 336.0}, {560.0, 80.0}, true, 120.0f, 48.0f},
        { {432.0, 48.0}, {496.0, 144.0}, {432.0, 48.0}, true, 100.0f, 16.0f},
        { {432.0, 48.0}, {368.0, 144.0}, {432.0, 48.0}, true, 100.0f, 16.0f},
        { {368.0, 144.0}, {304.0, 48.0}, {368.0, 144.0}, true, 100.0f, 16.0f},
        { {304.0, 48.0}, {240.0, 144.0}, {304.0, 48.0}, true, 100.0f, 16.0f},
        { {240.0, 144.0}, {176.0, 48.0}, {240.0, 144.0}, true, 100.0f, 16.0f},
        { {144.0, 48.0}, {144.0, 144.0}, {144.0, 48.0}, true, 100.0f, 16.0f},
        { {112.0, 144.0}, {112.0, 48.0}, {112.0, 144.0}, true, 100.0f, 16.0f}
    };



    int width = 640;
    int height = 416; // changed res to be able todevide by 32/16 to use tiled
    std::string title = "Billy Ray V0.0.1"; //i was gonna include the version number in a variable but i am just gonna do it this way :p
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);
    InitAudioDevice();
    Player player;
    player.position = {width/2.0f, height/2.0f}; 
    MovingSawblade movingsawblade;
    bool dead = false;
    sfx.failsound = LoadSound("F:\\Desktop\\Projects\\BillyRay\\Assets\\audio\\sound effects\\mixkit-wrong-answer-fail-notification-946.wav");
    
    while(WindowShouldClose() == false){
        BeginDrawing();
        if(dead == false){
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
                    if(!IsSoundPlaying(sfx.failsound)) PlaySound(sfx.failsound);
                    
                    
                    
                }



            }
            
            
            float velocity;
            if (IsKeyDown(KEY_W)){
                player.position.y -= player.speed * dt;

            };
            
            if (IsKeyDown(KEY_S)){
                player.position.y += player.speed * dt;
                velocity = 5;
            };
            if (IsKeyDown(KEY_A)) {
                player.position.x -= player.speed * dt;
                velocity = 5;
            };
            if (IsKeyDown(KEY_D)) {
                player.position.x += player.speed * dt;
                velocity = 5;
            };
            
            
            ClearBackground(DARKGRAY);
            
            DrawText(std::to_string(54).c_str(), 50.0f, 50.0f, 50.0f, WHITE);

            
            DrawRectangleV( player.position, {player.p_width, player.p_height}, SKYBLUE);
            
            for(const auto& saw : movingsawblades){
                DrawCircleV(saw.sb_pos_current, saw.sb_radius, RED);
            }
            


            
        }
        else if(dead){
            DrawRectangle(0, 0, width, height, BLACK);
            const char* gameover = "GAME OVER!";
            DrawText(gameover, 65.0f, height/3.0f, 80.0f, WHITE);
        }
        if(IsKeyDown(KEY_R) and dead){
            player.position = {width/2.0f, height/2.0f};
            dead = false;
        }
        EndDrawing();

    }
    
    UnloadSound(sfx.failsound);
    CloseAudioDevice();
    CloseWindow();
}   