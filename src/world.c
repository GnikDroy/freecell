#include "world.h"

#include "assets.h"
#include "constants.h"
#include "utils.h"

World world_init(void) {
  World world;
  world.game = game_init();
  world.assets = assets_init();

  Camera camera = {
      .view =
          {
              1.0f, 0.0f, 0.0f, 0.0f, // Column 0
              0.0f, 1.0f, 0.0f, 0.0f, // Column 1
              0.0f, 0.0f, 1.0f, 0.0f, // Column 2
              0.0f, 0.0f, 0.0f, 1.0f  // Column 3
          },
  };

  mat4_ortho(0.0f, (float)VIRTUAL_WIDTH, (float)VIRTUAL_HEIGHT, 0.0f, -1.0f,
             1.0f, camera.projection);

  world.camera = camera;
  return world;
}

void world_free(World *world) {
  game_free(&world->game);
  assets_free(&world->assets);
}
