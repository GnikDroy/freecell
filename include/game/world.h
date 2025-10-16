#pragma once
#include <stdint.h>

#include <cglm/struct.h>
#include <miniaudio.h>

#include "game/game.h"
#include "game/animation.h"

#include "rendering/camera.h"
#include "rendering/mesh.h"
#include "rendering/sprite.h"

#include "game/assets.h"
#include "game/controller.h"

extern const Color BACKGROUND_COLOR;

typedef struct World {
    RGFW_window* window;
    Game game;

    Camera camera;
    Assets assets;

    Sprite deck[54];
    Sprite characters[256];

    Sprite button_undo;
    Sprite button_new_game;
    Sprite button_sound;

    Vector ui_elements;

    GPUMesh game_gpu_mesh;
    Mesh game_mesh;

    bool sound_enabled;
    ma_engine engine;

    bool show_help;

    ma_decoder card_move_decoder;
    ma_sound card_move_sound;

    Controller controller;
    AnimationSystem animation_system;
} World;

World world_init(RGFW_window* window);

void world_free(World* world);
