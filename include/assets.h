#pragma once

#include "image.h"
#include "shader.h"
#include "texture.h"

typedef struct Assets {
  Image spritesheet;
  Shader main_shader;
  Texture spritesheet_texture;
} Assets;

Assets assets_init();

void assets_free(Assets *assets);
