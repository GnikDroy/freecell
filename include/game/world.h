#pragma once
#include <stdint.h>

#include <cglm/struct.h>
#include <miniaudio.h>

#include "core/game.h"

#include "game/animation.h"
#include "rendering/camera.h"
#include "rendering/mesh.h"
#include "rendering/sprite.h"

#include "game/assets.h"
#include "game/controller.h"

typedef struct World {
    GLFWwindow* window;
    Game game;

    Camera camera;
    Assets assets;

    Sprite deck[53];
    Sprite characters['~' - ' ' + 1];
    Sprite icon_clock;
    Sprite icon_game;

    Sprite button_undo;
    Sprite button_new_game;
    Sprite button_sound;

    Vector ui_elements;

    GPUMesh game_gpu_mesh;
    Mesh game_mesh;

    bool sound_enabled;
    ma_engine engine;

    ma_decoder card_move_decoder;
    ma_sound card_move_sound;

    Controller controller;
    AnimationSystem animation_system;
} World;

World world_init(GLFWwindow* window);

void world_free(World* world);
