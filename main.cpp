#include <raylib.h>
#include <iostream>
#include <string>
#include <vector>
#include <raymath.h>
#include "levels.h"



Font Lato;
bool music_enabled = true;
bool sfx_enabled = true;
bool admin = false; 
float sfx_volume = 1.0f;
Color LIGHTBLUE = {0, 255, 255, 255};
bool rst_notf_sd = false;
int st_menu = 1;
struct SawBlade {
    Vector2 sb_position;
    float radius;
    float rotation_speed; //making this will probably be complicated but not now :/
};
struct Player {
    Vector2 position;
    float p_width = 29.0f; // people always use this as a constant but i am just gonna make it a variable because i might add a power up that changes the scale
    float p_height = 29.0f;
    float speed = 120.0f;
    Rectangle player = {position.x, position.y, p_width, p_height};

};

struct spear {
    
};

enum GameStates {
    MAIN_MENU,
    SHOPMENU,
    GAME_OVER,
    PAUSED,
    GAME,
    SETTINGS,
    WIN_SCREEN,
    MENU,
    RESTART,
    NEXT_LEVEL,
    QUIT, 
    LEVEL_MENU
    
};





struct WinBlocks {
    Vector2 position = {32.0, 96.0};
    Vector2 size = {32.0f, 32.0f};
    Color color = GREEN;
};


struct SFX {

    Sound failsound;
    Sound win_sound;
    Sound notification_sound;
    Sound notification_sound_out;
    Sound power_up;
};

SFX sfx;
Texture2D cross_button;
Texture2D cross_button_hovered;
Texture2D shrinking_potion;
Texture2D plate_button;
Texture2D plate_button_hovered;
Texture2D checkbox;
Texture2D checkbox_hovered;
Texture2D checkbox_toggled_hovered;
Texture2D checkbox_toggled;
Texture2D knob_hovered;
Texture2D knob;
Texture2D slider_bar;
Texture2D slider_bar_side; // idk if i can flip it
GameStates gameState;
int level = 0;
bool onetimeloop = true;
bool confirm_restart = false;
bool shrunk = false;
int max_unlocked_level = 1;
static Rectangle default_potion = {1000.0f, 1000.0f, 0.0f, 0.0f};

void toggle(Vector2 position, bool& toggled, std::string toggle_text){
    Rectangle toggle_rec = {position.x, position.y, 16, 16};
    Rectangle TEXTURE8X8 = {0, 0, 8, 8};
    bool hovered = CheckCollisionPointRec(GetMousePosition(), toggle_rec);
    DrawTexturePro(hovered ? toggled ? checkbox_toggled_hovered : checkbox_hovered : toggled ? checkbox_toggled : checkbox, TEXTURE8X8, toggle_rec, {0, 0}, 0, WHITE);
    if(hovered and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        toggled = !toggled;
    }
    DrawText(toggle_text.c_str(), position.x + 25, position.y + 1, 16, WHITE);
}

void slider(Vector2 position, int length, float& value, bool locked, float devide_value_by){
    
    Vector2 knob_pos = {position.x + value * devide_value_by, position.y};
    static Rectangle src = {0, 0, 8, 8};
    static bool drag = false;
    Rectangle dst = {knob_pos.x, knob_pos.y, 16, 16};
    Vector2 md = GetMouseDelta();
    Vector2 mp = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mp, dst);
    if(hovered and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) drag = true;
    if(!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) drag = false;
    if(drag and !locked){
        knob_pos.x += md.x;
        knob_pos.x = Clamp(knob_pos.x, position.x, position.x + length);
        value = (knob_pos.x - position.x) / devide_value_by;

    }
    DrawTexturePro(slider_bar_side, {0, 0, -8, 8}, {position.x + 5, position.y + 4, 8, 8}, {0, 0}, 0, WHITE);
    DrawTexturePro(slider_bar_side, src, {position.x + length + 5, position.y + 4, 8, 8}, {0, 0}, 0, WHITE);
    DrawTexturePro(slider_bar, src, {position.x + 12, position.y + 4, length - 6, 8}, {0, 0}, 0, WHITE);
    DrawTexturePro(knob, src, dst, {0, 0}, 0, locked ? WHITE : drag ? BLUE : GRAY);

}
void slider_toggle(
    Vector2 position, 
    bool& toggled, 
    std::string toggle_text, 
    float& value, 
    int length, 
    float dvb,
    bool disappear_if_untoggled = false, 
    bool lock_if_untoggled = true,
    bool set_to_zero_if_untoggled = false,
    bool lock_if_set_to_0_and_untoggled = false,
    bool untoggle_if_set_to_0 = false
){
    float last_value = value;
    float slider_x = position.x + 25 + MeasureText(toggle_text.c_str(), 16) + 15;
    toggle(position, toggled, toggle_text);
    if(set_to_zero_if_untoggled and !toggled){
        value = 0.0;
        slider({slider_x, position.y}, length, value, true, dvb);
    }
    else if (set_to_zero_if_untoggled and toggled){
        value = last_value;
        slider({slider_x, position.y}, length, value, false, dvb);
    }
    if(disappear_if_untoggled and !toggled){
    }
    else if (lock_if_untoggled and !toggled){
        slider({slider_x, position.y}, length, value, true, dvb);
    }

    
    
    


}
void floating_window(Rectangle window_rect, bool line = false, int sections = 0){

    DrawRectangleRec(window_rect, WHITE);
    Rectangle dest_rec = {window_rect.x + window_rect.width - 25, window_rect.y + 10.0f, 16.0f, 16.0f};
    Rectangle src_rec = {0, 0, 16, 16};
    DrawRectangleV({window_rect.x + 2.0f, window_rect.y + 2.0f}, {window_rect.width - 4.0f, window_rect.height - 4.0f}, BLACK);
    bool colliding = CheckCollisionPointRec(GetMousePosition(), dest_rec);
    DrawTexturePro(colliding ? cross_button_hovered : cross_button, src_rec, dest_rec, {0.0f, 0.0f}, 0, WHITE);
    if(colliding and IsMouseButtonDown(MOUSE_BUTTON_LEFT)) gameState = MENU; // i will set this later to be LAST_GAMESTATE
    if (sections > 0){
        double fontsize = 18.0; 
        std::string section1 = "Audio";
        Vector2 sec1_size = MeasureTextEx(GetFontDefault(), section1.c_str(), 24, 2.0f);
        Rectangle sections_button_rect = {window_rect.x + 20, window_rect.y + 50, sec1_size.x, sec1_size.y};
        DrawTexturePro(CheckCollisionPointRec(GetMousePosition(), sections_button_rect) ? plate_button_hovered : plate_button, {0, 0, 32, 32}, sections_button_rect, {0, 0}, 0, WHITE);
        DrawTextPro(GetFontDefault(), section1.c_str(), {sections_button_rect.x + 7, sections_button_rect.y + 3}, {0, 0}, 0, fontsize, 2.0f, WHITE);
        if(CheckCollisionPointRec(GetMousePosition(), sections_button_rect) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) st_menu = 1;

        std::string section2 = "Controls";
        Vector2 sec2_size = MeasureTextEx(GetFontDefault(), section2.c_str(), 24, 2.0f);
        Rectangle sections2_button_rect = {sections_button_rect.x + sections_button_rect.width + 10, window_rect.y + 50, sec2_size.x, sec2_size.y};
        DrawTexturePro(CheckCollisionPointRec(GetMousePosition(), sections2_button_rect) ? plate_button_hovered : plate_button, {0, 0, 32, 32}, sections2_button_rect, {0, 0}, 0, WHITE);
        DrawTextPro(GetFontDefault(), section2.c_str(), {sections2_button_rect.x + 10, sections2_button_rect.y + 3}, {0, 0}, 0, fontsize, 2.0f, WHITE);
        if(CheckCollisionPointRec(GetMousePosition(), sections2_button_rect) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) st_menu = 2;


        std::string section3 = "Other";
        Vector2 sec3_size = MeasureTextEx(GetFontDefault(), section2.c_str(), 24, 2.0f);
        Rectangle sections3_button_rect = {sections2_button_rect.x + sections2_button_rect.width + 10, window_rect.y + 50, sec3_size.x, sec3_size.y};
        DrawTexturePro(CheckCollisionPointRec(GetMousePosition(), sections3_button_rect) ? plate_button_hovered : plate_button, {0, 0, 32, 32}, sections3_button_rect, {0, 0}, 0, WHITE);
        DrawTextPro(GetFontDefault(), section3.c_str(), {sections3_button_rect.x + 10, sections3_button_rect.y + 3}, {0, 0}, 0, fontsize, 2.0f, WHITE);
        if(CheckCollisionPointRec(GetMousePosition(), sections3_button_rect) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) st_menu = 3;
    }
    if(line){
        DrawLine(window_rect.x, window_rect.y + 35, window_rect.x + window_rect.width, window_rect.y + 35, WHITE);
    }
    

}

void push_notification(std::string text, float duration, int pos_change, float anim_speed, bool reset){
    float init_current_y = -30.0f;
    static Vector2 current = {320.0f, init_current_y};
    static float time_spent = 0.0f;
    static bool play_end_animation = false;
    static bool bool1 = true;
    static bool bool2 = true;
    static bool play_start_animation = true;
    if(reset){
        Vector2 current = {320.0f, init_current_y};
        float time_spent = 0.0f;
        bool play_end_animation = false;
        bool bool1 = true;
        bool bool2 = true;
        bool play_start_animation = true;
    }
    float dt = GetFrameTime();
    if(play_start_animation){
        if (bool1){
            bool1 = false;
            if (!IsSoundPlaying(sfx.notification_sound)) PlaySound(sfx.notification_sound);

        }
        if (current.y < init_current_y + pos_change and play_start_animation) {
            current.y += anim_speed;
        }
        else{
            current.y = init_current_y + pos_change;

        }        
        if(current.y >= init_current_y + pos_change){
            time_spent += GetFrameTime();
        }
        if(time_spent > duration){
            play_start_animation = false;
            play_end_animation = true;
        }
    }
    if(play_end_animation){
        if (bool2){
            if(!IsSoundPlaying(sfx.notification_sound_out)) PlaySound(sfx.notification_sound_out);
            bool2 = false;
        }
        
        if (current.y > init_current_y) current.y -= anim_speed;
    }
    DrawText(text.c_str(), current.x, current.y, 24, WHITE);
    
    
}

void shrink(Player& player, double amplifier, bool& play){
    
    static float org_player_height = player.p_height;
    static float org_player_width =  player.p_width;
    if(player.p_height > org_player_height / amplifier) player.p_height -= 0.3f;
    if(player.p_width > org_player_width / amplifier) player.p_width -= 0.3f;
    if (!IsSoundPlaying(sfx.power_up) and !play) PlaySound(sfx.power_up);
    play = true;

}

void disappear(Texture2D disappear, Rectangle& disappearer){
    disappear.height = 0;
    disappear.width = 0;
    disappearer = {-200, -200, 0, 0};
}




void play_level(
    Player& player,
    std::vector<MovingSawblade>& movingsawblades,
    std::vector<Block>& blocks,
    Rectangle win,
    Texture2D player_icon,
    Texture2D bg,
    SFX& sfx,
    Vector2 spawn_point,
    Texture2D sawblade,
    Rectangle& shrink_potion = default_potion
    
){
    Rectangle org_shrink = shrink_potion;
    static bool shrunk = false;
    static bool shrinking_sound = true;
    static bool disappear = false;
    
    if(onetimeloop){
        shrink_potion = org_shrink;
        shrinking_sound = false;
        shrunk = false;
        shrink_potion = org_shrink;
        player.p_height = 29.0f;
        player.p_width = 29.0f;
        player.position = spawn_point;
        for (auto& saw : movingsawblades) saw.sb_pos_current = saw.sb_pos_a;
        rst_notf_sd = true;
    }
    onetimeloop = false;
    
    player.player.x = player.position.x;
    player.player.y = player.position.y;
    player.player.width = player.p_width;
    player.player.height = player.p_height;
    float dt = GetFrameTime();
    Rectangle next_step = player.player;
    Vector2 next_pos = player.position;
    Rectangle b_rect;
    if(IsKeyDown(KEY_W)) next_pos.y -= player.speed *dt;
    if(IsKeyDown(KEY_A)) next_pos.x -= player.speed *dt;
    if(IsKeyDown(KEY_S)) next_pos.y += player.speed *dt;
    if(IsKeyDown(KEY_D)) next_pos.x += player.speed *dt;
    next_step = {next_pos.x, next_pos.y, player.p_width, player.p_height};
    bool collision = false;
    float rotation = 0.0f;
    for(auto& block : blocks){
        Rectangle b_rect = {block.position.x, block.position.y, block.size.x, block.size.y};
        if(CheckCollisionRecs(next_step, b_rect)){
            collision = true;
            break;
        }
    }
    if(!collision) player.position = next_pos;
    for(auto& saw : movingsawblades){
        (saw.rotation >= 360) ?  saw.rotation = 0 : saw.rotation += 360 * dt;
        Vector2 target;
        if(saw.sb_direction){
            target = saw.sb_pos_b;
        }
        else{
            target = saw.sb_pos_a;
        }
        saw.sb_pos_current = Vector2MoveTowards(saw.sb_pos_current, target, saw.sb_speed * dt);
        if(Vector2Distance(saw.sb_pos_current,target) < 1.0f){ // needed some help from our friend gemini here, it gave me the idea of using vector2move and vector2distance
            saw.sb_direction = !saw.sb_direction;
        }
        if(CheckCollisionCircleRec(saw.sb_pos_current, saw.sb_radius, player.player)){
            gameState = GAME_OVER; 
            if(!IsSoundPlaying(sfx.failsound)) PlaySound(sfx.failsound);

        }
    }
    if(IsKeyPressed(KEY_ESCAPE)) gameState = PAUSED;
    

    if(CheckCollisionRecs(player.player, shrink_potion)){
        shrink_potion = {-200, -200, 0, 0};
        shrunk = true;
    }
    if (shrunk) shrink(player, 1.5, shrinking_sound);
    /////////////////////////////////////////////////////////////
    
    DrawTexture(bg, 0, 0, SKYBLUE);
    for(const auto& block : blocks)DrawRectangleV(block.position, block.size, block.color);
    for(const auto& saw : movingsawblades){
        Rectangle src_rec = {0.0f, 0.0f, static_cast<float>(sawblade.width), static_cast<float>(sawblade.height)};
        Rectangle dest_rec = {saw.sb_pos_current.x, saw.sb_pos_current.y, saw.sb_radius * 2.3f, saw.sb_radius * 2.3f};
        DrawTexturePro(sawblade, src_rec, dest_rec, {dest_rec.width / 2.0f, dest_rec.height / 2.0f}, saw.rotation, WHITE);
    }   

    
        
        

    
    DrawTexturePro(player_icon, {0, 0, 32, 32}, player.player, {0, 0}, 0, WHITE);
    DrawFPS(50, 10);
    DrawRectangleRec(win, GREEN);
    if(CheckCollisionRecs(player.player, win)) gameState = WIN_SCREEN;
    push_notification("hi hru", 2.0f, 100.0f, 2.0f, rst_notf_sd);
    DrawTexturePro(shrinking_potion, {0, 0, 16, 16}, shrink_potion, {0, 0}, 0, LIME);
    
}
 
int main(int argc, char* argv[]) {

    
    



    
    std::vector<MovingSawblade> movingsawblades {
        
    };

    std::vector<Block> blocks {
        
    };
    Rectangle A16_Texture_source = {0, 0, 16, 16};
    Rectangle A_32x32_texure_source = {0, 0, 32, 32};
    Rectangle A_128_64Texture_source = {0, 0, 128, 64};
    Rectangle menu_button_rect2 = {192.0, 256.0, 64.0f, 64.0f};
    Rectangle restart_button_rect2 = {288.0, 256.0, 64.0f, 64.0f};
    Rectangle next_level_button_rect2 = {384.0, 256.0, 64.0f, 64.0f};
    Rectangle resume_button_rect2 = {384.0, 256.0, 64.0f, 64.0f};
    Rectangle play_button_rect = {32.0, 160.0, 96.0f, 32.0f};
    Rectangle settings_button_rect = {32.0, 208.0, 96.0f, 32.0f};
    Rectangle shop_button_rect = {32.0, 256.0, 96.0f, 32.0f}; 
    Rectangle quit_button_rect = {32.0, 304.0, 96.0f, 32.0f}; 

 
    int width = 640;
    int height = 416; // changed res to be able todevide by 32/16 to use tiled
    std::string title = "Billy Ray V0.0.1"; //i was gonna include the version number in a variable but i am just gonna do it this way :p
    InitWindow(width, height, title.c_str());
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitAudioDevice();
    sfx.notification_sound = LoadSound("Assets/audio/sound effects/Toast.ogg");
    sfx.notification_sound_out = LoadSound("Assets/audio/sound effects/Out.ogg");
    sfx.power_up = LoadSound("Assets/audio/sound effects/edr-power-up-01a-484722.mp3"); 
    sfx.win_sound = LoadSound("Assets/audio/sound effects/mixkit-tile-game-reveal-960.wav"); 
    Player player;
    WinBlocks winblocks;
    player.position = {32.0, 304.0};
    MovingSawblade movingsawblade;
    gameState = MENU;
    bool dead = false;
    bool play = true;
    sfx.failsound = LoadSound("Assets/audio/sound effects/mixkit-wrong-answer-fail-notification-946.wav");
    Texture2D sawblade = LoadTexture("Assets/Textures/sawblade.png");
    Texture2D resume_button = LoadTexture("Assets/Textures/gui/resume_button.png");
    Texture2D resume_button_hovered = LoadTexture("Assets/Textures/gui/resume_button_hovered.png");
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
    Texture2D locked_level_icon = LoadTexture("Assets/Textures/gui/locked_level.png");
    Texture2D shop_button = LoadTexture("Assets/Textures/gui/shop_button.png");
    Texture2D shop_button_hovered = LoadTexture("Assets/Textures/gui/shop_button_hovered.png");
              checkbox = LoadTexture("Assets/Textures/gui/checkbox.png");
              checkbox_hovered = LoadTexture("Assets/Textures/gui/checkbox_hovered.png");
              checkbox_toggled = LoadTexture("Assets/Textures/gui/checkbox_checked.png");
              checkbox_toggled_hovered = LoadTexture("Assets/Textures/gui/checkbox_checked_hovered.png");
              plate_button = LoadTexture("Assets/Textures/gui/empty_button.png");
              plate_button_hovered = LoadTexture("Assets/Textures/gui/empty_button_hovered.png");
              shrinking_potion = LoadTexture("Assets/Textures/potion_bottle_absorption.png");
              cross_button = LoadTexture("Assets/Textures/gui/cross_button.png");
              cross_button_hovered = LoadTexture("Assets/Textures/gui/cross_button_hovered.png");
              knob = LoadTexture("Assets/Textures/gui/slider_ring_hovered.png");
              slider_bar = LoadTexture("Assets/Textures/gui/slider_background_middle.png");
              slider_bar_side = LoadTexture("Assets/Textures/gui/slider_background_right.png");

    Image window_icon = LoadImage("Assets/Textures/icon.png");
    std::vector<Sound*> all_sfx {
        &sfx.failsound,
        &sfx.win_sound,
        &sfx.notification_sound,
        &sfx.notification_sound_out,
        &sfx.power_up
    };

    ImageFormat(&window_icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(window_icon);
    bool win_sound_played = false;
    bool win_screen_loaded = true;
    bool QUITCONFIRMED = false;
    Lato = LoadFontEx("Assets/Fonts/Lato/Lato-Regular.ttf", 30, 0, 0);

    while(WindowShouldClose() == false and !QUITCONFIRMED){

        BeginDrawing();
        Vector2 mouse_pos = GetMousePosition();
        Rectangle p_rect = {player.position.x, player.position.y, player.p_width, player.p_height};
        for(auto& sound_effect : all_sfx){
            SetSoundVolume(*sound_effect, sfx_volume);
        }
        switch(gameState){
            case GAME: {

                switch (level){
                    case 1: {
                        play_level(
                            player,
                            lvl1_sawblades,
                            lvl1_blocks,
                            {512.0f, 32.0f, 32.0f, 32.0f},
                            player_sprite,
                            bg_lvl1,
                            sfx,
                            {96.0, 320.0},
                            sawblade
                        
                        );
                        DrawTextPro(GetFontDefault(), "Welcome to my game, GLHF!", {60.0f, 32.0f}, {0.0f, 0.0f}, 0.0f, 23.0f, 2.0f, Color{255, 255, 255, 255});
                        DrawTextPro(GetFontDefault(), "Made by @aymen-moh\n@github\n@Slack\n@hackclub", {366.5f, 301.0f}, {0.0f, 0.0f}, 0.0f, 23.0f, 2.0f, Color{255, 255, 255, 255});
                        break;
                    }
                    case 2: {
                        play_level(
                            player,
                            lvl2_sawblades,
                            lvl2_blocks,
                            {560.0, 336.0, 32.0f, 32.0f},
                            player_sprite,
                            bg_lvl1,
                            sfx,
                            {48.0, 336.0},
                            sawblade
                        );
                        break;
                    }
                    case 3: {
                        play_level(
                            player,
                            lvl3_sawblades,
                            lvl3_blocks,
                            {48.0, 320.0, 32.0f, 32.0f},
                            player_sprite,
                            bg_lvl1,
                            sfx,
                            {48.0, 160.0},
                            sawblade
                        ); 
                        DrawTextPro(GetFontDefault(), "This wouldve been a path to a star / coin if i didnt need to ship  soon.", {10.0f, 64.0f}, {0.0f, 0.0f}, 0.0f, 16.0f, 2.0f, Color{255, 255, 255, 255});
                        break;
                    }
                    case 4: {
                        play_level(
                            player,
                            lvl4_sawblades,
                            lvl4_blocks,
                            {560.0, 192.0, 32.0f, 32.0f},
                            player_sprite,
                            bg_lvl1,
                            sfx,
                            {32.0, 192.0},
                            sawblade,
                            lvl4_potion
                        );
                        break;
                        
                    }
                    case 5: {
                        play_level(
                            player,
                            lvl5_sawblades,
                            lvl5_blocks,
                            {48.0, 48.0, 32.0f, 32.0f},
                            player_sprite,
                            bg_lvl1,
                            sfx,
                            {48.0, 336.0},
                            sawblade
                        );
                        break;
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
                int i = level - 1;
                if (level == max_unlocked_level){
                    level_grid[level].locked = false;
                    max_unlocked_level++;
                }
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, menu_button_rect2) ? menu_button_hovered : menu_button, A_32x32_texure_source, menu_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, menu_button_rect2)) gameState = MENU;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, restart_button_rect2) ? restart_button_hovered : restart_button, A_32x32_texure_source, restart_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, restart_button_rect2)) gameState = RESTART;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, next_level_button_rect2) ? next_level_button_hovered : next_level_button, A_32x32_texure_source, next_level_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, next_level_button_rect2)){
                    onetimeloop = true;
                    gameState = GAME;
                    level++;
                }
                

                
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
                win_sound_played = false;
                onetimeloop = true;
                for(auto& saw : movingsawblades){
                saw.sb_pos_current = saw.sb_pos_a;
                confirm_restart = true;
                }
                gameState = GAME;

                break;
            }
            case MENU: {
                win_sound_played = false;
                onetimeloop = true;
                ClearBackground(GRAY);
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, play_button_rect) ? play_button_hovered : play_button, A_128_64Texture_source, play_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, play_button_rect)) gameState = LEVEL_MENU; 
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, settings_button_rect) ? settings_button_hovered : settings_button, A_128_64Texture_source, settings_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, settings_button_rect)) gameState = SETTINGS;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, quit_button_rect) ? quit_button_hovered : quit_button, A_128_64Texture_source, quit_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, quit_button_rect)) gameState = QUIT;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, shop_button_rect) ? shop_button_hovered : shop_button, A_128_64Texture_source, shop_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, shop_button_rect)) gameState = SHOPMENU;
                break;
            }
            case QUIT: {
                floating_window({195.5f, 180.f, 249.0f, 100.0f}, 1);
                
                DrawTextPro(Lato, "Press Enter to Quit.", {269.0f, 225.0f}, {MeasureTextEx(GetFontDefault(), "Press Enter to Quit.", 15.0f, 0.0f).x/2.0f, 0.0f}, 0.0f, 28, 2.0f, WHITE);
                if(IsKeyPressed(KEY_ENTER)) QUITCONFIRMED = true;
                if(IsKeyPressed(KEY_ESCAPE)) gameState = MENU;         
                
                break;
            }
            case LEVEL_MENU: {
                ClearBackground(BLACK);
                for(auto& button : level_grid){
                    
                    Rectangle button_rect = {button.position.x, button.position.y, button.size.x, button.size.y};
                    Texture2D hovered_or_not_texture = CheckCollisionPointRec(mouse_pos,button_rect) ? plate_button_hovered : plate_button;
                    
                    DrawTexturePro(button.locked ? locked_level_icon : hovered_or_not_texture, A_32x32_texure_source, button_rect, {0, 0}, 0, WHITE);
                    int fontsize;
                    Vector2 placement = {button.position.x + 8, button.position.y + 2.5f};
                    if(button.lvl_id == 1) placement = {button.position.x + 12, button.position.y + 2.5f};
                    if(button.lvl_id > 9) placement = {button.position.x + 4, button.position.y + 5.0f};
                    if(button.lvl_id > 9) fontsize = 22;
                    if(button.lvl_id > 99) fontsize = 18;
                    if(button.lvl_id < 9) fontsize = 30;
                    if(!button.locked) DrawText(std::to_string(button.lvl_id).c_str(),placement.x, placement.y, fontsize, WHITE);
                    if(CheckCollisionPointRec(mouse_pos, button_rect) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and !button.locked){
                        level = button.lvl_id;
                        gameState = GAME;
                    }
                }
                break;
            }
            case PAUSED: {
                ClearBackground(BLACK);
                const char* gameover = "PAUSED";
                DrawText(gameover, 65.0f, height/3.0f, 80.0f, WHITE);
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, menu_button_rect2) ? menu_button_hovered : menu_button, A_32x32_texure_source, menu_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, menu_button_rect2)) gameState = MENU;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, restart_button_rect2) ? restart_button_hovered : restart_button, A_32x32_texure_source, restart_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, restart_button_rect2)) gameState = RESTART;
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, resume_button_rect2) ? resume_button_hovered : resume_button, A_32x32_texure_source, next_level_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, resume_button_rect2)) gameState = GAME;
                break;
            }
            case SETTINGS: {
                Rectangle target = {77, 62, 500, 300};
                floating_window(target, true, 4);
                std::string title = "SETTINGS";
                static bool toggled = false; 
                static int test = 30;
                DrawTextPro(Lato, title.c_str(), {target.x + 200, target.y + 5}, {0, 0}, 0, 24, 2.0f, WHITE);
                switch(st_menu){
                    case 1: {
                        toggle({target.x + 12, target.y + 100}, sfx_enabled, "SFX");
                        toggle({target.x + 12, target.y + 125}, music_enabled, "MUSIC");
                        slider_toggle({target.x + 12, target.y + 150}, sfx_enabled, "SFiugguiguigvhgghghjkgghX", sfx_volume, 100, 100.0f, false, true, true, true, false);
                        
                        break;
                        
                    }
                    case 3: {
                        
                        toggle({target.x + 12, target.y + 100}, admin, "Admin");
                    }
                }
                    
            }
            case SHOPMENU: {
                
            }
        
        }
        Vector2 mos = GetMousePosition();
    int mos11 = mos.x;
    int mos22 = mos.y;
    std::string mos1 = std::to_string(mos11);
    std::string mos2 = std::to_string(mos22);
    DrawText(mos1.c_str(), 50, 50, 24, WHITE);
    DrawText(mos2.c_str(), 100, 50, 24, WHITE);
    EndDrawing();
    } 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UnloadTexture(plate_button);
    UnloadTexture(plate_button_hovered);
    UnloadTexture(menu_button);
    UnloadTexture(menu_button_hovered);
    UnloadTexture(restart_button_hovered);
    UnloadTexture(restart_button);
    UnloadTexture(next_level_button);
    UnloadTexture(next_level_button_hovered);
    UnloadTexture(cross_button);
    UnloadTexture(cross_button_hovered);
    UnloadFont(Lato);
    UnloadTexture(sawblade);

    UnloadTexture(bg_lvl1);
    UnloadImage(window_icon); 
    UnloadTexture(player_sprite);
    UnloadSound(sfx.failsound);
    UnloadSound(sfx.win_sound);
    UnloadSound(sfx.notification_sound);
    UnloadSound(sfx.notification_sound_out);
    CloseAudioDevice();
    CloseWindow();

}
