#pragma once
#include <stdint.h>

#include "assets.h"
#include "game.h"

typedef struct {
  Game game;
  float projection[16];
  Assets assets;
} World;

World world_init(void);

void world_free(World *world);
