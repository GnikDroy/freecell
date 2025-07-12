#include "world.h"

#include "assets.h"
#include "constants.h"
#include "utils.h"

World world_init(void) {
  World world;
  world.game = game_init();
  world.assets = assets_init();
  mat4_ortho(0.0f, (float)VIRTUAL_WIDTH, (float)VIRTUAL_HEIGHT, 0.0f, -1.0f,
             1.0f, world.projection);
  return world;
}

void world_free(World *world) { 
  game_free(&world->game); 
  assets_free(&world->assets);
}
