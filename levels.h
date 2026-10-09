#pragma once
#include <raylib.h>
#include <vector>
#include <string>

struct MovingSawblade {

    Vector2 sb_pos_a;
    Vector2 sb_pos_b;
    Vector2 sb_pos_current;
    bool sb_direction;
    float sb_speed;
    float sb_radius;
    float rotation = 0.0f;
};

struct Block {
    Vector2 position;
    Vector2 size;
    Color color;
};

struct LevelGrid {
    Vector2 position;
    Vector2 size;
    bool locked;
    int lvl_id;

};
struct PlayerIconShopItem {
    Rectangle rect;
    std::string name;
    int price;
    bool unlocked;
    Texture2D icon;
};

extern Texture2D player_icon;
extern Texture2D player_icon2;
extern Texture2D player_icon3;
extern Texture2D player_icon4;


extern std::vector<MovingSawblade> lvl1_sawblades;
extern std::vector<Block> lvl1_blocks;
extern std::vector<MovingSawblade> lvl2_sawblades;
extern std::vector<Block> lvl2_blocks;
extern std::vector<MovingSawblade> lvl3_sawblades;
extern std::vector<Block> lvl3_blocks;
extern std::vector<MovingSawblade> lvl4_sawblades;
extern std::vector<Block> lvl4_blocks;
extern Rectangle lvl4_potion;
extern std::vector<MovingSawblade> lvl5_sawblades;
extern std::vector<Block> lvl5_blocks;
extern std::vector<LevelGrid> level_grid;
extern std::vector<PlayerIconShopItem> player_icons;
extern std::string death_mesagges[10];
