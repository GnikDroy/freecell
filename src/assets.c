#include "assets.h"
#include "constants.h"
#include "shader.h"
#include "texture.h"

const char MAIN_SPRITESHEET[] = {
#embed "assets/spritesheet.png"
};

Assets assets_init() {
  Assets assets;
  assets.spritesheet = image_load_from_memory((const uint8_t *)MAIN_SPRITESHEET,
                                              sizeof(MAIN_SPRITESHEET), 4);
  assets.spritesheet_texture = texture_init(&assets.spritesheet);
  assets.main_shader =
      shader_init(MAIN_VERTEX_SHADER_SOURCE, MAIN_FRAGMENT_SHADER_SOURCE);
  if (assets.main_shader == 0) {
    exit(EXIT_FAILURE);
  }
  return assets;
}

void assets_free(Assets *assets) {
  image_free(&assets->spritesheet);
  shader_free(&assets->main_shader);
  texture_free(&assets->spritesheet_texture);
}
