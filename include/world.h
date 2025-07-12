#pragma once
#include <stdint.h>

#include "assets.h"
#include "camera.h"
#include "game.h"
#include "mesh.h"
#include "sprite.h"

typedef struct {
  Game game;
  Camera camera;
  Assets assets;

  Sprite deck[53];

  Vector ui_elements;

  GPUMesh game_gpu_mesh;
  Mesh game_mesh;

  GPUMesh hitbox_gpu_mesh;
  Mesh hitbox_mesh;

} World;

World world_init(void);

void world_free(World *world);
