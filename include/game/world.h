#pragma once
#include <stdint.h>

#include <cglm/struct.h>
#include <miniaudio.h>

#include "core/game.h"

#include "rendering/camera.h"
#include "rendering/sprite.h"
#include "rendering/mesh.h"

#include "game/controller.h"
#include "game/assets.h"


typedef struct World {
    Game game;

    Camera camera;
    Assets assets;

    Sprite deck[53];

    Vector ui_elements;

    GPUMesh game_gpu_mesh;
    Mesh game_mesh;

    ma_engine engine;

    ma_decoder card_move_decoder;
    ma_sound card_move_sound;

    Controller controller;
} World;

World world_init(void);

void world_free(World* world);
