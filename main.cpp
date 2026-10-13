#include <raylib.h>
#include <iostream>
#include <string>
#include <vector>
#include <raymath.h>
#include "levels.h"


int balance = 120;
Font Lato;
Font LatoBold;
int width;
bool music_enabled = true;
bool sfx_enabled = true;
bool admin = false; 
bool outline = true;
bool auto_respawn = false;
float sfx_volume = 1.0f;
float music_volume;
Color LIGHTBLUE = {0, 255, 255, 255};
bool rst_notf_sd = false;
int st_menu = 1;
int selected_player= 0;
Sound click_sound;
Music bg_music_1;
Music bg_music_2;

bool check_rectangle_equality(Rectangle Rectangle1, Rectangle Rectangle2){
    bool E_X = false;
    bool E_Y = false;
    bool E_WIDTH = false;
    bool E_HEIGHT = false;

    if (Rectangle1.x == Rectangle2.x) E_X = true;
    if (Rectangle1.y == Rectangle2.y) E_Y = true;
    if (Rectangle1.width == Rectangle2.width) E_WIDTH = true;
    if (Rectangle1.height == Rectangle2.height) E_HEIGHT = true;
    if (E_X and E_Y and E_HEIGHT and E_WIDTH) return true;
    else return false;
}

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
Texture2D coin;
Texture2D wintxt;

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
        PlaySound(click_sound);
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
void floating_window(Rectangle window_rect, std::string WindowTitle, bool& back, bool line = false, int sections = 0){

    DrawRectangleRec(window_rect, WHITE);
    Rectangle dest_rec = {window_rect.x + window_rect.width - 25, window_rect.y + 10.0f, 16.0f, 16.0f};
    Rectangle src_rec = {0, 0, 16, 16};
    DrawRectangleV({window_rect.x + 2.0f, window_rect.y + 2.0f}, {window_rect.width - 4.0f, window_rect.height - 4.0f}, BLACK);
    bool colliding = CheckCollisionPointRec(GetMousePosition(), dest_rec);
    DrawTexturePro(colliding ? cross_button_hovered : cross_button, src_rec, dest_rec, {0.0f, 0.0f}, 0, WHITE);
    if(colliding and IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {back = true; PlaySound(click_sound);}  // nah i won't
    switch(sections){
        case 1: {
            double fontsize = 18.0; 
            std::string section1 = "Audio";
            Vector2 sec1_size = MeasureTextEx(GetFontDefault(), section1.c_str(), 24, 2.0f);
            Rectangle sections_button_rect = {window_rect.x + 20, window_rect.y + 50, sec1_size.x, sec1_size.y};
            DrawTexturePro(CheckCollisionPointRec(GetMousePosition(), sections_button_rect) ? plate_button_hovered : plate_button, {0, 0, 32, 32}, sections_button_rect, {0, 0}, 0, WHITE);
            DrawTextPro(GetFontDefault(), section1.c_str(), {sections_button_rect.x + 7, sections_button_rect.y + 3}, {0, 0}, 0, fontsize, 2.0f, WHITE);
            if(CheckCollisionPointRec(GetMousePosition(), sections_button_rect) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { st_menu = 1; PlaySound(click_sound); }
            std::string section2 = "Controls";
            Vector2 sec2_size = MeasureTextEx(GetFontDefault(), section2.c_str(), 24, 2.0f);
            Rectangle sections2_button_rect = {sections_button_rect.x + sections_button_rect.width + 10, window_rect.y + 50, sec2_size.x, sec2_size.y};
            DrawTexturePro(CheckCollisionPointRec(GetMousePosition(), sections2_button_rect) ? plate_button_hovered : plate_button, {0, 0, 32, 32}, sections2_button_rect, {0, 0}, 0, WHITE);
            DrawTextPro(GetFontDefault(), section2.c_str(), {sections2_button_rect.x + 10, sections2_button_rect.y + 3}, {0, 0}, 0, fontsize, 2.0f, WHITE);
            if(CheckCollisionPointRec(GetMousePosition(), sections2_button_rect) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { st_menu = 2; PlaySound(click_sound); }


            std::string section3 = "Other";
            Vector2 sec3_size = MeasureTextEx(GetFontDefault(), section2.c_str(), 24, 2.0f);
            Rectangle sections3_button_rect = {sections2_button_rect.x + sections2_button_rect.width + 10, window_rect.y + 50, sec3_size.x, sec3_size.y};
            DrawTexturePro(CheckCollisionPointRec(GetMousePosition(), sections3_button_rect) ? plate_button_hovered : plate_button, {0, 0, 32, 32}, sections3_button_rect, {0, 0}, 0, WHITE);
            DrawTextPro(GetFontDefault(), section3.c_str(), {sections3_button_rect.x + 10, sections3_button_rect.y + 3}, {0, 0}, 0, fontsize, 2.0f, WHITE);
            if(CheckCollisionPointRec(GetMousePosition(), sections3_button_rect) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { st_menu = 3; PlaySound(click_sound); }
            break;
        }
        case 2: {
            if(line){
                DrawLine(window_rect.x, window_rect.y + 35, window_rect.x + window_rect.width, window_rect.y + 35, WHITE);
                float titlewidth = MeasureText(WindowTitle.c_str(), 24);
                DrawTextPro(Lato, WindowTitle.c_str(), {window_rect.x + window_rect.width / 2 - titlewidth / 2, window_rect.y + 7}, {0, 0}, 0, 24, 2.0f, WHITE);

            }
        }
    }
 

}
void draw_currency(Vector2 position = {560, 7}){
    int fontssize = 16;
    std::string balance_str = std::to_string(balance);
    Vector2 balance_width = {MeasureTextEx(GetFontDefault(), balance_str.c_str(), 18, 2.0f).x + 26, MeasureTextEx(GetFontDefault(), balance_str.c_str(), 18, 2.0f).y + 5};
    DrawRectangle(position.x, position.y, balance_width.x, balance_width.y, WHITE);
    DrawRectangle(position.x + 1, position.y + 1, balance_width.x - 2, balance_width.y - 2, BLACK);
    DrawTexturePro(coin, {0, 0, 8, 8}, {position.x + 4, position.y + 4, balance_width.y - 8, balance_width.y - 8}, {0, 0}, 0, WHITE);
    DrawText(balance_str.c_str(), position.x + 24, position.y + 4, fontssize, GOLD);
}

void push_notification(std::string text, float duration, int pos_change, float anim_speed, bool reset, Color Colour = WHITE, Font font = GetFontDefault()){
    float init_current_y = -30.0f;
    Vector2 text_length = MeasureTextEx(font, text.c_str(), 20, 2.0f);
    static Vector2 current = {1, init_current_y};
    current.x = width / 2 - text_length.x / 2;
    static float time_spent = 0.0f;
    static bool play_end_animation = false;
    static bool bool1 = true;
    static bool bool2 = true;
    static bool play_start_animation = true;
    if(reset){
        current = {320.0f, init_current_y};
        time_spent = 0.0f;
        play_end_animation = false;
        bool1 = true;
        bool2 = true;
        play_start_animation = true;
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
    DrawTextPro(font, text.c_str(), {current.x, current.y}, {0, 0}, 0, 20, 2.0f, Colour);
    
    
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
    Rectangle shrink_potion = default_potion
    
){
    static bool shrunk = false;
    static bool dashing = false;
    static float dash_time = 0.0f;
    static Vector2 mov_dir = {0, 0};
    Vector2 dir = {(float)(IsKeyDown(KEY_D)) - (IsKeyDown(KEY_A)), (float)(IsKeyDown(KEY_W)) - (IsKeyDown(KEY_S))};
    static bool shrinking_sound = true;
    static bool potion_collected = false;

    static bool disappear = false;
    if(onetimeloop){
        potion_collected = false;
        shrinking_sound = false;
        shrunk = false;
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
            if(!auto_respawn) gameState = GAME_OVER; 
            if(auto_respawn) gameState = RESTART;
            if(!IsSoundPlaying(sfx.failsound)) PlaySound(sfx.failsound);

        }
    }
    if(IsKeyPressed(KEY_ESCAPE)) gameState = PAUSED;
    

    /////////////////////////////////////////////////////////////
    
    DrawTexturePro(bg, {0, 0, 20, 13}, {0, 0, 640, 416}, {0, 0}, 0, WHITE);
    if(outline) for(const auto& block : blocks)DrawRectangleV({block.position.x - 3, block.position.y - 3}, {block.size.x + 6, block.size.y + 6}, BLACK);
    for(const auto& block : blocks)DrawRectangleV(block.position, block.size, outline ? block.color : DARKGRAY);
    for(const auto& saw : movingsawblades){

        Rectangle src_rec = {0.0f, 0.0f, static_cast<float>(sawblade.width), static_cast<float>(sawblade.height)};
        Rectangle dest_rec = {saw.sb_pos_current.x, saw.sb_pos_current.y, saw.sb_radius * 2.3f, saw.sb_radius * 2.3f};
        DrawTexturePro(sawblade, src_rec, dest_rec, {dest_rec.width / 2.0f, dest_rec.height / 2.0f}, saw.rotation, WHITE);

    }   

    
        
        

    
    DrawTexturePro(player_icon, {0, 0, 16, 16}, player.player, {0, 0}, 0, WHITE);
    DrawFPS(50, 10);
    DrawTexturePro(wintxt, {0, 0, 8, 8}, {win.x - 3, win.y - 3, win.width + 6, win.height + 6}, {0, 0}, 0, BLACK);
    DrawTexturePro(wintxt, {0, 0, 8, 8}, win, {0, 0}, 0, WHITE);
    if(CheckCollisionRecs(player.player, win)) gameState = WIN_SCREEN;
    bool level_has_potion = shrink_potion.width > 0 and shrink_potion.height > 0;
    if(level_has_potion and !potion_collected){
        if(CheckCollisionRecs(player.player, shrink_potion)){
            potion_collected = true;
            shrunk = true;
        }
        DrawTexturePro(shrinking_potion, {3, 1, 13, 15}, shrink_potion, {0, 0}, 0, WHITE); ///// GD REFRENCEEEEE
        
    }
    if(shrunk) shrink(player, 1.5, shrinking_sound);
    push_notification("hi hru", 2.0f, 100.0f, 2.0f, rst_notf_sd);
    
    
}
 
int main(int argc, char* argv[]) {

    
    

//

    
    std::vector<MovingSawblade> movingsawblades {
        
    };

    std::vector<Block> blocks {
        
    }; 
    
    Rectangle A16_Texture_source = {0, 0, 16, 16};
    Rectangle x32 = {0, 0, 32, 32};
    Rectangle A_128_64Texture_source = {0, 0, 128, 64};
    Rectangle menu_button_rect2 = {192.0, 256.0, 64.0f, 64.0f};
    Rectangle restart_button_rect2 = {288.0, 256.0, 64.0f, 64.0f};
    Rectangle next_level_button_rect2 = {384.0, 256.0, 64.0f, 64.0f};
    Rectangle resume_button_rect2 = {384.0, 256.0, 64.0f, 64.0f};
    Rectangle play_button_rect = {32.0, 160.0, 96.0f, 32.0f};
    Rectangle settings_button_rect = {32.0, 208.0, 96.0f, 32.0f};
    Rectangle shop_button_rect = {32.0, 256.0, 96.0f, 32.0f}; 
    Rectangle quit_button_rect = {32.0, 304.0, 96.0f, 32.0f}; 

 
    width = 640;
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
    click_sound = LoadSound("Assets/audio/sound effects/Click_stereo.ogg.mp3");
    bg_music_1 = LoadMusicStream();
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

              wintxt = LoadTexture("Assets/Textures/win.png");
              coin = LoadTexture("Assets/Textures/gui/coin.png");
              player_icons[0].icon = LoadTexture("Assets/Textures/Icon.png");
              player_icons[1].icon = LoadTexture("Assets/Textures/Icon2.png");
              player_icons[2].icon = LoadTexture("Assets/Textures/Icon3.png");
              player_icons[3].icon = LoadTexture("Assets/Textures/Icon4.png");
              player_icons[4].icon = LoadTexture("Assets/Textures/Icon5.png");
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
    LatoBold = LoadFontEx("Assets/Fonts/Lato/Lato-Bold.ttf", 30, 0, 0);
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
                            player_icons[selected_player].icon,
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
                            player_icons[selected_player].icon,
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
                            player_icons[selected_player].icon,
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
                            player_icons[selected_player].icon,
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
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {48.0, 336.0},
                            sawblade
                        );
                        break;
                    }
                    case 6: {
                        play_level(
                            player,
                            lvl6_sawblades,
                            lvl6_blocks,
                            {384.0, 64.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {224, 64},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 7: {
                        play_level(
                            player,
                            lvl7_sawblades,
                            lvl7_blocks,
                            {576, 32, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {64.0, 48.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 8: {
                        play_level(
                            player,
                            lvl8_sawblades,
                            lvl8_blocks,
                            {384.0, 64.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {224.0, 64.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 9: {
                        play_level(
                            player,
                            lvl9_sawblades,
                            lvl9_blocks,
                            {560.0, 160.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {64.0, 176.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 10: {
                        play_level(
                            player,
                            lvl10_sawblades,
                            lvl10_blocks,
                            {432.0, 48.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {176.0, 48.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 11: {
                        play_level(
                            player,
                            lvl11_sawblades,
                            lvl11_blocks,
                            {560.0, 336.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {48.0, 336.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 12: {
                        play_level(
                            player,
                            lvl12_sawblades,
                            lvl12_blocks,
                            {544.0, 160.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {48.0, 192.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 13: {
                        play_level(
                            player,
                            lvl13_sawblades,
                            lvl13_blocks,
                            {432.0, 192.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {64.0, 48.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 14: {
                        play_level(
                            player,
                            lvl14_sawblades,
                            lvl14_blocks,
                            {64.0, 352.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {545.0, 48.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                    case 15: {
                        play_level(
                            player,
                            lvl15_sawblades,
                            lvl15_blocks,
                            {576.0, 64.0, 32.0f, 32.0f},
                            player_icons[selected_player].icon,
                            bg_lvl1,
                            sfx,
                            {80.0, 32.0},
                            sawblade
                            
                        );
                        break;
                        
                    }
                }
                break;
            }
            case WIN_SCREEN: {
                static int coin_bonus = 0;
                DrawRectangle(0, 0, 640.0f, 416.0f, BLACK);
                
                if (win_sound_played == false){
                    PlaySound(sfx.win_sound);    
                    win_sound_played = true;
                    if (level == max_unlocked_level){
                        level_grid[level].locked = false;
                        coin_bonus = 100;
                        balance += coin_bonus;
                        max_unlocked_level++;
                    }
                    else {
                        coin_bonus = 0;
                    }
                }
                
                
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, menu_button_rect2) ? menu_button_hovered : menu_button, x32, menu_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, menu_button_rect2)) { gameState = MENU; PlaySound(click_sound); }
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, restart_button_rect2) ? restart_button_hovered : restart_button, x32, restart_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, restart_button_rect2)) { gameState = RESTART; PlaySound(click_sound); }
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, next_level_button_rect2) ? next_level_button_hovered : next_level_button, x32, next_level_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, next_level_button_rect2)){
                    onetimeloop = true;
                    win_sound_played = false;
                    gameState = GAME;
                    level++;
                    PlaySound(click_sound);
                }

                

                
                break;
            }
            case GAME_OVER: {
                
                ClearBackground(BLACK);
                const char* gameover = "GAME OVER!";
                DrawText(gameover, 65.0f, height/3.0f, 80.0f, WHITE);
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, {224.0, 256.0, 64.0f, 64.0f}) ? menu_button_hovered : menu_button, x32, {224.0, 256.0, 64.0f, 64.0f}, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, {224.0, 256.0, 64.0f, 64.0f})) {gameState = MENU; PlaySound(click_sound);}
                
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, {352.0, 256.0, 64.0f, 64.0f}) ? restart_button_hovered : restart_button, x32, {352.0, 256.0, 64.0f, 64.0f}, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, {352.0, 256.0, 64.0f, 64.0f})) {gameState = RESTART; PlaySound(click_sound);}
                
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
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, play_button_rect)) {gameState = LEVEL_MENU; PlaySound(click_sound);}
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, settings_button_rect) ? settings_button_hovered : settings_button, A_128_64Texture_source, settings_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, settings_button_rect)) {gameState = SETTINGS; PlaySound(click_sound);}
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, quit_button_rect) ? quit_button_hovered : quit_button, A_128_64Texture_source, quit_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, quit_button_rect)) {gameState = QUIT; PlaySound(click_sound);}
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, shop_button_rect) ? shop_button_hovered : shop_button, A_128_64Texture_source, shop_button_rect, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, shop_button_rect)) {gameState = SHOPMENU; PlaySound(click_sound);}
                break;
            }
            case QUIT: {
                bool back = false;
                floating_window({195.5f, 180.f, 249.0f, 100.0f}, "QUIT?", back, true, 2);
                if(back) gameState = MENU;

                DrawTextPro(Lato, "Press Enter to Quit.", {269.0f, 225.0f}, {MeasureTextEx(GetFontDefault(), "Press Enter to Quit.", 15.0f, 0.0f).x/2.0f, 0.0f}, 0.0f, 28, 2.0f, WHITE);
                if(IsKeyPressed(KEY_ENTER)) QUITCONFIRMED = true;
                if(IsKeyPressed(KEY_ESCAPE)) gameState = MENU;         
                
                break;
            }
            case LEVEL_MENU: {
                ClearBackground(BLACK);
                for(auto& button : level_grid){
                    if(admin) button.locked = false;
                    Rectangle button_rect = {button.position.x, button.position.y, button.size.x, button.size.y};
                    Texture2D hovered_or_not_texture = CheckCollisionPointRec(mouse_pos,button_rect) ? plate_button_hovered : plate_button;
                    
                    if(button.exist)DrawTexturePro(button.locked ? locked_level_icon : hovered_or_not_texture, x32, button_rect, {0, 0}, 0, WHITE);
                    int fontsize;
                    Vector2 placement = {button.position.x + 8, button.position.y + 2.5f};
                    if(button.lvl_id == 1) placement = {button.position.x + 12, button.position.y + 2.5f};
                    if(button.lvl_id > 9) placement = {button.position.x + 4, button.position.y + 5.0f};
                    if(button.lvl_id > 9) fontsize = 22;
                    if(button.lvl_id > 99) fontsize = 18;
                    if(button.lvl_id < 9) fontsize = 30;
                    if(button.exist) if(!button.locked) DrawText(std::to_string(button.lvl_id).c_str(),placement.x, placement.y, fontsize, WHITE);
                    if(CheckCollisionPointRec(mouse_pos, button_rect) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and !button.locked){
                        level = button.lvl_id;
                        PlaySound(click_sound);
                        gameState = GAME;
                    }
                }
                break;
            }
            case PAUSED: {
                ClearBackground(BLACK);
                const char* gameover = "PAUSED";
                DrawText(gameover, 65.0f, height/3.0f, 80.0f, WHITE);
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, menu_button_rect2) ? menu_button_hovered : menu_button, x32, menu_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, menu_button_rect2)) {gameState = MENU; PlaySound(click_sound);}
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, restart_button_rect2) ? restart_button_hovered : restart_button, x32, restart_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, restart_button_rect2)) {gameState = RESTART; PlaySound(click_sound);}
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, resume_button_rect2) ? resume_button_hovered : resume_button, x32, next_level_button_rect2, {0, 0}, 0, WHITE);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, resume_button_rect2)) {gameState = GAME; PlaySound(click_sound);}
                break;
            }
            case SETTINGS: {
                Rectangle target = {77, 62, 500, 300};
                bool back = false;
                floating_window(target, "SETTINGS", back, true, 1);
                if(back) gameState = MENU;
                std::string title = "SETTINGS";
                static bool toggled = false; 
                static int test = 30;
                DrawTextPro(Lato, title.c_str(), {target.x + 200, target.y + 5}, {0, 0}, 0, 24, 2.0f, WHITE);
                switch(st_menu){
                    case 1: {
                        slider_toggle({target.x + 12, target.y + 150}, sfx_enabled, "SFX", sfx_volume, 100, 100.0f, false, true, true, true, false);
                        toggle({target.x + 12, target.y + 125}, music_enabled, "MUSIC");
                        
                        
                        break;
                        
                    }
                    case 3: {
                        
                        toggle({target.x + 12, target.y + 100}, admin, "Admin");
                        toggle({target.x + 12, target.y + 125}, outline, "Block Outline");
                        toggle({target.x + 12, target.y + 150}, auto_respawn, "Auto Respawn");
                    }
                }
                break;
                    
            }
            case SHOPMENU: {
                ClearBackground(BLACK);
                static Rectangle rect  = {30, 66, 100, 100};
                DrawLine(320, 0, 320, 416, WHITE);
                static int current_screen = 1;
                static int icon_name = 0;
                static bool rst_notf = false;
                static bool show_notf = false;
                
                static std::string purchase_successful;
                /////////////////////////////////
                DrawLine(320, 0, 320, 416, WHITE);
                for(auto& icon : player_icons){
                    if(admin) icon.unlocked = true;
                    switch(current_screen){
                        case 1: {
                            bool hovered = (CheckCollisionPointRec(mouse_pos, icon.rect));
                            bool selected = false;
                            
                            
                            if(icon.unlocked){
                                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and hovered){
                                    selected_player = icon.player_id;
                                    PlaySound(click_sound);
                                }
                                if (selected_player == icon.player_id){
                                    selected = true;
                                }
                            }
                            if(!icon.unlocked and IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and CheckCollisionPointRec(mouse_pos, icon.rect)){
                                PlaySound(click_sound);
                                icon_name = icon.player_id;
                                current_screen = 2;
                            }
                            
                            DrawTexturePro(selected ? plate_button_hovered : hovered ? plate_button_hovered : plate_button, x32, icon.rect, {0, 0}, 0, selected ? LIGHTBLUE :  icon.unlocked ? GREEN : RED);
                            DrawTexturePro(icon.icon, {0, 0, 16, 16}, {icon.rect.x + 11, icon.rect.y + 9, 48, 48}, {0, 0}, 0, WHITE);
                            if(!icon.unlocked){
                                DrawTexturePro(coin, {0, 0, 8, 8}, {icon.rect.x + 10, icon.rect.y + 61, 12, 12}, {0, 0}, 0, WHITE);
                                DrawText(std::to_string(icon.price).c_str(), icon.rect.x + 24, icon.rect.y + 61, 14, GOLD);
                                
                            }
                            break;
                            }
                        case 2:{
                            bool back = false;
                            if(!back){
                                bool insufficient_bal = false;
                                if(balance < player_icons[icon_name].price) insufficient_bal = true;
                                Rectangle ft_rect = {200, 150, 240, 150};
                                floating_window(ft_rect, "Purchase?", back, true, 2);
                                Rectangle button_rect1 = {ft_rect.x + 30, ft_rect.y + 110, 50, 30};
                                bool hovered = CheckCollisionPointRec(mouse_pos, button_rect1);
                                DrawTexturePro(hovered ? plate_button_hovered : plate_button, x32, button_rect1, {0, 0}, 0, WHITE);
                                std::string no = "No";
                                DrawTextPro(LatoBold, no.c_str(), {button_rect1.x + 12, button_rect1.y + 3}, {0, 0}, 0, 24, 2.0f, WHITE);
                                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and hovered){
                                    PlaySound(click_sound);
                                    back = true;
                                    show_notf = false;
                                }
                                std::string yes = "Yes";
                                Rectangle button_rect2 = {ft_rect.x + 160, ft_rect.y + 110, 50, 30};
                                bool hovered2 = CheckCollisionPointRec(mouse_pos, button_rect2);
                                DrawTexturePro(insufficient_bal ? plate_button : hovered2 ? plate_button_hovered : plate_button, x32, button_rect2, {0, 0}, 0, WHITE);
                                DrawTextPro(LatoBold, yes.c_str(), {button_rect2.x + 7, button_rect2.y + 3}, {0, 0}, 0, 24, 2.0f, insufficient_bal ? GRAY : WHITE);
                                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and hovered2 and !insufficient_bal){
                                    if(balance > player_icons[icon_name].price){
                                        PlaySound(click_sound);
                                        player_icons[icon_name].unlocked = true;
                                        show_notf = true;
                                        rst_notf = true;
                                        purchase_successful = purchase_successful = TextFormat("You Just Bought %s for %d coins!", player_icons[icon_name].name.c_str(), player_icons[icon_name].price); 
                                        balance -= player_icons[icon_name].price;
                                        back = true;
                                    }
                                }
                                
                                std::string purcofrm = "Do You Wish To Buy";
                                static Vector2 length1111 = MeasureTextEx(LatoBold, purcofrm.c_str(), 18, 2.0f);
                                DrawTextPro(LatoBold, purcofrm.c_str(), {ft_rect.x + 13+ ft_rect.x / 2 - length1111.x / 2, ft_rect.y + 35}, {0, 0}, 0, 18, 2.0f, WHITE);
                                Vector2 icon_name_length = MeasureTextEx(LatoBold, player_icons[icon_name].name.c_str(), 24, 2.0f);
                                DrawTextPro(LatoBold, player_icons[icon_name].name.c_str(), {ft_rect.x + ft_rect.width / 2 - icon_name_length.x / 2, ft_rect.y + 50}, {0, 0}, 0, 24, 2.0f, LIGHTBLUE);
                                std::string confirmtxt2 = "For";
                                DrawTextPro(LatoBold, confirmtxt2.c_str(), {ft_rect.x + 83, ft_rect.y + 70}, {0,0}, 0, 18, 2.0f, WHITE);
                                DrawTextPro(LatoBold, std::to_string(player_icons[icon_name].price).c_str(), {ft_rect.x + 131, ft_rect.y + 70}, {0,0}, 0, 18, 2.0f, GOLD);
                                DrawTexturePro(coin, {0, 0, 8, 8}, {ft_rect.x + 117, ft_rect.y + 74, 12, 12}, {0, 0}, 0, WHITE);
                                std::string confirmtxt3 = "?";
                                Vector2 balance_length = MeasureTextEx(LatoBold, std::to_string(player_icons[icon_name].price).c_str(), 18, 2.0f);
                                DrawTextPro(LatoBold, confirmtxt3.c_str(), {ft_rect.x + 133 + balance_length.x, ft_rect.y + 70}, {0, 0}, 0, 18, 2.0f, WHITE);
                            }
                            if(back){
                                current_screen = 1;
                                rst_notf = true;
                        }
                    }
                    }

                    
                    
                }
                /////////////////////////////////
                
                DrawRectangle(0, 0, 650, 36, BLACK);
                DrawRectangle(0, 394, 640, 50, BLACK);
                DrawLine(0, 36, 640, 36, WHITE);
                DrawLine(0, 394, 640, 394, WHITE);
                DrawTexturePro(CheckCollisionPointRec(mouse_pos, {5, 5, 24, 24}) ? resume_button_hovered : resume_button, {0, 0, -32, 32}, {5, 5, 24, 24}, {0, 0}, 0, WHITE);
                if(CheckCollisionPointRec(mouse_pos, {5, 5, 24, 24}) and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {gameState = MENU; PlaySound(click_sound);}
                draw_currency();
                if(show_notf) push_notification(purchase_successful, 2.0f, 37, 2.0f, rst_notf, LIME, LatoBold);
                if(show_notf) rst_notf = false;

                break;
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
    UnloadTexture(player_icon);
    UnloadSound(sfx.failsound);
    UnloadSound(sfx.win_sound); 
    UnloadSound(sfx.notification_sound);
    UnloadSound(sfx.notification_sound_out);
    CloseAudioDevice();
    CloseWindow();

}
