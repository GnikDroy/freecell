#pragma once
#include <stdint.h>

#include "assets.h"
#include "camera.h"
#include "game.h"

typedef struct {
  Game game;
  Camera camera;
  Assets assets;
} World;

World world_init(void);

void world_free(World *world);
