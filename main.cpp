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
    float p_width = 32.0f; // people always use this as a constant but i am just gonna make it a variable because i might add a power up that changes the scale
    float p_height = 32.0f;
    float speed = 150.0f;

};



enum GameStates {
    MAIN_MENU,
    SHOPMENU,
    GAME_OVER,
    PAUSED,
    GAME,
    WIN_SCREEN,
    MENU
};


struct Block {
    Vector2 position;
    Vector2 size;
    Color color;
};


struct WinBlocks {
    Vector2 position = {32.0, 96.0};
    Vector2 size = {32.0f, 32.0f};
    Color color = GREEN;
};


struct SFX {
    Sound failsound;
    Sound win_sound;
};


int main(int argc, char* argv[]) {
    SFX sfx;




    std::vector<MovingSawblade> movingsawblades {
        { {112.0, 272.0}, {496.0, 272.0}, {112.0, 272.0}, true, 280.0f, 16.0f},
        { {496.0, 368.0}, {112.0, 368.0}, {496.0, 368.0}, true, 280.0f, 16.0f},
        { {144.0, 272.0}, {144.0, 368.0}, {144.0, 272.0}, true, 40.0f, 16.0f},
        { {560.0, 30.0}, {560.0, 336.0}, {560.0, 80.0}, true, 80.0f, 48.0f},
        { {432.0, 48.0}, {368.0, 144.0}, {432.0, 48.0}, true, 100.0f, 16.0f},
        { {368.0, 144.0}, {304.0, 48.0}, {368.0, 144.0}, true, 100.0f, 16.0f},
        { {304.0, 48.0}, {240.0, 144.0}, {304.0, 48.0}, true, 100.0f, 16.0f},
        { {240.0, 144.0}, {176.0, 48.0}, {240.0, 144.0}, true, 100.0f, 16.0f},
        { {144.0, 48.0}, {144.0, 144.0}, {144.0, 48.0}, true, 100.0f, 16.0f},
        { {112.0, 144.0}, {112.0, 48.0}, {112.0, 144.0}, true, 100.0f, 16.0f}
    };

    std::vector<Block> blocks {
        { {0.0, 160.0}, {512.0f, 96.0f}, GRAY },
        { {0.0, 384.0}, {640.0f, 32.0f}, GRAY },
        { {0.0, 0.0}, {640.0f, 32.0f}, GRAY },
        { {608.0, 32.0}, {32.0f, 352.0f}, GRAY },
        { {0.0, 32.0}, {32.0f, 128.0f}, GRAY },
        { {0.0, 256.0}, {32.0f, 128.0f}, GRAY }
    };


    int width = 640;
    int height = 416; // changed res to be able todevide by 32/16 to use tiled
    std::string title = "Billy Ray V0.0.1"; //i was gonna include the version number in a variable but i am just gonna do it this way :p
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);
    InitAudioDevice();
    Player player;
    WinBlocks winblocks;
    player.position = {32.0, 304.0};
    MovingSawblade movingsawblade;
    GameStates gameState = GAME;
    bool dead = false;
    bool play = true;
    bool prevent_memory_leak_001 = true;
    bool prevent_memory_leak_002 = true;
    sfx.failsound = LoadSound("Assets/audio/sound effects/mixkit-wrong-answer-fail-notification-946.wav");
    Texture2D player_sprite = LoadTexture("Assets/Textures/Icon.png");
    Texture2D bg_lvl1 = LoadTexture("Assets/Textures/background.png");
    Image window_icon = LoadImage("Assets/Textures/icon.png");
    sfx.win_sound = LoadSound("Assets/audio/sound effects/mixkit-tile-game-reveal-960.wav");
    ImageFormat(&window_icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(window_icon);
    bool win_sound_played = false;
    bool win_screen_loaded = true;
    while(WindowShouldClose() == false){
        BeginDrawing();
        Vector2 mouse_pos = GetMousePosition();
        Rectangle p_rect = {player.position.x, player.position.y, player.p_width, player.p_height};
        switch(gameState){
            case GAME: {
                if(dead == false and play){
                    Rectangle w_rect = {winblocks.position.x, winblocks.position.y, winblocks.size.x, winblocks.size.y};
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

                    if (IsKeyDown(KEY_W)) player.position.y -= player.speed * dt;
                    if (IsKeyDown(KEY_S)) player.position.y += player.speed * dt;
                    p_rect.y = player.position.y;



                    for(auto& block : blocks){
                        Rectangle b_rect_y = {block.position.x, block.position.y, block.size.x, block.size.y};
                        if (CheckCollisionRecs(p_rect, b_rect_y)){
                            if(IsKeyDown(KEY_W)){
                                player.position.y += player.speed * dt;
                            }
                            if (IsKeyDown(KEY_S)){
                                player.position.y -= player.speed * dt;

                            }


                        }
                    }

                    
                    if (IsKeyDown(KEY_A)) player.position.x -= player.speed * dt;
                    if (IsKeyDown(KEY_D)) player.position.x += player.speed * dt;
                    p_rect.x = player.position.x;
                    for(auto& block : blocks){
                        Rectangle b_rect_x = {block.position.x, block.position.y, block.size.x, block.size.y};
                        
                        if (CheckCollisionRecs(p_rect, b_rect_x)){
                            if(IsKeyDown(KEY_A)){
                                player.position.x += player.speed * dt;

                            }
                            if(IsKeyDown(KEY_D)){
                                player.position.x -= player.speed * dt;
                            }
                        }

                    }





                    DrawTexture(bg_lvl1, 0, 0, SKYBLUE);

                    DrawFPS(50, 10);



                    for(const auto& saw : movingsawblades){
                        DrawCircleV(saw.sb_pos_current, saw.sb_radius, RED);

                    }
                    for(const auto& block : blocks){
                        DrawRectangleV(block.position, block.size, block.color);
                    }
                    DrawTexture(player_sprite, player.position.x, player.position.y, WHITE);
                    DrawFPS(50, 10);
                    DrawRectangleV(winblocks.position, winblocks.size, winblocks.color);
                    if(CheckCollisionRecs(p_rect, w_rect)){
                        win_screen_loaded = false;
                        gameState = WIN_SCREEN;
                    }

                }
                
                else if(dead){
                    DrawRectangle(0, 0, width, height, BLACK);
                    const char* gameover = "GAME OVER!";
                    DrawText(gameover, 65.0f, height/3.0f, 80.0f, WHITE);
                }
                if(IsKeyDown(KEY_R)){
                    player.position = {32.0, 304.0};
                    dead = false;
                    
                    for(auto& saw : movingsawblades){
                        saw.sb_pos_current = saw.sb_pos_a;

                    }
                }
                break;
            }
            case WIN_SCREEN: {
                DrawRectangle(0, 0, 640.0f, 416.0f, BLACK);
                Texture2D menu_button;
                Texture2D next_level_button;
                Texture2D restart_button;
                Rectangle restart_button_rect;
                Rectangle menu_button_rect;
                Rectangle next_level_button_rect;
                Rectangle restart_button_rect2;
                Rectangle menu_button_rect2;
                Rectangle next_level_button_rect2;
                
                if (win_screen_loaded == false) {
                    menu_button = LoadTexture("Assets/Textures/gui/menu_button.png");
                    next_level_button = LoadTexture("Assets/Textures/gui/next_level_button.png");
                    restart_button = LoadTexture("Assets/Textures/gui/restart_button.png");
                    restart_button_rect = {0, 0, 32, 32};
                    menu_button_rect = {0, 0, 32, 32};
                    menu_button_rect2 = {192.0, 256.0, 64.0f, 64.0f};
                    next_level_button_rect = {0, 0, 32, 32};
                    restart_button_rect2 = {288.0, 256.0, 64.0f, 64.0f};
                    next_level_button_rect2 = {384.0, 256.0, 64.0f, 64.0f};
                    win_screen_loaded = true;
                    
                }
                if (win_sound_played == false){
                    PlaySound(sfx.win_sound);
                    win_sound_played = true;
                }
                
                if (CheckCollisionPointRec(mouse_pos, menu_button_rect2)){
                    if(prevent_memory_leak_001 == true){
                        UnloadTexture(menu_button);
                        menu_button = LoadTexture("Assets/Textures/gui/menu_button_hovered.png");
                        prevent_memory_leak_002 = true;
                        prevent_memory_leak_001 = false;
                    }
                    DrawTexturePro(menu_button, menu_button_rect, menu_button_rect2, {0, 0}, 0.0f, WHITE);
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        gameState = MENU;
                    }
                

                    
                }
                else{
                    if (prevent_memory_leak_002 == true){
                        UnloadTexture(menu_button);
                        menu_button = LoadTexture("Assets/Textures/gui/menu_button.png");
                        prevent_memory_leak_001 = true;
                        prevent_memory_leak_002 = false;
                    }
                    DrawTexturePro(menu_button, menu_button_rect, menu_button_rect2, {0, 0}, 0.0f, WHITE);
                }

                
                

                
                break;
            }
        }
        EndDrawing();
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    UnloadTexture(bg_lvl1);
    UnloadImage(window_icon); 
    UnloadTexture(player_sprite);
    UnloadSound(sfx.failsound);
    UnloadSound(sfx.win_sound);

    CloseAudioDevice();
    CloseWindow();

}
