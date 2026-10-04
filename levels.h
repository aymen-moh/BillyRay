#pragma once
#include <raylib.h>
#include <vector>


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