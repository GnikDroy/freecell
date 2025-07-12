#pragma once

#include "image.h"
#include "sprite.h"

typedef struct {
  Image spritesheet;
  Sprite deck[53];
} Assets;

Assets assets_init();

void assets_free(Assets *assets);
