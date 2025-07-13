#include "world.h"

#include "assets.h"
#include "card_sprites.h"
#include "constants.h"
#include "ui_element.h"
#include "utils.h"

World world_init(void) {
  World world = {0};
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

  mat4_ortho(0.0f, (float)VIRTUAL_WIDTH, (float)VIRTUAL_HEIGHT, 0.0f, -1000.0f,
             1.0f, camera.projection);
  world.camera = camera;

  generate_card_sprites(world.deck);

  world.ui_elements = vec_init(sizeof(UIElement));

  world.game_mesh = mesh_init();
  world.game_gpu_mesh = gpu_mesh_init();

  world.controller.layout_pending = true;
  world.controller.bake_pending = true;
  return world;
}

void world_free(World *world) {
  game_free(&world->game);
  assets_free(&world->assets);

  vec_free(&world->ui_elements);

  gpu_mesh_free(&world->game_gpu_mesh);
  mesh_free(&world->game_mesh);
}
