#pragma once
#include <stdint.h>

#include <cglm/struct.h>
#include <miniaudio.h>

#include "assets.h"
#include "camera.h"
#include "controller.h"
#include "game.h"
#include "mesh.h"
#include "sprite.h"

typedef struct World {
    Game game;

    Camera camera;
    Assets assets;

    Sprite deck[53];

    Vector ui_elements;

    GPUMesh game_gpu_mesh;
    Mesh game_mesh;

    ma_engine engine;

    Controller controller;
} World;

World world_init(void);

void world_free(World* world);
