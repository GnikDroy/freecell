#pragma once

#include "game.h"
#include "image.h"
#include <stdint.h>

typedef struct {
  Game game;
  Image spritesheet;
  float projection[16];
} World;

const char card_spritesheet[] = {
#embed "assets/cards.png"
};

World world_init(void) {
  World world;
  world.game = game_init();
  world.spritesheet = image_load_from_memory((const uint8_t *)card_spritesheet,
                         sizeof(card_spritesheet), 4);
  return world;
}

void world_free(World *world) {
  game_free(&world->game);
  image_free(&world->spritesheet);
}
