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
    Rectangle player = {position.x, position.y, p_width, p_height};

};



enum GameStates {
    MAIN_MENU,
    SHOPMENU,
    GAME_OVER,
    PAUSED,
    GAME,
    WIN_SCREEN,
    MENU,
    RESTART,
    NEXT_LEVEL,
    QUIT
    
};

enum Levels { // i just had this genius idea of putting levels in an enum and then making a switch in GAME nice.
    Level1,
    Level2,
    Level3
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

Texture2D cross_button;
Texture2D cross_button_hovered;
GameStates gameState;
Levels levels;
bool onetimeloop = true;
void floating_window(Rectangle window_rect){
    
    
    DrawRectangleRec(window_rect, WHITE);
    Rectangle dest_rec = {window_rect.x + window_rect.width - 25, window_rect.y + 10.0f, 16.0f, 16.0f};
    Rectangle src_rec = {0, 0, 16, 16};
    DrawRectangleV({window_rect.x + 2.0f, window_rect.y + 2.0f}, {window_rect.width - 4.0f, window_rect.height - 4.0f}, BLACK);
    bool colliding = CheckCollisionPointRec(GetMousePosition(), dest_rec);
    DrawTexturePro(colliding ? cross_button_hovered : cross_button, src_rec, dest_rec, {0.0f, 0.0f}, 0, WHITE);
    if(colliding and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) gameState = MENU; // i will set this later to be LAST_GAMESTATE

}

std::vector<MovingSawblade> lvl1_sawblades{
    { {208.0, 304.0}, {144.0, 240.0}, {208.0, 304.0}, true, 30.0f, 16.0f},
    { {496.0, 48.0}, {496.0, 16.0}, {496.0, 48.0}, true, 30.0f, 16.0f},
    { {528.0, 80.0}, {560.0, 80.0}, {528.0, 80.0}, true, 30.0f, 16.0f},
    { {272.0, 144.0}, {272.0, 240.0}, {272.0, 144.0}, true, 100.0f, 16.0f},
    { {368.0, 240.0}, {368.0, 112.0}, {368.0, 240.0}, true, 100.0f, 16.0f}
};
std::vector<Block> lvl1_blocks{
    { {256.0, 256.0}, {384.0f, 160.0f}, GRAY },
    { {0.0, 0.0}, {352.0f, 128.0f}, GRAY },
    { {544.0, 0.0}, {96.0f, 256.0f}, GRAY },
    { {0.0, 128.0}, {64.0f, 288.0f}, GRAY },
    { {64.0, 128.0}, {96.0f, 96.0f}, GRAY },
    { {448.0, 160.0}, {96.0f, 96.0f}, GRAY },
    { {352.0, 0.0}, {192.0f, 32.0f}, GRAY },
    { {160.0, 352.0}, {96.0f, 64.0f}, GRAY },
    { {64.0, 384.0}, {96.0f, 32.0f}, GRAY },
    { {352.0, 32.0}, {64.0f, 32.0f}, GRAY },
    { {64.0, 224.0}, {64.0f, 32.0f}, GRAY },
    { {160.0, 128.0}, {64.0f, 32.0f}, GRAY },
    { {192.0, 320.0}, {64.0f, 32.0f}, GRAY },
    { {416.0, 192.0}, {32.0f, 64.0f}, GRAY },
    { {480.0, 128.0}, {64.0f, 32.0f}, GRAY },
    { {64.0, 256.0}, {32.0f, 32.0f}, GRAY },
    { {160.0, 160.0}, {32.0f, 32.0f}, GRAY },
    { {224.0, 288.0}, {32.0f, 32.0f}, GRAY },
    { {384.0, 224.0}, {32.0f, 32.0f}, GRAY },
    { {352.0, 64.0}, {32.0f, 32.0f}, GRAY }
};


void play_level(
    Player& player,
    std::vector<MovingSawblade>& movingsawblades,
    std::vector<Block>& blocks,
    Rectangle win,
    Texture2D player_icon,
    Texture2D bg,
    SFX& sfx,
    Vector2 spawn_point
    
){
    if(onetimeloop) player.position = spawn_point;
    onetimeloop = false;
    player.player.x = player.position.x;
    player.player.y = player.position.y;
    player.player.width = player.p_width;
    player.player.height = player.p_height;
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
        if(CheckCollisionCircleRec(saw.sb_pos_current, saw.sb_radius, player.player)){
            gameState = GAME_OVER; 
            if(!IsSoundPlaying(sfx.failsound)) PlaySound(sfx.failsound);

        }
    }
    if (IsKeyDown(KEY_W)) player.position.y -= player.speed * dt;
    if (IsKeyDown(KEY_S)) player.position.y += player.speed * dt;
    for(auto& block : blocks){
        Rectangle b_rect_y = {block.position.x, block.position.y, block.size.x, block.size.y};
        if (CheckCollisionRecs(player.player, b_rect_y)){
            if(IsKeyDown(KEY_W)) player.position.y += player.speed * dt;
            if(IsKeyDown(KEY_S)) player.position.y -= player.speed * dt;                                                
        }
    }
    if (IsKeyDown(KEY_A)) player.position.x -= player.speed * dt;
    if (IsKeyDown(KEY_D)) player.position.x += player.speed * dt;
    for(auto& block : blocks){
        Rectangle b_rect_x = {block.position.x, block.position.y, block.size.x, block.size.y};                        
        if (CheckCollisionRecs(player.player, b_rect_x)){
            if(IsKeyDown(KEY_A)) player.position.x += player.speed * dt;                            
            if(IsKeyDown(KEY_D)) player.position.x -= player.speed * dt;                                                            
        }

    }
    /////////////////////////////////////////////////////////////
    DrawTexture(bg, 0, 0, SKYBLUE);
    for(const auto& saw : movingsawblades) DrawCircleV(saw.sb_pos_current, saw.sb_radius, RED);
    for(const auto& block : blocks)DrawRectangleV(block.position, block.size, block.color);
    DrawTexture(player_icon, player.position.x, player.position.y, WHITE);
    DrawFPS(50, 10);
    DrawRectangleRec(win, GREEN);
    if(CheckCollisionRecs(player.player, win)) gameState = WIN_SCREEN;


}
 
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
    Rectangle A16_Texture_source = {0, 0, 16, 16};
    Rectangle A_32x32_texure_source = {0, 0, 32, 32};
    Rectangle A_128_64Texture_source = {0, 0, 128, 64};
    Rectangle menu_button_rect2 = {192.0, 256.0, 64.0f, 64.0f};
    Rectangle restart_button_rect2 = {288.0, 256.0, 64.0f, 64.0f};
    Rectangle next_level_button_rect2 = {384.0, 256.0, 64.0f, 64.0f};
    Rectangle play_button_rect = {32.0, 160.0, 96.0f, 32.0f};
    Rectangle settings_button_rect = {32.0, 208.0, 96.0f, 32.0f};
    Rectangle quit_button_rect = {32.0, 256.0, 96.0f, 32.0f};

    int width = 640;
    int height = 416; // changed res to be able todevide by 32/16 to use tiled
    std::string title = "Billy Ray V0.0.1"; //i was gonna include the version number in a variable but i am just gonna do it this way :p
    InitWindow(width, height, title.c_str());
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitAudioDevice();
    Player player;
    WinBlocks winblocks;
    player.position = {32.0, 304.0};
    MovingSawblade movingsawblade;
    gameState = GAME;
    bool dead = false;
    bool play = true;
    sfx.failsound = LoadSound("Assets/audio/sound effects/mixkit-wrong-answer-fail-notification-946.wav");
    Texture2D player_sprite = LoadTexture("Assets/Textures/Icon.png");
    Texture2D bg_lvl1 = LoadTexture("Assets/Textures/background.png");
    Texture2D menu_button = LoadTexture("Assets/Textures/gui/menu_button.png");
    Texture2D menu_button_hovered = LoadTexture("Assets/Textures/gui/menu_button_hovered.png");
    Texture2D restart_button = LoadTexture("Assets/Textures/gui/restart_button.png");
    Texture2D restart_button_hovered = LoadTexture("Assets/Textures/gui/restart_button_hovered.png");
    Texture2D next_level_button = LoadTexture("Assets/Textures/gui/next_level_button.png");
    Texture2D next_level_button_hovered = LoadTexture("Assets/Textures/gui/next_level_button_hovered.png");
    Texture2D play_button = LoadTexture("Assets/Textures/gui/play_button.png");
    Texture2D play_button_hovered = LoadTexture("Assets/Textures/gui/play_button_hovered.png");
    Texture2D settings_button = LoadTexture("Assets/Textures/gui/settings_button.png");
    Texture2D settings_button_hovered = LoadTexture("Assets/Textures/gui/settings_button_hovered.png");
    Texture2D quit_button = LoadTexture("Assets/Textures/gui/quit_button.png");
    Texture2D quit_button_hovered = LoadTexture("Assets/Textures/gui/quit_button_hovered.png");
              cross_button = LoadTexture("Assets/Textures/gui/cross_button.png");
              cross_button_hovered = LoadTexture("Assets/Textures/gui/cross_button_hovered.png");

    Image window_icon = LoadImage("Assets/Textures/icon.png");

    sfx.win_sound = LoadSound("Assets/audio/sound effects/mixkit-tile-game-reveal-960.wav");
    ImageFormat(&window_icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(window_icon);
    bool win_sound_played = false;
    bool win_screen_loaded = true;
    bool QUITCONFIRMED = false;
    Font Lato = LoadFontEx("Assets/Fonts/Lato/Lato-Regular.ttf", 30, 0, 0);

    while(WindowShouldClose() == false and !QUITCONFIRMED){
        BeginDrawing();
        Vector2 mouse_pos = GetMousePosition();
        Rectangle p_rect = {player.position.x, player.position.y, player.p_width, player.p_height};
        switch(gameState){
            case GAME: {
                switch (levels){
                    case Level1: {
                        play_level(
                            player,
                            lvl1_sawblades,
                            lvl1_blocks,
                            {512.0f, 32.0f, 32.0f, 32.0f},
                            player_sprite,
                            bg_lvl1,
                            sfx,
                            {96.0, 320.0}
                        );
                        
                            
                        
                    }
                }
                break;
            }
            case WIN_SCREEN: {
                DrawRectangle(0, 0, 640.0f, 416.0f, BLACK);
                
                if (win_sound_played == false){
                    PlaySound(sfx.win_sound);
                    win_sound_played = true;
                }

                DrawTexturePro(CheckCollisionPointRec(mouse_pos, menu_button_rect2) ? menu_button_hovered : menu_button, A_32x32_texure_source, menu_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, menu_button_rect2)) gameState = MENU;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, restart_button_rect2) ? restart_button_hovered : restart_button, A_32x32_texure_source, restart_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, restart_button_rect2)) gameState = RESTART;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, next_level_button_rect2) ? next_level_button_hovered : next_level_button, A_32x32_texure_source, next_level_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, next_level_button_rect2)) gameState = NEXT_LEVEL;
                

                
                break;
            }
            case GAME_OVER: {
                
                ClearBackground(BLACK);
                const char* gameover = "GAME OVER!";
                DrawText(gameover, 65.0f, height/3.0f, 80.0f, WHITE);
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, {224.0, 256.0, 64.0f, 64.0f}) ? menu_button_hovered : menu_button, A_32x32_texure_source, {224.0, 256.0, 64.0f, 64.0f}, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, {224.0, 256.0, 64.0f, 64.0f})) gameState = MENU;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, {352.0, 256.0, 64.0f, 64.0f}) ? restart_button_hovered : restart_button, A_32x32_texure_source, {352.0, 256.0, 64.0f, 64.0f}, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, {352.0, 256.0, 64.0f, 64.0f})) gameState = RESTART;
                
                break;
            }
            case RESTART: {
                onetimeloop = true;
                for(auto& saw : movingsawblades){
                saw.sb_pos_current = saw.sb_pos_a;
                
                }
                gameState = GAME;

                break;
            }
            case MENU: {
                
                ClearBackground(GRAY);
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, play_button_rect) ? play_button_hovered : play_button, A_128_64Texture_source, play_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, play_button_rect)) gameState = GAME;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, settings_button_rect) ? settings_button_hovered : settings_button, A_128_64Texture_source, settings_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, settings_button_rect));
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, quit_button_rect) ? quit_button_hovered : quit_button, A_128_64Texture_source, quit_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, quit_button_rect)) gameState = QUIT;
                break;
            }
            case QUIT: {
                floating_window({195.5f, 180.f, 249.0f, 100.0f});
                
                DrawTextPro(Lato, "Press Enter to Quit.", {269.0f, 225.0f}, {MeasureTextEx(GetFontDefault(), "Press Enter to Quit.", 15.0f, 0.0f).x/2.0f, 0.0f}, 0.0f, 28, 2.0f, WHITE);
                if(IsKeyPressed(KEY_ENTER)) QUITCONFIRMED = true;
                if(IsKeyPressed(KEY_ESCAPE)) gameState = MENU;         
                
                break;
            }
        
        }

        EndDrawing();
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    UnloadTexture(menu_button);
    UnloadTexture(menu_button_hovered);
    UnloadTexture(restart_button_hovered);
    UnloadTexture(restart_button);
    UnloadTexture(next_level_button);
    UnloadTexture(next_level_button_hovered);
    UnloadTexture(cross_button);
    UnloadTexture(cross_button_hovered);
    UnloadFont(Lato);


    UnloadTexture(bg_lvl1);
    UnloadImage(window_icon); 
    UnloadTexture(player_sprite);
    UnloadSound(sfx.failsound);
    UnloadSound(sfx.win_sound);

    CloseAudioDevice();
    CloseWindow();

}
